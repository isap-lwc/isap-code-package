#ifndef ASCONP_H
#define ASCONP_H

#include <inttypes.h>
#include "forceinline.h"

typedef union
{
    uint64_t x;
    uint32_t w[2];
    uint8_t b[8];
} lane_t;

typedef union
{
    lane_t l[5];
    uint64_t x[5];
    uint32_t w[5][2];
    uint8_t b[5][8];
} state_t;

#define RC(i) ((uint64_t)constants[i + 1] << 32 | constants[i])

/* ---------------------------------------------------------------- */

forceinline lane_t U64BIG(lane_t x)
{
    lane_t t0;
    t0.x = ((((x.x) & 0x00000000000000FFULL) << 56) | (((x.x) & 0x000000000000FF00ULL) << 40) |
            (((x.x) & 0x0000000000FF0000ULL) << 24) | (((x.x) & 0x00000000FF000000ULL) << 8) |
            (((x.x) & 0x000000FF00000000ULL) >> 8) | (((x.x) & 0x0000FF0000000000ULL) >> 24) |
            (((x.x) & 0x00FF000000000000ULL) >> 40) | (((x.x) & 0xFF00000000000000ULL) >> 56));
    return t0;
}

/* ---------------------------------------------------------------- */

// Credit to Henry S. Warren, Hacker's Delight, Addison-Wesley, 2002
forceinline void to_bit_interleaving(lane_t *out, lane_t in)
{
    uint32_t lo = in.w[0];
    uint32_t hi = in.w[1];
    uint32_t r0, r1;
    r0 = (lo ^ (lo >> 1)) & 0x22222222, lo ^= r0 ^ (r0 << 1);
    r0 = (lo ^ (lo >> 2)) & 0x0C0C0C0C, lo ^= r0 ^ (r0 << 2);
    r0 = (lo ^ (lo >> 4)) & 0x00F000F0, lo ^= r0 ^ (r0 << 4);
    r0 = (lo ^ (lo >> 8)) & 0x0000FF00, lo ^= r0 ^ (r0 << 8);
    r1 = (hi ^ (hi >> 1)) & 0x22222222, hi ^= r1 ^ (r1 << 1);
    r1 = (hi ^ (hi >> 2)) & 0x0C0C0C0C, hi ^= r1 ^ (r1 << 2);
    r1 = (hi ^ (hi >> 4)) & 0x00F000F0, hi ^= r1 ^ (r1 << 4);
    r1 = (hi ^ (hi >> 8)) & 0x0000FF00, hi ^= r1 ^ (r1 << 8);
    out->w[0] = (lo & 0x0000FFFF) | (hi << 16);
    out->w[1] = (lo >> 16) | (hi & 0xFFFF0000);
}

/* ---------------------------------------------------------------- */

// Credit to Henry S. Warren, Hacker's Delight, Addison-Wesley, 2002
forceinline void from_bit_interleaving(lane_t *out, lane_t in)
{
    uint32_t lo = ((in).w[0] & 0x0000FFFF) | ((in).w[1] << 16);
    uint32_t hi = ((in).w[0] >> 16) | ((in).w[1] & 0xFFFF0000);
    uint32_t r0, r1;
    r0 = (lo ^ (lo >> 8)) & 0x0000FF00, lo ^= r0 ^ (r0 << 8);
    r0 = (lo ^ (lo >> 4)) & 0x00F000F0, lo ^= r0 ^ (r0 << 4);
    r0 = (lo ^ (lo >> 2)) & 0x0C0C0C0C, lo ^= r0 ^ (r0 << 2);
    r0 = (lo ^ (lo >> 1)) & 0x22222222, lo ^= r0 ^ (r0 << 1);
    r1 = (hi ^ (hi >> 8)) & 0x0000FF00, hi ^= r1 ^ (r1 << 8);
    r1 = (hi ^ (hi >> 4)) & 0x00F000F0, hi ^= r1 ^ (r1 << 4);
    r1 = (hi ^ (hi >> 2)) & 0x0C0C0C0C, hi ^= r1 ^ (r1 << 2);
    r1 = (hi ^ (hi >> 1)) & 0x22222222, hi ^= r1 ^ (r1 << 1);
    out->x = (uint64_t)hi << 32 | lo;
}

/* ---------------------------------------------------------------- */

forceinline void ROUND(state_t *s, uint32_t ce, uint32_t co)
{
    uint32_t reg0, reg1, reg2, reg3;
    s->w[2][0] ^= ce;
    s->w[2][1] ^= co;
    __asm__ __volatile__(
        "eor %[x0_e], %[x0_e], %[x4_e]\n\t"
        "eor %[x0_o], %[x0_o], %[x4_o]\n\t"
        "eor %[x4_e], %[x4_e], %[x3_e]\n\t"
        "eor %[x4_o], %[x4_o], %[x3_o]\n\t"
        "eor %[x2_e], %[x2_e], %[x1_e]\n\t"
        "eor %[x2_o], %[x2_o], %[x1_o]\n\t"
        "bic %[reg0], %[x0_e], %[x4_e]\n\t"
        "bic %[reg1], %[x4_e], %[x3_e]\n\t"
        "bic %[reg2], %[x2_e], %[x1_e]\n\t"
        "bic %[reg3], %[x1_e], %[x0_e]\n\t"
        "eor %[x2_e], %[x2_e], %[reg1]\n\t"
        "eor %[x0_e], %[x0_e], %[reg2]\n\t"
        "eor %[x4_e], %[x4_e], %[reg3]\n\t"
        "bic %[reg3], %[x3_e], %[x2_e]\n\t"
        "eor %[x3_e], %[x3_e], %[reg0]\n\t"
        "bic %[reg2], %[x0_o], %[x4_o]\n\t"
        "bic %[reg0], %[x2_o], %[x1_o]\n\t"
        "bic %[reg1], %[x4_o], %[x3_o]\n\t"
        "eor %[x1_e], %[x1_e], %[reg3]\n\t"
        "eor %[x0_o], %[x0_o], %[reg0]\n\t"
        "eor %[x2_o], %[x2_o], %[reg1]\n\t"
        "bic %[reg3], %[x1_o], %[x0_o]\n\t"
        "bic %[reg0], %[x3_o], %[x2_o]\n\t"
        "eor %[x3_o], %[x3_o], %[reg2]\n\t"
        "eor %[x3_o], %[x3_o], %[x2_o]\n\t"
        "eor %[x4_o], %[x4_o], %[reg3]\n\t"
        "eor %[x1_o], %[x1_o], %[reg0]\n\t"
        "eor %[x3_e], %[x3_e], %[x2_e]\n\t"
        "eor %[x1_e], %[x1_e], %[x0_e]\n\t"
        "eor %[x1_o], %[x1_o], %[x0_o]\n\t"
        "eor %[x0_e], %[x0_e], %[x4_e]\n\t"
        "eor %[x0_o], %[x0_o], %[x4_o]\n\t"
        "mvn %[x2_e], %[x2_e]\n\t"
        "mvn %[x2_o], %[x2_o]\n\t"
        "eor %[reg0], %[x0_e], %[x0_o], ror #4\n\t"
        "eor %[reg1], %[x0_o], %[x0_e], ror #5\n\t"
        "eor %[reg2], %[x1_e], %[x1_e], ror #11\n\t"
        "eor %[reg3], %[x1_o], %[x1_o], ror #11\n\t"
        "eor %[x0_e], %[x0_e], %[reg1], ror #9\n\t"
        "eor %[x0_o], %[x0_o], %[reg0], ror #10\n\t"
        "eor %[x1_e], %[x1_e], %[reg3], ror #19\n\t"
        "eor %[x1_o], %[x1_o], %[reg2], ror #20\n\t"
        "eor %[reg0], %[x2_e], %[x2_o], ror #2\n\t"
        "eor %[reg1], %[x2_o], %[x2_e], ror #3\n\t"
        "eor %[reg2], %[x3_e], %[x3_o], ror #3\n\t"
        "eor %[reg3], %[x3_o], %[x3_e], ror #4\n\t"
        "eor %[x2_e], %[x2_e], %[reg1]\n\t"
        "eor %[x2_o], %[x2_o], %[reg0], ror #1\n\t"
        "eor %[x3_e], %[x3_e], %[reg2], ror #5\n\t"
        "eor %[x3_o], %[x3_o], %[reg3], ror #5\n\t"
        "eor %[reg0], %[x4_e], %[x4_e], ror #17\n\t"
        "eor %[reg1], %[x4_o], %[x4_o], ror #17\n\t"
        "eor %[x4_e], %[x4_e], %[reg1], ror #3\n\t"
        "eor %[x4_o], %[x4_o], %[reg0], ror #4\n\t"
        : [x0_e] "+r"(s->w[0][0]), [x1_e] "+r"(s->w[1][0]),
          [x2_e] "+r"(s->w[2][0]), [x3_e] "+r"(s->w[3][0]),
          [x4_e] "+r"(s->w[4][0]), [x0_o] "+r"(s->w[0][1]),
          [x1_o] "+r"(s->w[1][1]), [x2_o] "+r"(s->w[2][1]),
          [x3_o] "+r"(s->w[3][1]), [x4_o] "+r"(s->w[4][1]),
          [reg0] "=r"(reg0), [reg1] "=r"(reg1),
          [reg2] "=r"(reg2), [reg3] "=r"(reg3)::);
}

/* ---------------------------------------------------------------- */

forceinline void P12ROUNDS(state_t *s)
{
    ROUND(s, 0xc, 0xc);
    ROUND(s, 0x9, 0xc);
    ROUND(s, 0xc, 0x9);
    ROUND(s, 0x9, 0x9);
    ROUND(s, 0x6, 0xc);
    ROUND(s, 0x3, 0xc);
    ROUND(s, 0x6, 0x9);
    ROUND(s, 0x3, 0x9);
    ROUND(s, 0xc, 0x6);
    ROUND(s, 0x9, 0x6);
    ROUND(s, 0xc, 0x3);
    ROUND(s, 0x9, 0x3);
}

/* ---------------------------------------------------------------- */

forceinline void P6ROUNDS(state_t *s)
{
    ROUND(s, 0x6, 0x9);
    ROUND(s, 0x3, 0x9);
    ROUND(s, 0xc, 0x6);
    ROUND(s, 0x9, 0x6);
    ROUND(s, 0xc, 0x3);
    ROUND(s, 0x9, 0x3);
}

/* ---------------------------------------------------------------- */

forceinline void P1ROUNDS(state_t *s)
{
    ROUND(s, 0x9, 0x3);
}

/* ---------------------------------------------------------------- */

#endif // ASCONP_H

