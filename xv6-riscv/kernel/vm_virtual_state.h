#include "types.h"

// Struct to keep VM registers (Sample; feel free to change.)
struct vm_reg {
    int     code;
    int     mode;
    uint64  val;
};

// Keep the virtual state of the VM's privileged registers
struct vm_virtual_state {
    struct vm_reg privi_regs[7];
};

void do_setup_regs(void);

void setup(void) {
    do_setup_regs();
}

void do_setup_regs(void) {
    // Machine trap handling registers
    // User trap setup
    // User trap handling
    // Supervisor trap setup
    // User trap handling
    // Supervisor page table register
    // Machine information registers
    // Machine trap setup registers
}