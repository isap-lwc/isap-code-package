#include <stdio.h>
#include <string.h>
#include "api.h"
#include "isap.h"
#include "asconp.h"

const u8 ISAP_IV1[] = {0x01,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const u8 ISAP_IV2[] = {0x02,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const u8 ISAP_IV3[] = {0x03,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};

/******************************************************************************/
/*                                   IsapRk                                   */
/******************************************************************************/

void isap_rk(
	const u8 *k,
	const u8 *iv,
	const u8 *y,
	const u64 ylen,
	u8 *out,
	const u64 outlen
){
    const u64 *k64 = (u64 *)k;
    const u64 *iv64 = (u64 *)iv;
    u64 *out64 = (u64 *)out;
    u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;

    // Init state
    t0 = t1 = t2 = t3 = t4 = 0;
    x0 = U64BIG(k64[0]);
    x1 = U64BIG(k64[1]);
    x2 = U64BIG(iv64[0]);
    x3 = x4 = 0;
    P12;

    // Absorb Y
    for (size_t i = 0; i < ylen*8-1; i++){
        size_t cur_byte_pos = i/8;
        size_t cur_bit_pos = 7-(i%8);
        u8 cur_bit = ((y[cur_byte_pos] >> (cur_bit_pos)) & 0x01) << 7;
        x0 ^= ((u64)cur_bit) << 56;
        P1;
    }
    u8 cur_bit = ((y[ylen-1]) & 0x01) << 7;
    x0 ^= ((u64)cur_bit) << 56;
    P12;

    // Extract K*
    out64[0] = U64BIG(x0);
    out64[1] = U64BIG(x1);
    if(outlen == 24){
        out64[2] = U64BIG(x2);
    }
}




void isap_rk2(
	const u8 *k,
	const u8 *iv,
	const u8 *y,
	const size_t ylen,
	state_t *out,
	const u64 outlen
){
    // u64 *out64 = (u64 *)out;
    // u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;

    state_t s;
    state_t* ss = &s;

    // Init state
    t0 = t1 = t2 = t3 = t4 = 0;
    ss->x[0] = U64BIG(((u64*)k)[0]);
    ss->x[1] = U64BIG(((u64*)k)[1]);
    ss->x[2] = U64BIG(((u64*)iv)[0]);
    ss->x[3] = 0;
    ss->x[4] = 0;
    P122;

    // // Absorb Y
    // for (size_t i = 0; i < ylen*8-1; i++){
    //     size_t cur_byte_pos = i/8;
    //     size_t cur_bit_pos = 7-(i%8);
    //     u8 cur_bit = ((          ss->b[cur_byte_pos/]     >> (cur_bit_pos)) & 0x01) << 7;
    //     ss->x[0] ^= ((u64)cur_bit) << 56;
    //     P1;
    // }
    // u8 cur_bit = ((y[ylen-1]) & 0x01) << 7;
    // x0 ^= ((u64)cur_bit) << 56;
    // P12;

    // Absorb Y, bit by bit
    for (size_t i = 0; i < 16; i++)
    {
        u64 cur_byte = y[i];
        ss->x[0] ^= (cur_byte & 0x80) << 56;
        P11;
        ss->x[0] ^= (cur_byte & 0x40) << 57;
        P11;
        ss->x[0] ^= (cur_byte & 0x20) << 58;
        P11;
        ss->x[0] ^= (cur_byte & 0x10) << 59;
        P11;
        ss->x[0] ^= (cur_byte & 0x08) << 60;
        P11;
        ss->x[0] ^= (cur_byte & 0x04) << 61;
        P11;
        ss->x[0] ^= (cur_byte & 0x02) << 62;
        P11;
        ss->x[0] ^= (cur_byte & 0x01) << 63;
        if (i != 15)
        {
            P11;
            y += 1;
        }
    }

    // Extract K*
    out->x[0] = U64BIG(ss->x[0]);
    out->x[1] = U64BIG(ss->x[1]);
    if(outlen == 24){
        out->x[2] = U64BIG(ss->x[2]);
    }
}

/******************************************************************************/
/*                                  IsapMac                                   */
/******************************************************************************/

void isap_mac(
    const u8 *k,
    const u8 *npub,
    const u8 *ad, const u64 adlen,
    const u8 *c, const u64 clen,
    u8 *tag
){
    u8 state[ISAP_STATE_SZ];
    const u64 *npub64 = (u64 *)npub;
    u64 *state64 = (u64 *)state;

    state_t s;
    state_t* ss = &s;

    // u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;
    t0 = t1 = t2 = t3 = t4 = 0;

    // Init state
    // x0 = U64BIG(npub64[0]);
    // x1 = U64BIG(npub64[1]);
    // x2 = U64BIG(((u64 *)ISAP_IV1)[0]);
    // x3 = x4 = 0;
    // P12;
    ss->x[0] = U64BIG(npub64[0]);
    ss->x[1] = U64BIG(npub64[1]);
    ss->x[2] = U64BIG(((u64 *)ISAP_IV1)[0]);
    ss->x[3] = 0;
    ss->x[4] = 0;
    P122;

    // Absorb AD
    ABSORB_LANES2(ss,ad,adlen);

    // Domain seperation
    ss->x[4] ^= 0x0000000000000001ULL;

    // Absorb C
    ABSORB_LANES2(ss,c,clen);

    // u64 x0, x1, x2, x3, x4;
    // x0 = ss->x[0];
    // x1 = ss->x[1];
    // x2 = ss->x[2];
    // x3 = ss->x[3];
    // x4 = ss->x[4];

    // // Derive K*
    // state64[0] = U64BIG(x0);
    // state64[1] = U64BIG(x1);
    // state64[2] = U64BIG(x2);
    // state64[3] = U64BIG(x3);
    // state64[4] = U64BIG(x4);
    // isap_rk(k,ISAP_IV2,(u8 *)state64,CRYPTO_KEYBYTES,(u8 *)state64,CRYPTO_KEYBYTES);
    // x0 = U64BIG(state64[0]);
    // x1 = U64BIG(state64[1]);
    // x2 = U64BIG(state64[2]);
    // x3 = U64BIG(state64[3]);
    // x4 = U64BIG(state64[4]);

    // Derive K*
    ss->x[0] = U64BIG(ss->x[0]);
    ss->x[1] = U64BIG(ss->x[1]);
    ss->x[2] = U64BIG(ss->x[2]);
    ss->x[3] = U64BIG(ss->x[3]);
    ss->x[4] = U64BIG(ss->x[4]);
    isap_rk2(k,ISAP_IV2,(u8*)(ss->b),CRYPTO_KEYBYTES,ss,CRYPTO_KEYBYTES);
    ss->x[0] = U64BIG(ss->x[0]);
    ss->x[1] = U64BIG(ss->x[1]);
    ss->x[2] = U64BIG(ss->x[2]);
    ss->x[3] = U64BIG(ss->x[3]);
    ss->x[4] = U64BIG(ss->x[4]);

    u64 x0, x1, x2, x3, x4;
    x0 = ss->x[0];
    x1 = ss->x[1];
    x2 = ss->x[2];
    x3 = ss->x[3];
    x4 = ss->x[4];

    // Squeeze tag
    P12;
    unsigned long long *tag64 = (u64 *)tag;
    tag64[0] = U64BIG(x0);
    tag64[1] = U64BIG(x1);
}

/******************************************************************************/
/*                                  IsapEnc                                   */
/******************************************************************************/

void isap_enc(
	const u8 *k,
	const u8 *npub,
	const u8 *m,
    const u64 mlen,
	u8 *c
){
    u8 state[ISAP_STATE_SZ];

    // Init state
    u64 *state64 = (u64 *)state;
    u64 *npub64 = (u64 *)npub;
    isap_rk(k,ISAP_IV3,npub,CRYPTO_NPUBBYTES,state,ISAP_STATE_SZ-CRYPTO_NPUBBYTES);
    u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;
    t0 = t1 = t2 = t3 = t4 = 0;
    x0 = U64BIG(state64[0]);
    x1 = U64BIG(state64[1]);
    x2 = U64BIG(state64[2]);
    x3 = U64BIG(npub64[0]);
    x4 = U64BIG(npub64[1]);
    P6;

    // Squeeze key stream
    u64 rem_bytes = mlen;
    u64 *m64 = (u64 *)m;
    u64 *c64 = (u64 *)c;
    u32 idx64 = 0;
    while(1){
        if(rem_bytes>ISAP_rH_SZ){
            // Squeeze full lane
            c64[idx64] = U64BIG(x0) ^ m64[idx64];
            idx64++;
            P6;
            rem_bytes -= ISAP_rH_SZ;
        } else if(rem_bytes==ISAP_rH_SZ){
            // Squeeze full lane and stop
            c64[idx64] = U64BIG(x0) ^ m64[idx64];
            break;
        } else {
            // Squeeze partial lane and stop
            u64 lane64 = U64BIG(x0);
            u8 *lane8 = (u8 *)&lane64;
            u32 idx8 = idx64*8;
            for (u32 i = 0; i < rem_bytes; i++) {
                c[idx8] = lane8[i] ^ m[idx8];
                idx8++;
            }
            break;
        }
    }
}
