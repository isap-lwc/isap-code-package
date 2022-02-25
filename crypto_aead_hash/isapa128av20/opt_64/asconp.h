#ifndef ASCONP_H_
#define ASCONP_H_

#include "forceinline.h"

typedef unsigned char u8;
typedef unsigned long long u64;
typedef unsigned long u32;
typedef long long i64;


typedef union {
  u64 x[5];
  u32 w[5][2];
  u8 b[5][8];
} state_t;

const int R[5][2] = {
    {19, 28}, {39, 61}, {1, 6}, {10, 17}, {7, 41}};

#define ROTR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#define EXT_BYTE(x, n) ((u8)((u64)(x) >> (8 * (7 - (n)))))
#define INS_BYTE(x, n) ((u64)(x) << (8 * (7 - (n))))

#define U64BIG(x)                              \
    ((ROTR(x, 8) & (0xFF000000FF000000ULL)) |  \
     (ROTR(x, 24) & (0x00FF000000FF0000ULL)) | \
     (ROTR(x, 40) & (0x0000FF000000FF00ULL)) | \
     (ROTR(x, 56) & (0x000000FF000000FFULL)))

#define ABSORB_LANES(src, len) ({ \
    u32 rem_bytes = len; \
    u64 *src64 = (u64 *)src; \
    u32 idx64 = 0; \
    while(1){ \
        if(rem_bytes>ISAP_rH_SZ){ \
            x0 ^= U64BIG(src64[idx64]); \
            idx64++; \
            P12; \
            rem_bytes -= ISAP_rH_SZ; \
        } else if(rem_bytes==ISAP_rH_SZ){ \
            x0 ^= U64BIG(src64[idx64]); \
            P12; \
            x0 ^= 0x8000000000000000ULL; \
            P12; \
            break; \
        } else { \
            u64 lane64; \
            u8 *lane8 = (u8 *)&lane64; \
            u32 idx8 = idx64*8; \
            for (u32 i = 0; i < 8; i++) { \
                if(i<(rem_bytes)){ \
                    lane8[i] = src[idx8]; \
                    idx8++; \
                } else if(i==rem_bytes){ \
                    lane8[i] = 0x80; \
                } else { \
                    lane8[i] = 0x00; \
                } \
            } \
            x0 ^= U64BIG(lane64); \
            P12; \
            break; \
        } \
    } \
})

forceinline void ROUND(u64 C, u64 *x0, u64 *x1, u64 *x2, u64 *x3, u64 *x4, u64 *t0, u64 *t1, u64 *t2, u64 *t3, u64 *t4)
{
    *x2 ^= C;
    *x0 ^= *x4;
    *x4 ^= *x3;
    *x2 ^= *x1;
    *t0 = *x0;
    *t4 = *x4;
    *t3 = *x3;
    *t1 = *x1;
    *t2 = *x2;
    *x0 = *t0 ^ ((~*t1) & *t2);
    *x2 = *t2 ^ ((~*t3) & *t4);
    *x4 = *t4 ^ ((~*t0) & *t1);
    *x1 = *t1 ^ ((~*t2) & *t3);
    *x3 = *t3 ^ ((~*t4) & *t0);
    *x1 ^= *x0;
    *t1 = *x1;
    *x1 = ROTR(*x1, R[1][0]);
    *x3 ^= *x2;
    *t2 = *x2;
    *x2 = ROTR(*x2, R[2][0]);
    *t4 = *x4;
    *t2 ^= *x2;
    *x2 = ROTR(*x2, R[2][1] - R[2][0]);
    *t3 = *x3;
    *t1 ^= *x1;
    *x3 = ROTR(*x3, R[3][0]);
    *x0 ^= *x4;
    *x4 = ROTR(*x4, R[4][0]);
    *t3 ^= *x3;
    *x2 ^= *t2;
    *x1 = ROTR(*x1, R[1][1] - R[1][0]);
    *t0 = *x0;
    *x2 = ~*x2;
    *x3 = ROTR(*x3, R[3][1] - R[3][0]);
    *t4 ^= *x4;
    *x4 = ROTR(*x4, R[4][1] - R[4][0]);
    *x3 ^= *t3;
    *x1 ^= *t1;
    *x0 = ROTR(*x0, R[0][0]);
    *x4 ^= *t4;
    *t0 ^= *x0;
    *x0 = ROTR(*x0, R[0][1] - R[0][0]);
    *x0 ^= *t0;
}

forceinline void ROUND2(u64 C, state_t* ss)
{
    u64 t0, t1, t2, t3, t4;
    ss->x[2] ^= C;
    ss->x[0] ^= ss->x[4];
    ss->x[4] ^= ss->x[3];
    ss->x[2] ^= ss->x[1];
    t0 = ss->x[0];
    t4 = ss->x[4];
    t3 = ss->x[3];
    t1 = ss->x[1];
    t2 = ss->x[2];
    ss->x[0] = t0 ^ ((~t1) & t2);
    ss->x[2] = t2 ^ ((~t3) & t4);
    ss->x[4] = t4 ^ ((~t0) & t1);
    ss->x[1] = t1 ^ ((~t2) & t3);
    ss->x[3] = t3 ^ ((~t4) & t0);
    ss->x[1] ^= ss->x[0];
    t1 = ss->x[1];
    ss->x[1] = ROTR(ss->x[1], R[1][0]);
    ss->x[3] ^= ss->x[2];
    t2 = ss->x[2];
    ss->x[2] = ROTR(ss->x[2], R[2][0]);
    t4 = ss->x[4];
    t2 ^= ss->x[2];
    ss->x[2] = ROTR(ss->x[2], R[2][1] - R[2][0]);
    t3 = ss->x[3];
    t1 ^= ss->x[1];
    ss->x[3] = ROTR(ss->x[3], R[3][0]);
    ss->x[0] ^= ss->x[4];
    ss->x[4] = ROTR(ss->x[4], R[4][0]);
    t3 ^= ss->x[3];
    ss->x[2] ^= t2;
    ss->x[1] = ROTR(ss->x[1], R[1][1] - R[1][0]);
    t0 = ss->x[0];
    ss->x[2] = ~ss->x[2];
    ss->x[3] = ROTR(ss->x[3], R[3][1] - R[3][0]);
    t4 ^= ss->x[4];
    ss->x[4] = ROTR(ss->x[4], R[4][1] - R[4][0]);
    ss->x[3] ^= t3;
    ss->x[1] ^= t1;
    ss->x[0] = ROTR(ss->x[0], R[0][0]);
    ss->x[4] ^= t4;
    t0 ^= ss->x[0];
    ss->x[0] = ROTR(ss->x[0], R[0][1] - R[0][0]);
    ss->x[0] ^= t0;
}

#define P122 ({      \
    ROUND2(0xf0, ss); \
    ROUND2(0xe1, ss); \
    ROUND2(0xd2, ss); \
    ROUND2(0xc3, ss); \
    ROUND2(0xb4, ss); \
    ROUND2(0xa5, ss); \
    ROUND2(0x96, ss); \
    ROUND2(0x87, ss); \
    ROUND2(0x78, ss); \
    ROUND2(0x69, ss); \
    ROUND2(0x5a, ss); \
    ROUND2(0x4b, ss); \
})

#define P11 ({      \
    ROUND2(0x4b, ss); \
})

#define P12 ({                                                     \
    ROUND(0xf0, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0xe1, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0xd2, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0xc3, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0xb4, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0xa5, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x96, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x87, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x78, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x69, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x5a, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x4b, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
})

#define P6 ({                                                      \
    ROUND(0x96, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x87, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x78, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x69, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x5a, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
    ROUND(0x4b, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
})

#define P1 ({                                                      \
    ROUND(0x4b, &x0, &x1, &x2, &x3, &x4, &t0, &t1, &t2, &t3, &t4); \
})

forceinline void ABSORB_LANES2(state_t* ss, const u8 *src, u64 len)
{
    u32 rem_bytes = len;
    u64 *src64 = (u64 *)src;
    u32 idx64 = 0;
    while (1)
    {
        if (rem_bytes > ISAP_rH_SZ)
        {
            ss->x[0] ^= U64BIG(src64[idx64]);
            idx64++;
            P122;
            rem_bytes -= ISAP_rH_SZ;
        }
        else if (rem_bytes == ISAP_rH_SZ)
        {
            ss->x[0] ^= U64BIG(src64[idx64]);
            P122;
            ss->x[0] ^= 0x8000000000000000ULL;
            P122;
            break;
        }
        else
        {
            u64 lane64;
            u8 *lane8 = (u8 *)&lane64;
            u32 idx8 = idx64 * 8;
            for (size_t i = 0; i < 8; i++)
            {
                if (i < rem_bytes)
                {
                    lane8[i] = src[idx8];
                    idx8++;
                }
                else if (i == rem_bytes)
                {
                    lane8[i] = 0x80;
                }
                else
                {
                    lane8[i] = 0x00;
                }
            }
            ss->x[0] ^= U64BIG(lane64);
            P122;
            break;
        }
    }
}

#endif // ASCONP_H_