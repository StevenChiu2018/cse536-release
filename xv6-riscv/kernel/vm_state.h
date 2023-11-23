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
    struct vm_reg privi_regs[20];
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
    state->privi_regs[1] = (struct vm_reg){.code = 0x341, .mode = MACHINE, .val = 0};
    state->privi_regs[2] = (struct vm_reg){.code = 0x342, .mode = MACHINE, .val = 0};
    state->privi_regs[3] = (struct vm_reg){.code = 0x343, .mode = MACHINE, .val = 0};
    state->privi_regs[4] = (struct vm_reg){.code = 0x344, .mode = MACHINE, .val = 0};
    state->privi_regs[5] = (struct vm_reg){.code = 0x34a, .mode = MACHINE, .val = 0};
    state->privi_regs[6] = (struct vm_reg){.code = 0x34b, .mode = MACHINE, .val = 0};
    // Machine trap setup registers
    state->privi_regs[7] = (struct vm_reg){.code = 0x300, .mode = MACHINE, .val = 0};
    state->privi_regs[8] = (struct vm_reg){.code = 0x301, .mode = MACHINE, .val = 0};
    state->privi_regs[9] = (struct vm_reg){.code = 0x302, .mode = MACHINE, .val = 0};
    state->privi_regs[10] = (struct vm_reg){.code = 0x303, .mode = MACHINE, .val = 0};
    state->privi_regs[11] = (struct vm_reg){.code = 0x304, .mode = MACHINE, .val = 0};
    state->privi_regs[12] = (struct vm_reg){.code = 0x305, .mode = MACHINE, .val = 0};
    state->privi_regs[13] = (struct vm_reg){.code = 0x306, .mode = MACHINE, .val = 0};
    state->privi_regs[14] = (struct vm_reg){.code = 0x310, .mode = MACHINE, .val = 0};
    // Machine information registers
    state->privi_regs[15] = (struct vm_reg){.code = 0xf11, .mode = MACHINE, .val = 0};
    state->privi_regs[16] = (struct vm_reg){.code = 0xf12, .mode = MACHINE, .val = 0};
    state->privi_regs[17] = (struct vm_reg){.code = 0xf13, .mode = MACHINE, .val = 0};
    state->privi_regs[18] = (struct vm_reg){.code = 0xf14, .mode = MACHINE, .val = 0};
    state->privi_regs[19] = (struct vm_reg){.code = 0xf15, .mode = MACHINE, .val = 0};
    // User trap setup
    // User trap handling
    // Supervisor trap setup
    // User trap handling
    // Supervisor page table register
}