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

lane_t U64BIG(lane_t x)
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
void to_bit_interleaving(lane_t *out, lane_t in)
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
void from_bit_interleaving(lane_t *out, lane_t in)
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

#define ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

/* ---------------------------------------------------------------- */

forceinline void ROUND(state_t *s, uint32_t ce, uint32_t co)
{
    lane_t t0, t1, t2, t3, t4;
    s->w[2][0] ^= ce;
    s->w[2][1] ^= co;
    /* non-linear layer */
    s->w[0][0] ^= s->w[4][0];
    s->w[0][1] ^= s->w[4][1];
    s->w[4][0] ^= s->w[3][0];
    s->w[4][1] ^= s->w[3][1];
    s->w[2][0] ^= s->w[1][0];
    s->w[2][1] ^= s->w[1][1];
    t0.w[0] = s->w[0][0];
    t0.w[1] = s->w[0][1];
    t4.w[0] = s->w[4][0];
    t4.w[1] = s->w[4][1];
    t3.w[0] = s->w[3][0];
    t3.w[1] = s->w[3][1];
    t1.w[0] = s->w[1][0];
    t1.w[1] = s->w[1][1];
    t2.w[0] = s->w[2][0];
    t2.w[1] = s->w[2][1];
    s->w[0][0] = t0.w[0] ^ (~t1.w[0] & t2.w[0]);
    s->w[0][1] = t0.w[1] ^ (~t1.w[1] & t2.w[1]);
    s->w[2][0] = t2.w[0] ^ (~t3.w[0] & t4.w[0]);
    s->w[2][1] = t2.w[1] ^ (~t3.w[1] & t4.w[1]);
    s->w[4][0] = t4.w[0] ^ (~t0.w[0] & t1.w[0]);
    s->w[4][1] = t4.w[1] ^ (~t0.w[1] & t1.w[1]);
    s->w[1][0] = t1.w[0] ^ (~t2.w[0] & t3.w[0]);
    s->w[1][1] = t1.w[1] ^ (~t2.w[1] & t3.w[1]);
    s->w[3][0] = t3.w[0] ^ (~t4.w[0] & t0.w[0]);
    s->w[3][1] = t3.w[1] ^ (~t4.w[1] & t0.w[1]);
    s->w[1][0] ^= s->w[0][0];
    s->w[1][1] ^= s->w[0][1];
    s->w[3][0] ^= s->w[2][0];
    s->w[3][1] ^= s->w[2][1];
    s->w[0][0] ^= s->w[4][0];
    s->w[0][1] ^= s->w[4][1];
    /* linear layer */
    t0.w[0] = (s->w[0][0]) ^ ROTR32(s->w[0][1], 4);
    t0.w[1] = (s->w[0][1]) ^ ROTR32(s->w[0][0], 5);
    t1.w[0] = (s->w[1][0]) ^ ROTR32(s->w[1][0], 11);
    t1.w[1] = (s->w[1][1]) ^ ROTR32(s->w[1][1], 11);
    t2.w[0] = (s->w[2][0]) ^ ROTR32(s->w[2][1], 2);
    t2.w[1] = (s->w[2][1]) ^ ROTR32(s->w[2][0], 3);
    t3.w[0] = (s->w[3][0]) ^ ROTR32(s->w[3][1], 3);
    t3.w[1] = (s->w[3][1]) ^ ROTR32(s->w[3][0], 4);
    t4.w[0] = (s->w[4][0]) ^ ROTR32(s->w[4][0], 17);
    t4.w[1] = (s->w[4][1]) ^ ROTR32(s->w[4][1], 17);
    s->w[0][0] ^= ROTR32(t0.w[1], 9);
    s->w[0][1] ^= ROTR32(t0.w[0], 10);
    s->w[1][0] ^= ROTR32(t1.w[1], 19);
    s->w[1][1] ^= ROTR32(t1.w[0], 20);
    s->w[2][0] ^= t2.w[1];
    s->w[2][1] ^= ROTR32(t2.w[0], 1);
    s->w[3][0] ^= ROTR32(t3.w[0], 5);
    s->w[3][1] ^= ROTR32(t3.w[1], 5);
    s->w[4][0] ^= ROTR32(t4.w[1], 3);
    s->w[4][1] ^= ROTR32(t4.w[0], 4);
    s->w[2][0] = ~(s->w[2][0]);
    s->w[2][1] = ~(s->w[2][1]);
}

/* ---------------------------------------------------------------- */

void P12ROUNDS(state_t *s)
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

void P6ROUNDS(state_t *s)
{
    ROUND(s, 0x6, 0x9);
    ROUND(s, 0x3, 0x9);
    ROUND(s, 0xc, 0x6);
    ROUND(s, 0x9, 0x6);
    ROUND(s, 0xc, 0x3);
    ROUND(s, 0x9, 0x3);
}

/* ---------------------------------------------------------------- */

void P1ROUNDS(state_t *s)
{
    ROUND(s, 0x9, 0x3);
}

/* ---------------------------------------------------------------- */

#endif // ASCONP_H

