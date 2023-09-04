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

uint64 find_kernel_load_addr(enum kernel ktype) {
    /* CSE 536: Get kernel load address from headers */
    kernel_elfhdr = (struct elfhdr*)RAMDISK;
    uint64 offset = kernel_elfhdr -> phoff;
    ushort size = kernel_elfhdr -> phentsize;
    uint64 text_address = RAMDISK + offset + size;
    kernel_phdr = (struct proghdr*)text_address;

    return kernel_phdr -> vaddr;
}

uint64 find_kernel_size(enum kernel ktype) {
    /* CSE 536: Get kernel binary size from headers */
    kernel_elfhdr = (struct elfhdr*)RAMDISK;

    return kernel_elfhdr -> shoff + (kernel_elfhdr -> shentsize * kernel_elfhdr -> shnum);
}

uint64 find_kernel_entry_addr(enum kernel ktype) {
    /* CSE 536: Get kernel entry point from headers */
    kernel_elfhdr = (struct elfhdr*)RAMDISK;

    return kernel_elfhdr -> entry;
}