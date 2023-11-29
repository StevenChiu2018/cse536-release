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