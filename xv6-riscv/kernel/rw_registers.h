#include "types.h"

void write_to_register(uint64 regis_code, uint64 value) {
    switch (regis_code)
    {
    case 0x1:
        w_ra(value);
        break;

    case 0x2:
        w_sp(value);
        break;

    case 0x5:
        w_t0(value);
        break;

    case 0x6:
        w_t1(value);
        break;

    case 0x7:
        w_t2(value);
        break;

    case 0x8:
        w_s0(value);
        break;

    case 0x9:
        w_s1(value);
        break;

    case 0xa:
        w_a0(value);
        break;

    case 0xb:
        w_a1(value);
        break;

    case 0xc:
        w_a2(value);
        break;

    case 0xd:
        w_a3(value);
        break;

    case 0xe:
        w_a4(value);
        break;

    case 0xf:
        w_a5(value);
        break;

    case 0x10:
        w_a6(value);
        break;

    case 0x11:
        w_a7(value);
        break;

    case 0x12:
        w_s2(value);
        break;

    case 0x13:
        w_s3(value);
        break;

    case 0x14:
        w_s4(value);
        break;

    case 0x15:
        w_s5(value);
        break;

    case 0x16:
        w_s6(value);
        break;

    case 0x17:
        w_s7(value);
        break;

    case 0x18:
        w_s8(value);
        break;

    case 0x19:
        w_s9(value);
        break;

    case 0x1a:
        w_s10(value);
        break;

    case 0x1b:
        w_s11(value);
        break;

    case 0x1c:
        w_t3(value);
        break;

    case 0x1d:
        w_t4(value);
        break;

    case 0x1e:
        w_t5(value);
        break;

    case 0x1f:
        w_t6(value);
        break;

    default:
        break;
    }
}