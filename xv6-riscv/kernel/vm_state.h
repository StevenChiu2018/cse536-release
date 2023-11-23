#include "types.h"

enum Execution_mode {MACHINE, SUPERVISOR, USER};

// Struct to keep VM registers (Sample; feel free to change.)
struct vm_reg {
    int code;
    enum Execution_mode mode;
    uint64 val;
};

// Keep the virtual state of the VM's privileged registers
struct vm_state {
    struct vm_reg privi_regs[7];
};

void setup_regs(struct vm_state*);

struct vm_state generate_vm_state(void) {
    struct vm_state state;

    setup_regs(&state);

    return state;
}

void setup_regs(struct vm_state *state) {
    // Machine trap handling registers
    state->privi_regs[0] = (struct vm_reg){.code = 0x340, .mode = MACHINE, .val = 0};
    // User trap setup
    // User trap handling
    // Supervisor trap setup
    // User trap handling
    // Supervisor page table register
    // Machine information registers
    // Machine trap setup registers
}