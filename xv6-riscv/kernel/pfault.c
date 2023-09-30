/* This file contains code for a generic page fault handler for processes. */
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "elf.h"

#include "sleeplock.h"
#include "fs.h"
#include "buf.h"

#define VIRTUAL_ADDR_OFFSTET_BITS 12

int loadseg(pagetable_t pagetable, uint64 va, struct inode *ip, uint offset, uint sz);
int flags2perm(int flags);

uint64 find_faulting_base_addr(void);
bool is_cow(struct proc*, uint64);

void load_binary_page(struct proc *p, uint64 faulting_page_addr);

void load_heap_page(struct proc*, int);
int get_heap_tracker_index(struct proc*, uint64);

int get_contiguous_psa_blockno(int blocks);
int get_oldest_heap_page_index(struct proc*);
void set_heap_tracker_for_moving_to_disk(struct heap_tracker_t*, int);
uchar* copy_to_kernel_space(struct proc*, struct heap_tracker_t*);
void write_victim_page_to_PSA(int, uchar*);

void load_page_from_PSA(struct proc*, int, int);
void set_heap_tracker_for_moving_back(struct heap_tracker_t*);

/* CSE 536: (2.4) read current time. */
uint64 read_current_timestamp() {
    uint64 curticks = 0;

    acquire(&tickslock);
    curticks = ticks;
    wakeup(&ticks);
    release(&tickslock);

    return curticks;
}

bool psa_tracker[PSASIZE];

/* All blocks are free during initialization. */
void init_psa_regions(void)
{
    for (int i = 0; i < PSASIZE; i++)
        psa_tracker[i] = false;
}

/* Evict heap page to disk when resident pages exceed limit */
void evict_page_to_disk(struct proc* p) {
    /* Find free block */
    int blockno = get_contiguous_psa_blockno(4);
    /* Find victim page using FIFO. */
    int oldest_heap_page_index = get_oldest_heap_page_index(p);
    set_heap_tracker_for_moving_to_disk(&(p->heap_tracker[oldest_heap_page_index]), blockno);
    /* Read memory from the user to kernel memory first. */
    uchar* kernel_space_addr = copy_to_kernel_space(p, &(p->heap_tracker[oldest_heap_page_index]));
    write_victim_page_to_PSA(blockno, kernel_space_addr);

    /* Write to the disk blocks. Below is a template as to how this works. There is
     * definitely a better way but this works for now. :p */

    /* Unmap swapped out page */
    /* Update the resident heap tracker. */
    uvmunmap(p->pagetable, p->heap_tracker[oldest_heap_page_index].addr, 1, 0);
    p->resident_heap_pages--;
    /* Print statement. */
    print_evict_page(p->heap_tracker[oldest_heap_page_index].addr, p->heap_tracker[oldest_heap_page_index].startblock);
}

int get_contiguous_psa_blockno(int blocks) {
    uint64 rest_blocks = blocks;

    for(int i = 0;i < PSASIZE;i++) {
        if(psa_tracker[i])
            rest_blocks = blocks;
        else {
            rest_blocks--;
            if(rest_blocks == 0)
                return i - blocks + 1;
        }
    }

    return -1;
}

int get_oldest_heap_page_index(struct proc *p) {
    uint64 oldest_time = p->heap_tracker[0].last_load_time;
    int oldest_index = 0;

    for(int i = 1;i < MAXHEAP;i++) {
        if(p->heap_tracker[i].last_load_time < oldest_time) {
            oldest_index = i;
            oldest_time = p->heap_tracker[i].last_load_time;
        }
    }

    return oldest_index;
}

void set_heap_tracker_for_moving_to_disk(struct heap_tracker_t *h, int blockno) {
    h->startblock = blockno;
    h->last_load_time = 0xFFFFFFFFFFFFFFFF;
}

uchar* copy_to_kernel_space(struct proc *p, struct heap_tracker_t *h) {
    uchar *kernel_space_addr = kalloc();
    copyin(p->pagetable, (char*)kernel_space_addr, h->addr, PGSIZE);

    return kernel_space_addr;
}

void write_victim_page_to_PSA(int start_blockno, uchar *kernel_space_addr) {
    struct buf* b;

    // Copy page contents to b.data using memmove.
    for(int offset = 0, blockno = start_blockno;offset < PGSIZE; offset += BSIZE, blockno++) {
        b = bread(1, PSASTART + blockno);
        memmove(b->data, kernel_space_addr + offset, BSIZE);
        bwrite(b);
        brelse(b);
        psa_tracker[blockno] = true;
    }
}

/* Retrieve faulted page from disk. */
void retrieve_page_from_disk(struct proc* p, int heap_tracker_index) {
    /* Find where the page is located in disk */
    int blockno = p->heap_tracker[heap_tracker_index].startblock;
    load_page_from_PSA(p, heap_tracker_index, blockno);
    set_heap_tracker_for_moving_back(&(p->heap_tracker[heap_tracker_index]));

    /* Print statement. */
    print_retrieve_page(p->heap_tracker[heap_tracker_index].addr, blockno);
}

void load_page_from_PSA(struct proc *p, int heap_tracker_index, int start_blockno) {
    /* Create a kernel page to read memory temporarily into first. */
    /* Read the disk block into temp kernel page. */
    /* Copy from temp kernel page to uvaddr (use copyout) */

    uint64 va = p->heap_tracker[heap_tracker_index].addr;
    struct buf *b;

    for(int blockno = start_blockno;blockno < 4;blockno++, va += BSIZE) {
        b = bread(1, PSASTART + blockno);
        copyout(p->pagetable, va, (char*)b->data, BSIZE);
        brelse(b);
        psa_tracker[blockno] = false;
    }
}

void set_heap_tracker_for_moving_back(struct heap_tracker_t *h) {
    h->last_load_time = read_current_timestamp();
    h->startblock = -1;
}

void page_fault_handler(void)
{
    /* Current process struct */
    struct proc *p = myproc();

    /* Find faulting address. */
    uint64 faulting_addr = find_faulting_base_addr();
    print_page_fault(p->name, faulting_addr);

    if(is_cow(p, faulting_addr)) {
        copy_on_write(p, faulting_addr);
        goto out;
    }

    /* Check if the fault address is a heap page. Use p->heap_tracker */
    int heap_tracker_index;
    if ((heap_tracker_index = get_heap_tracker_index(p, faulting_addr)) != -1) {
        goto heap_handle;
    }

    load_binary_page(p, faulting_addr);

    /* Go to out, since the remainder of this code is for the heap. */
    goto out;

heap_handle:
    /* 2.4: Check if resident pages are more than heap pages. If yes, evict. */
    if (p->resident_heap_pages == MAXRESHEAP) {
        evict_page_to_disk(p);
    }

    /* 2.4: Heap page was swapped to disk previously. We must load it from disk. */
    if (p->heap_tracker[heap_tracker_index].startblock != -1) {
        retrieve_page_from_disk(p, heap_tracker_index);
    }
    /* 2.3: Map a heap page into the process' address space. (Hint: check growproc) */
    load_heap_page(p, heap_tracker_index);

out:
    /* Flush stale page table entries. This is important to always do. */
    sfence_vma();
    return;
}

uint64 find_faulting_base_addr(void) {
    uint64 faulting_addr = r_stval();
    uint64 base_addr = ((faulting_addr >> VIRTUAL_ADDR_OFFSTET_BITS) << VIRTUAL_ADDR_OFFSTET_BITS);

    return base_addr;
}

int get_heap_tracker_index(struct proc *p, uint64 faulting_page_addr) {
    for(int i=0;i<MAXHEAP;i++) {
        if(p->heap_tracker[i].addr <= faulting_page_addr && faulting_page_addr < (p->heap_tracker[i].addr + PGSIZE))
            return i;
    }

    return -1;
}

void load_binary_page(struct proc *p, uint64 faulting_page_addr) {
    begin_op();
    struct inode *in = namei(p->name);
    ilock(in);
    struct elfhdr elf;
    struct proghdr ph;
    readi(in, 0, (uint64)&elf, 0, sizeof(elf));

    for(int i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)) {
        readi(in, 0, (uint64)&ph, off, sizeof(ph));

        if(ph.type != ELF_PROG_LOAD)
            continue;

        if(faulting_page_addr < ph.vaddr || faulting_page_addr > (ph.vaddr + ph.memsz))
            continue;

        uvmalloc(p->pagetable, ph.vaddr, ph.vaddr + ph.memsz, flags2perm(ph.flags));
        loadseg(p->pagetable, ph.vaddr, in, ph.off, ph.filesz);
        /* If it came here, it is a page from the program binary that we must load. */
        print_load_seg(faulting_page_addr, ph.off, ph.memsz);
    }
    iunlockput(in);
    end_op();
}

void load_heap_page(struct proc *p, int heap_tracker_index) {
    uvmalloc(p->pagetable, p->heap_tracker[heap_tracker_index].addr, p->heap_tracker[heap_tracker_index].addr + PGSIZE, PTE_W);

    /* 2.4: Update the last load time for the loaded heap page in p->heap_tracker. */
    p->heap_tracker[heap_tracker_index].last_load_time = read_current_timestamp();
    p->heap_tracker[heap_tracker_index].loaded = true;
    p->heap_tracker[heap_tracker_index].startblock = -1;
    p->resident_heap_pages++;
}

bool is_cow(struct proc *p, uint64 va) {
    if(!p->cow_enabled)
        return false;

    pte_t *pte;

    if((pte = walk(p->pagetable, va, 0)) == 0)
        return false;
    if((*pte & PTE_V) == 0)
        return false;

    return true;
}