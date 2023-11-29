#include "types.h"

void write_to_register(uint64 regis_code, uint64 value) {
    struct proc *p = myproc();

    if(regis_code == 0x1) {
        p->trapframe->ra = value;
    } else if(regis_code == 1) {
        p->trapframe->ra = value;
    } else if(regis_code == 2) {
        p->trapframe->sp = value;
    } else if(regis_code == 3) {
        p->trapframe->gp = value;
    } else if(regis_code == 4) {
        p->trapframe->tp = value;
    } else if(regis_code == 5) {
        p->trapframe->t0 = value;
    } else if(regis_code == 6) {
        p->trapframe->t1 = value;
    } else if(regis_code == 7) {
        p->trapframe->t2 = value;
    } else if(regis_code == 8) {
        p->trapframe->s0 = value;
    } else if(regis_code == 9) {
        p->trapframe->s1 = value;
    } else if(regis_code == 10) {
        p->trapframe->a0 = value;
    } else if(regis_code == 11) {
        p->trapframe->a1 = value;
    } else if(regis_code == 12) {
        p->trapframe->a2 = value;
    } else if(regis_code == 13) {
        p->trapframe->a3 = value;
    } else if(regis_code == 14) {
        p->trapframe->a4 = value;
    } else if(regis_code == 15) {
        p->trapframe->a5 = value;
    } else if(regis_code == 16) {
        p->trapframe->a6 = value;
    } else if(regis_code == 17) {
        p->trapframe->a7 = value;
    } else if(regis_code == 18) {
        p->trapframe->s2 = value;
    } else if(regis_code == 19) {
        p->trapframe->s3 = value;
    } else if(regis_code == 20) {
        p->trapframe->s4 = value;
    } else if(regis_code == 21) {
        p->trapframe->s5 = value;
    } else if(regis_code == 22) {
        p->trapframe->s6 = value;
    } else if(regis_code == 23) {
        p->trapframe->s7 = value;
    } else if(regis_code == 24) {
        p->trapframe->s8 = value;
    } else if(regis_code == 25) {
        p->trapframe->s9 = value;
    } else if(regis_code == 26) {
        p->trapframe->s10 = value;
    } else if(regis_code == 27) {
        p->trapframe->s11 = value;
    } else if(regis_code == 28) {
        p->trapframe->t3 = value;
    } else if(regis_code == 29) {
        p->trapframe->t4 = value;
    } else if(regis_code == 30) {
        p->trapframe->t5 = value;
    } else if(regis_code == 31) {
        p->trapframe->t6 = value;
    }
}

uint64 read_from_register(uint64 regis_code) {
    struct proc *p = myproc();

    if(regis_code == 0x1) {
        return p->trapframe->ra;
    } else if(regis_code == 1) {
        return p->trapframe->ra;
    } else if(regis_code == 2) {
        return p->trapframe->sp;
    } else if(regis_code == 3) {
        return p->trapframe->gp;
    } else if(regis_code == 4) {
        return p->trapframe->tp;
    } else if(regis_code == 5) {
        return p->trapframe->t0;
    } else if(regis_code == 6) {
        return p->trapframe->t1;
    } else if(regis_code == 7) {
        return p->trapframe->t2;
    } else if(regis_code == 8) {
        return p->trapframe->s0;
    } else if(regis_code == 9) {
        return p->trapframe->s1;
    } else if(regis_code == 10) {
        return p->trapframe->a0;
    } else if(regis_code == 11) {
        return p->trapframe->a1;
    } else if(regis_code == 12) {
        return p->trapframe->a2;
    } else if(regis_code == 13) {
        return p->trapframe->a3;
    } else if(regis_code == 14) {
        return p->trapframe->a4;
    } else if(regis_code == 15) {
        return p->trapframe->a5;
    } else if(regis_code == 16) {
        return p->trapframe->a6;
    } else if(regis_code == 17) {
        return p->trapframe->a7;
    } else if(regis_code == 18) {
        return p->trapframe->s2;
    } else if(regis_code == 19) {
        return p->trapframe->s3;
    } else if(regis_code == 20) {
        return p->trapframe->s4;
    } else if(regis_code == 21) {
        return p->trapframe->s5;
    } else if(regis_code == 22) {
        return p->trapframe->s6;
    } else if(regis_code == 23) {
        return p->trapframe->s7;
    } else if(regis_code == 24) {
        return p->trapframe->s8;
    } else if(regis_code == 25) {
        return p->trapframe->s9;
    } else if(regis_code == 26) {
        return p->trapframe->s10;
    } else if(regis_code == 27) {
        return p->trapframe->s11;
    } else if(regis_code == 28) {
        return p->trapframe->t3;
    } else if(regis_code == 29) {
        return p->trapframe->t4;
    } else if(regis_code == 30) {
        return p->trapframe->t5;
    } else if(regis_code == 31) {
        return p->trapframe->te;
    }
}