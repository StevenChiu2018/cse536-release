#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "vm_state.h"
#include "rw_registers.h"

enum execution_mode cur_exe_mode;
struct vm_state state;

struct instruct {
    uint64 addr;
    uint32 op;
    uint32 rd;
    uint32 funct3;
    uint32 rs1;
    uint32 uimm;
};

struct instruct get_trap_instruction(void);
uint32 emulate_trap_instruction(struct instruct*);

// In your ECALL, add the following for prints
// struct proc* p = myproc();
// printf("(EC at %p)\n", p->trapframe->epc);
uint32 trap_and_emulate_ecall(void) {
    struct instruct trap_instruct = get_trap_instruction();

    printf("(EC at %p)\n", trap_instruct.addr);

    return emulate_trap_instruction(&trap_instruct);
}

uint32 trap_and_emulate(void) {
    /* Comes here when a VM tries to execute a supervisor instruction. */

    /* Retrieve all required values from the instruction */
    struct instruct trap_instruct = get_trap_instruction();

    /* Print the statement */
    printf("(PI at %p) op = %x, rd = %x, funct3 = %x, rs1 = %x, uimm = %x\n",
                trap_instruct.addr, trap_instruct.op, trap_instruct.rd, trap_instruct.funct3, trap_instruct.rs1, trap_instruct.uimm);

    return emulate_trap_instruction(&trap_instruct);
}

uint32 extract_instruction(void);
struct instruct decode_instruction(uint32 instruction);

struct instruct get_trap_instruction(void) {
    uint64 coded_instruction = extract_instruction();
    return decode_instruction(coded_instruction);
}

uint32 extract_instruction(void) {
    struct proc *p = myproc();
    uint64 instruction_va = r_sepc();
    char *buf = kalloc();
    copyin(p->pagetable, buf, instruction_va, PGSIZE);

    return *((uint32*) buf);
}

struct instruct decode_instruction(uint32 coded_instruction) {
    struct instruct instruction;

    instruction.addr = r_sepc();
    instruction.op = coded_instruction % 128;
    coded_instruction >>= 7;
    instruction.rd = coded_instruction % 32;
    coded_instruction >>= 5;
    instruction.funct3 = coded_instruction % 8;
    coded_instruction >>= 3;
    instruction.rs1 = coded_instruction % 32;
    coded_instruction >>= 5;
    instruction.uimm = coded_instruction;

    return instruction;
}

uint32 do_emulate_csrr(struct instruct*);
uint32 do_emulate_csrw(struct instruct*);
uint32 do_emulate_mret(struct instruct*);
uint32 do_emulate_sret(struct instruct*);
uint32 do_emulate_ecall(struct instruct*);
uint32 is_valid_to_read(uint32);
uint32 is_valid_to_write(uint32);

uint32 emulate_trap_instruction(struct instruct* trap_instruction) {
    if(trap_instruction->funct3 == 0x1) {
        return do_emulate_csrw(trap_instruction);
    } else if(trap_instruction->funct3 == 0x2) {
        return do_emulate_csrr(trap_instruction);
    } else if(trap_instruction->uimm == 0x302) {
        return do_emulate_mret(trap_instruction);
    } else if(trap_instruction->uimm == 0x102) {
        return do_emulate_sret(trap_instruction);
    } else if(trap_instruction->uimm == 0x0) {
        return do_emulate_ecall(trap_instruction);
    } else {
        panic("un-emulated instruction happened\n");
        return 0;
    }
}

uint32 do_emulate_csrr(struct instruct *trap_instruction) {
    struct vm_reg *reg = get_privi_reg(&state, trap_instruction->uimm);

    if(is_valid_to_read(reg->auth)) {
        write_to_register(trap_instruction->rd, reg->val);
    } else {
        return -1;
    }

    return 1;
}

uint32 do_emulate_csrw(struct instruct *trap_instrucion) {
    struct vm_reg *reg = get_privi_reg(&state, trap_instrucion->uimm);

    if(is_valid_to_write(reg->auth)) {
        reg->val = read_from_register(trap_instrucion->rs1);

        return reg->code != 0xf11 || reg->val != 0x0 ? 1 : 0;
    } else {
        return -1;
    }
}

uint32 do_emulate_mret(struct instruct *trap_instruction) {
    struct proc *p = myproc();
    struct vm_reg *reg = get_privi_reg(&state, 0x300);
    uint64 mpp = reg->val & MSTATUS_MPP_MASK;

    if((mpp & MSTATUS_MPP_S) == MSTATUS_MPP_S) {
        cur_exe_mode = SUPERVISOR;
        struct vm_reg *mepc = get_privi_reg(&state, 0x341);
        p->trapframe->epc = mepc->val - 4;
    } else {
        return 0;
    }

    return 1;
}

uint32 do_emulate_sret(struct instruct *trap_instruction) {
    struct proc *p = myproc();
    struct vm_reg *reg = get_privi_reg(&state, 0x100);
    uint64 spp = reg->val & SSTATUS_SPP;

    if(spp == 0) {
        cur_exe_mode = USER;
        struct vm_reg *sepc = get_privi_reg(&state, 0x141);
        p->trapframe->epc = sepc->val - 4;
    } else {
        return 0;
    }

    return 1;
}

uint32 do_emulate_ecall(struct instruct *trap_instruction) {
    struct proc *p = myproc();
    cur_exe_mode = SUPERVISOR;
    p->trapframe->epc = p->trapframe->kernel_trap - 4;

    return 1;
}

uint32 is_valid_to_read(uint32 regis_auth) {
    return cur_exe_mode >= (regis_auth >> 4);
}

uint32 is_valid_to_write(uint32 regis_auth) {
    return cur_exe_mode >= (regis_auth & 15);
}

void trap_and_emulate_init(void) {
    /* Create and initialize all state for the VM */
    state = generate_vm_state();
    cur_exe_mode = MACHINE;
}