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
    struct vm_reg privi_regs[33];
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
    // Supervisor page table register
    state->privi_regs[20] = (struct vm_reg){.code = 0x180, .mode = SUPERVISOR, .val = 0};
    // Supervisor trap setup registers
    state->privi_regs[21] = (struct vm_reg){.code = 0x100, .mode = SUPERVISOR, .val = 0};
    state->privi_regs[22] = (struct vm_reg){.code = 0x104, .mode = SUPERVISOR, .val = 0};
    state->privi_regs[23] = (struct vm_reg){.code = 0x105, .mode = SUPERVISOR, .val = 0};
    state->privi_regs[24] = (struct vm_reg){.code = 0x106, .mode = SUPERVISOR, .val = 0};
    // User trap handling registers
    state->privi_regs[25] = (struct vm_reg){.code = 0x040, .mode = USER, .val = 0};
    state->privi_regs[26] = (struct vm_reg){.code = 0x041, .mode = USER, .val = 0};
    state->privi_regs[27] = (struct vm_reg){.code = 0x042, .mode = USER, .val = 0};
    state->privi_regs[28] = (struct vm_reg){.code = 0x043, .mode = USER, .val = 0};
    state->privi_regs[29] = (struct vm_reg){.code = 0x044, .mode = USER, .val = 0};
    // User trap setup registers
    state->privi_regs[30] = (struct vm_reg){.code = 0x000, .mode = USER, .val = 0};
    state->privi_regs[31] = (struct vm_reg){.code = 0x004, .mode = USER, .val = 0};
    state->privi_regs[32] = (struct vm_reg){.code = 0x005, .mode = USER, .val = 0};
}