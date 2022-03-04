#ifndef ASCONP_H_
#define ASCONP_H_

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

/* ---------------------------------------------------------------- */

#define P_sH PROUNDS(s, 12)
#define P_sB PROUNDS(s, 1)
#define P_sE PROUNDS(s, 6)
#define P_sK PROUNDS(s, 12)

/* ---------------------------------------------------------------- */

#define U64TOWORD(x) U64BIG(x)
#define WORDTOU64(x) U64BIG(x)

/* ---------------------------------------------------------------- */

#define TOBI(x) (x)
#define FROMBI(x) (x)

/* ---------------------------------------------------------------- */

lane_t U64BIG(lane_t x)
{
    x.x = ((((x.x) & 0x00000000000000FFULL) << 56) | (((x.x) & 0x000000000000FF00ULL) << 40) |
           (((x.x) & 0x0000000000FF0000ULL) << 24) | (((x.x) & 0x00000000FF000000ULL) << 8) |
           (((x.x) & 0x000000FF00000000ULL) >> 8) | (((x.x) & 0x0000FF0000000000ULL) >> 24) |
           (((x.x) & 0x00FF000000000000ULL) >> 40) | (((x.x) & 0xFF00000000000000ULL) >> 56));
    return x;
}

/* ---------------------------------------------------------------- */

forceinline uint64_t ROR(uint64_t x, int n) { return x >> n | x << (-n & 63); }

/* ---------------------------------------------------------------- */

forceinline void ROUND(state_t *s, uint8_t C)
{
    uint64_t xtemp;
    /* round constant */
    s->x[2] ^= C;
    /* s-box layer */
    s->x[0] ^= s->x[4];
    s->x[4] ^= s->x[3];
    s->x[2] ^= s->x[1];
    xtemp = s->x[0] & ~s->x[4];
    s->x[0] ^= s->x[2] & ~s->x[1];
    s->x[2] ^= s->x[4] & ~s->x[3];
    s->x[4] ^= s->x[1] & ~s->x[0];
    s->x[1] ^= s->x[3] & ~s->x[2];
    s->x[3] ^= xtemp;
    s->x[1] ^= s->x[0];
    s->x[3] ^= s->x[2];
    s->x[0] ^= s->x[4];
    /* linear layer */
    xtemp = s->x[0] ^ ROR(s->x[0], 28 - 19);
    s->x[0] ^= ROR(xtemp, 19);
    xtemp = s->x[1] ^ ROR(s->x[1], 61 - 39);
    s->x[1] ^= ROR(xtemp, 39);
    xtemp = s->x[2] ^ ROR(s->x[2], 6 - 1);
    s->x[2] ^= ROR(xtemp, 1);
    xtemp = s->x[3] ^ ROR(s->x[3], 17 - 10);
    s->x[3] ^= ROR(xtemp, 10);
    xtemp = s->x[4] ^ ROR(s->x[4], 41 - 7);
    s->x[4] ^= ROR(xtemp, 7);
    s->x[2] = ~s->x[2];
}

/* ---------------------------------------------------------------- */

void PROUNDS(state_t *s, uint8_t nr)
{
    switch (nr)
    {
    case 12:
        ROUND(s, 0xf0);
        ROUND(s, 0xe1);
        ROUND(s, 0xd2);
        ROUND(s, 0xc3);
        ROUND(s, 0xb4);
        ROUND(s, 0xa5);
    case 6:
        ROUND(s, 0x96);
        ROUND(s, 0x87);
        ROUND(s, 0x78);
        ROUND(s, 0x69);
        ROUND(s, 0x5a);
    default:
        ROUND(s, 0x4b);
    }
}

/* ---------------------------------------------------------------- */

#endif // ASCONP_H_
