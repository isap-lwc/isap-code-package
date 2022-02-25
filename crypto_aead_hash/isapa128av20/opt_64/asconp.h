#ifndef ASCONP_H_
#define ASCONP_H_

#include "forceinline.h"

typedef unsigned char u8;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef union
{
    u64 x[5];
    u32 w[5][2];
    u8 b[5][8];
} state_t;

const int R[5][2] = {
    {19, 28}, {39, 61}, {1, 6}, {10, 17}, {7, 41}};

#define ROTR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

/* ---------------------------------------------------------------- */

forceinline u64 U64BIG(u64 x)
{
    return ((((x)&0x00000000000000FFULL) << 56) | (((x)&0x000000000000FF00ULL) << 40) |
            (((x)&0x0000000000FF0000ULL) << 24) | (((x)&0x00000000FF000000ULL) << 8) |
            (((x)&0x000000FF00000000ULL) >> 8) | (((x)&0x0000FF0000000000ULL) >> 24) |
            (((x)&0x00FF000000000000ULL) >> 40) | (((x)&0xFF00000000000000ULL) >> 56));
}

/* ---------------------------------------------------------------- */

forceinline void ROUND(u64 C, state_t *s)
{
    u64 t0, t1, t2, t3, t4;
    s->x[2] ^= C;
    s->x[0] ^= s->x[4];
    s->x[4] ^= s->x[3];
    s->x[2] ^= s->x[1];
    t0 = s->x[0];
    t4 = s->x[4];
    t3 = s->x[3];
    t1 = s->x[1];
    t2 = s->x[2];
    s->x[0] = t0 ^ ((~t1) & t2);
    s->x[2] = t2 ^ ((~t3) & t4);
    s->x[4] = t4 ^ ((~t0) & t1);
    s->x[1] = t1 ^ ((~t2) & t3);
    s->x[3] = t3 ^ ((~t4) & t0);
    s->x[1] ^= s->x[0];
    t1 = s->x[1];
    s->x[1] = ROTR(s->x[1], R[1][0]);
    s->x[3] ^= s->x[2];
    t2 = s->x[2];
    s->x[2] = ROTR(s->x[2], R[2][0]);
    t4 = s->x[4];
    t2 ^= s->x[2];
    s->x[2] = ROTR(s->x[2], R[2][1] - R[2][0]);
    t3 = s->x[3];
    t1 ^= s->x[1];
    s->x[3] = ROTR(s->x[3], R[3][0]);
    s->x[0] ^= s->x[4];
    s->x[4] = ROTR(s->x[4], R[4][0]);
    t3 ^= s->x[3];
    s->x[2] ^= t2;
    s->x[1] = ROTR(s->x[1], R[1][1] - R[1][0]);
    t0 = s->x[0];
    s->x[2] = ~s->x[2];
    s->x[3] = ROTR(s->x[3], R[3][1] - R[3][0]);
    t4 ^= s->x[4];
    s->x[4] = ROTR(s->x[4], R[4][1] - R[4][0]);
    s->x[3] ^= t3;
    s->x[1] ^= t1;
    s->x[0] = ROTR(s->x[0], R[0][0]);
    s->x[4] ^= t4;
    t0 ^= s->x[0];
    s->x[0] = ROTR(s->x[0], R[0][1] - R[0][0]);
    s->x[0] ^= t0;
}

/* ---------------------------------------------------------------- */

forceinline void P12(state_t *s)
{
    ROUND(0xf0, s);
    ROUND(0xe1, s);
    ROUND(0xd2, s);
    ROUND(0xc3, s);
    ROUND(0xb4, s);
    ROUND(0xa5, s);
    ROUND(0x96, s);
    ROUND(0x87, s);
    ROUND(0x78, s);
    ROUND(0x69, s);
    ROUND(0x5a, s);
    ROUND(0x4b, s);
}

/* ---------------------------------------------------------------- */

forceinline void P6(state_t *s)
{
    ROUND(0x96, s);
    ROUND(0x87, s);
    ROUND(0x78, s);
    ROUND(0x69, s);
    ROUND(0x5a, s);
    ROUND(0x4b, s);
}

/* ---------------------------------------------------------------- */

forceinline void P1(state_t *s)
{
    ROUND(0x4b, s);
}

/* ---------------------------------------------------------------- */

#endif // ASCONP_H_
