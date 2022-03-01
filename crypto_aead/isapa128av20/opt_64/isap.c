#include <stdio.h>
#include <inttypes.h>
#include "api.h"
#include "isap.h"
#include "asconp.h"
#include "forceinline.h"

// ISAP-A-128a
const uint8_t ISAP_IV_A[] = {0x01, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
const uint8_t ISAP_IV_KA[] = {0x02, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
const uint8_t ISAP_IV_KE[] = {0x03, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
#define P_sH P12(s)
#define P_sB P1(s)
#define P_sE P6(s)
#define P_sK P12(s)

forceinline void ABSORB_LANES(state_t *s, const uint8_t *src, uint64_t len)
{
    while (len >= ISAP_rH_SZ)
    {
        s->x[0] ^= U64BIG(*(uint64_t *)src);
        len -= ISAP_rH_SZ;
        src += 8;
        P_sH;
    }
    if (len > 0)
    {
        size_t i;
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
    const uint8_t *k,
    const uint8_t *iv,
    const uint8_t *y,
    const size_t ylen,
    state_t *out,
    const uint64_t outlen)
{
    state_t state;
    state_t *s = &state;

    // Init state
    s->x[0] = U64BIG(*(uint64_t *)(k + 0));
    s->x[1] = U64BIG(*(uint64_t *)(k + 8));
    s->x[2] = U64BIG(*(uint64_t *)(iv));
    s->x[3] = 0;
    s->x[4] = 0;
    P_sK;

    // Absorb Y, bit by bit
    for (size_t i = 0; i < 16; i++)
    {
        uint64_t cur_byte = *y;
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
    const uint8_t *k,
    const uint8_t *npub,
    const uint8_t *ad, const uint64_t adlen,
    const uint8_t *c, const uint64_t clen,
    uint8_t *tag)
{
    state_t state;
    state_t *s = &state;

    s->x[0] = U64BIG(*(uint64_t *)(npub + 0));
    s->x[1] = U64BIG(*(uint64_t *)(npub + 8));
    s->x[2] = U64BIG(*(uint64_t *)(ISAP_IV_A));
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
    isap_rk(k, ISAP_IV_KA, (uint8_t *)(s->b), CRYPTO_KEYBYTES, s, CRYPTO_KEYBYTES);

    // Squeeze tag
    P_sH;
    *(uint64_t *)(tag + 0) = U64BIG(s->x[0]);
    *(uint64_t *)(tag + 8) = U64BIG(s->x[1]);
}

/******************************************************************************/
/*                                  IsapEnc                                   */
/******************************************************************************/

void isap_enc(
    const uint8_t *k,
    const uint8_t *npub,
    const uint8_t *m,
    uint64_t mlen,
    uint8_t *c)
{
    state_t state;
    state_t *s = &state;
    isap_rk(k, ISAP_IV_KE, npub, CRYPTO_NPUBBYTES, s, ISAP_STATE_SZ - CRYPTO_NPUBBYTES);

    // Init state
    s->x[3] = U64BIG(*(uint64_t *)(npub + 0));
    s->x[4] = U64BIG(*(uint64_t *)(npub + 8));

    // Squeeze key stream
    while (mlen >= ISAP_rH_SZ)
    {
        // Squeeze full lane
        P_sE;
        *(uint64_t *)c = U64BIG(s->x[0]) ^ *(uint64_t *)m;
        mlen -= ISAP_rH_SZ;
        m += 8;
        c += 8;
    }

    if (mlen > 0)
    {
        // Squeeze partial lane
        P_sE;
        for (size_t i = 0; i < mlen; i++)
        {
            *c = s->b[0][7 - i] ^ *m;
            c += 1;
            m += 1;
        }
    }
}

