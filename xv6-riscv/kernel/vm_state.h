#include "types.h"

#define PRIVI_REGS_AMOUNT 33

enum execution_mode {USER, SUPERVISOR, MACHINE};

// Struct to keep VM registers (Sample; feel free to change.)
struct vm_reg {
    int code;
    uint64 val;
    uint32 auth; // the leftmost digit present the auth of read, and the rightmost digit presetn the auth of write
};

// Keep the virtual state of the VM's privileged registers
struct vm_state {
    struct vm_reg privi_regs[PRIVI_REGS_AMOUNT];
};

void setup_privi_regs(struct vm_state*);

struct vm_state generate_vm_state(void) {
    struct vm_state state;

    setup_privi_regs(&state);

    return state;
}

void setup_privi_regs(struct vm_state *state) {
    // Machine trap handling registers
    state->privi_regs[0] = (struct vm_reg){.code = 0x340, .auth = 0x22, .val = 0};
    state->privi_regs[1] = (struct vm_reg){.code = 0x341, .auth = 0x22, .val = 0};
    state->privi_regs[2] = (struct vm_reg){.code = 0x342, .auth = 0x22, .val = 0};
    state->privi_regs[3] = (struct vm_reg){.code = 0x343, .auth = 0x22, .val = 0};
    state->privi_regs[4] = (struct vm_reg){.code = 0x344, .auth = 0x22, .val = 0};
    state->privi_regs[5] = (struct vm_reg){.code = 0x34a, .auth = 0x22, .val = 0};
    state->privi_regs[6] = (struct vm_reg){.code = 0x34b, .auth = 0x22, .val = 0};
    // Machine trap setup registers
    state->privi_regs[7] = (struct vm_reg){.code = 0x300, .auth = 0x22, .val = 0x1800};
    state->privi_regs[8] = (struct vm_reg){.code = 0xf10, .auth = 0x22, .val = 0};
    state->privi_regs[9] = (struct vm_reg){.code = 0x302, .auth = 0x22, .val = 0};
    state->privi_regs[10] = (struct vm_reg){.code = 0x303, .auth = 0x22, .val = 0};
    state->privi_regs[11] = (struct vm_reg){.code = 0x304, .auth = 0x22, .val = 0};
    state->privi_regs[12] = (struct vm_reg){.code = 0x305, .auth = 0x22, .val = 0};
    state->privi_regs[13] = (struct vm_reg){.code = 0x306, .auth = 0x22, .val = 0};
    state->privi_regs[14] = (struct vm_reg){.code = 0x310, .auth = 0x22, .val = 0};
    // Machine information registers
    state->privi_regs[15] = (struct vm_reg){.code = 0xf11, .auth = 0x02, .val = 0x637365353336};
    state->privi_regs[16] = (struct vm_reg){.code = 0xf12, .auth = 0x22, .val = 0};
    state->privi_regs[17] = (struct vm_reg){.code = 0xf13, .auth = 0x22, .val = 0};
    state->privi_regs[18] = (struct vm_reg){.code = 0xf14, .auth = 0x22, .val = 0};
    state->privi_regs[19] = (struct vm_reg){.code = 0xf15, .auth = 0x22, .val = 0};
    // Supervisor page table register
    state->privi_regs[20] = (struct vm_reg){.code = 0x180, .auth = 0x11, .val = 0};
    // Supervisor trap setup registers
    state->privi_regs[21] = (struct vm_reg){.code = 0x100, .auth = 0x11, .val = 0};
    state->privi_regs[22] = (struct vm_reg){.code = 0x104, .auth = 0x11, .val = 0};
    state->privi_regs[23] = (struct vm_reg){.code = 0x105, .auth = 0x11, .val = 0};
    state->privi_regs[24] = (struct vm_reg){.code = 0x106, .auth = 0x11, .val = 0};
    // User trap handling registers
    state->privi_regs[25] = (struct vm_reg){.code = 0x040, .auth = 0x00, .val = 0};
    state->privi_regs[26] = (struct vm_reg){.code = 0x041, .auth = 0x00, .val = 0};
    state->privi_regs[27] = (struct vm_reg){.code = 0x042, .auth = 0x00, .val = 0};
    state->privi_regs[28] = (struct vm_reg){.code = 0x043, .auth = 0x00, .val = 0};
    state->privi_regs[29] = (struct vm_reg){.code = 0x044, .auth = 0x00, .val = 0};
    // User trap setup registers
    state->privi_regs[30] = (struct vm_reg){.code = 0x000, .auth = 0x00, .val = 0};
    state->privi_regs[31] = (struct vm_reg){.code = 0x004, .auth = 0x00, .val = 0};
    state->privi_regs[32] = (struct vm_reg){.code = 0x005, .auth = 0x00, .val = 0};
}

struct vm_reg* get_privi_reg(struct vm_state* state, int code) {
    for(int i = 0;i < PRIVI_REGS_AMOUNT;i++) {
        if(state->privi_regs[i].code == code) {
            return &state->privi_regs[i];
        }
    }

    return NULL;
}