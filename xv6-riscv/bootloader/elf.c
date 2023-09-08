#include "types.h"
#include "param.h"
#include "layout.h"
#include "riscv.h"
#include "defs.h"
#include "buf.h"
#include "elf.h"

#include <stdbool.h>

struct elfhdr* kernel_elfhdr;
struct proghdr* kernel_phdr;

uint64 get_binary_addr(enum kernel ktype)
{
    if(ktype == NORMAL) return RAMDISK;
    else return RECOVERYDISK;
}

struct elfhdr* get_elf_header(enum kernel ktype)
{
    return (struct elfhdr*)get_binary_addr(ktype);
}

uint64 find_kernel_load_addr(enum kernel ktype) {
    /* CSE 536: Get kernel load address from headers */
    uint64 binary_addr = get_binary_addr(ktype);
    kernel_elfhdr = get_elf_header(ktype);
    uint64 offset = kernel_elfhdr -> phoff;
    ushort size = kernel_elfhdr -> phentsize;
    uint64 text_address = binary_addr + offset + size;
    kernel_phdr = (struct proghdr*)text_address;

    return kernel_phdr -> vaddr;
}

uint64 find_kernel_size(enum kernel ktype) {
    /* CSE 536: Get kernel binary size from headers */
    kernel_elfhdr = get_elf_header(ktype);

    return kernel_elfhdr -> shoff + (kernel_elfhdr -> shentsize * kernel_elfhdr -> shnum);
}

uint64 find_kernel_entry_addr(enum kernel ktype) {
    /* CSE 536: Get kernel entry point from headers */
    kernel_elfhdr = get_elf_header(ktype);

    return kernel_elfhdr -> entry;
}