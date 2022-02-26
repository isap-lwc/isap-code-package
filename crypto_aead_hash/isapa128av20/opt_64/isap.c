#include <stdio.h>
#include "api.h"
#include "isap.h"
#include "asconp.h"
#include "forceinline.h"

// ISAP-A-128a
const u8 ISAP_IV1[] = {0x01, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
const u8 ISAP_IV2[] = {0x02, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
const u8 ISAP_IV3[] = {0x03, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
#define P_sH P12(s)
#define P_sB P1(s)
#define P_sE P6(s)
#define P_sK P12(s)

forceinline void ABSORB_LANES(state_t *s, const u8 *src, u64 len)
{
    while (len >= ISAP_rH_SZ)
    {
        s->x[0] ^= U64BIG(*(u64 *)src);
        len -= ISAP_rH_SZ;
        src += 8;
        P_sH;
    }
    if (len > 0)
    {
        u64 i;
        for (i = 0; i < len; i++)
        {
            s->b[0][7 - i] ^= *src;
            src++;
        }
        s->b[0][7 - i] ^= 0x80;
        P_sH;
    }
    else
    {
        s->b[0][7] ^= 0x80;
        P_sH;
    }
}

/******************************************************************************/
/*                                   IsapRk                                   */
/******************************************************************************/

void isap_rk(
    const u8 *k,
    const u8 *iv,
    const u8 *y,
    const size_t ylen,
    state_t *out,
    const u64 outlen)
{
    state_t state;
    state_t *s = &state;

    // Init state
    s->x[0] = U64BIG(((u64 *)k)[0]);
    s->x[1] = U64BIG(((u64 *)k)[1]);
    s->x[2] = U64BIG(((u64 *)iv)[0]);
    s->x[3] = 0;
    s->x[4] = 0;
    P_sK;

    // Absorb Y, bit by bit
    for (size_t i = 0; i < 16; i++)
    {
        u64 cur_byte = *y;
        s->x[0] ^= (cur_byte & 0x80) << 56;
        P_sB;
        s->x[0] ^= (cur_byte & 0x40) << 57;
        P_sB;
        s->x[0] ^= (cur_byte & 0x20) << 58;
        P_sB;
        s->x[0] ^= (cur_byte & 0x10) << 59;
        P_sB;
        s->x[0] ^= (cur_byte & 0x08) << 60;
        P_sB;
        s->x[0] ^= (cur_byte & 0x04) << 61;
        P_sB;
        s->x[0] ^= (cur_byte & 0x02) << 62;
        P_sB;
        s->x[0] ^= (cur_byte & 0x01) << 63;
        if (i != 15)
        {
            P_sB;
            y += 1;
        }
    }

    // Extract K*
    P_sK;
    out->x[0] = s->x[0];
    out->x[1] = s->x[1];
    if (outlen == 24)
    {
        out->x[2] = s->x[2];
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
    u8 *tag)
{
    state_t state;
    state_t *s = &state;

    s->x[0] = U64BIG(((u64 *)npub)[0]);
    s->x[1] = U64BIG(((u64 *)npub)[1]);
    s->x[2] = U64BIG(((u64 *)ISAP_IV1)[0]);
    s->x[3] = 0;
    s->x[4] = 0;
    P_sH;

    // Absorb AD
    ABSORB_LANES(s, ad, adlen);

    // Domain seperation
    s->x[4] ^= 0x1ULL;

    // Absorb C
    ABSORB_LANES(s, c, clen);

    // Derive K*
    s->x[0] = U64BIG(s->x[0]);
    s->x[1] = U64BIG(s->x[1]);
    isap_rk(k, ISAP_IV2, (u8 *)(s->b), CRYPTO_KEYBYTES, s, CRYPTO_KEYBYTES);

    // Squeeze tag
    P_sH;
    ((u64 *)tag)[0] = U64BIG(s->x[0]);
    ((u64 *)tag)[1] = U64BIG(s->x[1]);
}

/******************************************************************************/
/*                                  IsapEnc                                   */
/******************************************************************************/

void isap_enc(
    const u8 *k,
    const u8 *npub,
    const u8 *m,
    u64 mlen,
    u8 *c)
{
    state_t state;
    state_t *s = &state;
    isap_rk(k, ISAP_IV3, npub, CRYPTO_NPUBBYTES, s, ISAP_STATE_SZ - CRYPTO_NPUBBYTES);

    // Init state
    s->x[3] = U64BIG(((u64 *)npub)[0]);
    s->x[4] = U64BIG(((u64 *)npub)[1]);
    P_sE;

    // Squeeze key stream
    while (mlen >= ISAP_rH_SZ)
    {
        // Squeeze full lane
        *(u64 *)c = U64BIG(s->x[0]) ^ *(u64 *)m;
        mlen -= ISAP_rH_SZ;
        m += 8;
        c += 8;
        P_sE;
    }
    if (mlen > 0)
    {
        // Squeeze partial lane
        for (size_t i = 0; i < mlen; i++)
        {
            *c = s->b[0][7 - i] ^ *m;
            c += 1;
            m += 1;
        }
    }
}

/******************************************************************************/
/*                                Ascon-Hash                                  */
/******************************************************************************/

// Ascon-Hash
const u8 ASCON_HASH_IV[] = {0x00, 0x40, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00};

int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    state_t state;
    state_t *s = &state;

    // Initialize
    s->x[0] = U64BIG(*(u64 *)ASCON_HASH_IV);
    s->x[1] = 0;
    s->x[2] = 0;
    s->x[3] = 0;
    s->x[4] = 0;
    P_sH;

    ABSORB_LANES(s, in, inlen);

    // Squeeze full lanes
    for (size_t i = 0; i < 4; i++)
    {
        *(u64 *)out = U64BIG(s->x[0]);
        out += 8;
        if (i < 3)
        {
            P_sH;
        }
    }

    return 0;
}
