# _entry should be the first function invoked when
# the QEMU CPU starts executing. It's location should
# according to the memory layout provided in param.h

#include "param.h"

.section .text
.global _entry
_entry:
        # task: setup a per-CPU stack for C and jump to start() function
        la sp, bl_stack
        la a0, NCPU
        la a1, STSIZE
        mul a2, a0, a1
        addi a2, a2, 1
        add sp, sp, a2

        call start
spin:
        j spin
