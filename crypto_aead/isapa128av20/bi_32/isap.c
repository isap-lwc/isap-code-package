#include <string.h>
#include <inttypes.h>
#include "api.h"
#include "isap.h"
#include "asconp.h"
#include "config.h"

// ISAP-A-128a
const uint8_t ISAP_IV_A[] = {0x01, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
const uint8_t ISAP_IV_KA[] = {0x02, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};
const uint8_t ISAP_IV_KE[] = {0x03, ISAP_K, ISAP_rH, ISAP_rB, ISAP_sH, ISAP_sB, ISAP_sE, ISAP_sK};

forceinline void ABSORB_LANES(state_t *s, const uint8_t *src, uint64_t len)
{
    lane_t t0;
    while (len >= 8)
    {
        // Absorb full lanes
        to_bit_interleaving(&t0, U64BIG(*(lane_t *)src));
        s->w[0][0] ^= t0.w[0];
        s->w[0][1] ^= t0.w[1];
        len -= ISAP_rH / 8;
        src += ISAP_rH / 8;
        P_sH;
    }

    if (len > 0)
    {
        // Absorb partial lanes
        size_t i;
        lane_t t1 = {0};
        for (i = 0; i < len; i++)
        {
            t1.b[7 - i] ^= *src;
            src++;
        }
        t1.b[7 - i] ^= 0x80;
        to_bit_interleaving(&t1, t1);
        s->w[0][0] ^= t1.w[0];
        s->w[0][1] ^= t1.w[1];
        P_sH;
    }
    else
    {
        // Absorb empty lane (padding)
        s->b[0][7] ^= 0x80;
        P_sH;
    }
}

/******************************************************************************/
/*                                 ISAP_RK                                    */
/******************************************************************************/

void isap_rk(
    const uint8_t *k,
    const uint8_t *iv,
    const uint8_t *y,
    state_t *out,
    const size_t outlen)
{
    state_t state;
    state_t *s = &state;

    // Init state
    to_bit_interleaving(s->l + 0, U64BIG(*(lane_t *)(k + 0)));
    to_bit_interleaving(s->l + 1, U64BIG(*(lane_t *)(k + 8)));
    to_bit_interleaving(s->l + 2, U64BIG(*(lane_t *)(iv + 0)));
    s->x[3] = 0;
    s->x[4] = 0;
    P_sK;

    // Absorb Y, bit by bit
    for (size_t i = 0; i < 16; i++)
    {
        uint32_t cur_byte = *y;
        s->w[0][1] ^= (cur_byte & 0x80) << 24;
        P_sB;
        s->w[0][1] ^= (cur_byte & 0x40) << 25;
        P_sB;
        s->w[0][1] ^= (cur_byte & 0x20) << 26;
        P_sB;
        s->w[0][1] ^= (cur_byte & 0x10) << 27;
        P_sB;
        s->w[0][1] ^= (cur_byte & 0x08) << 28;
        P_sB;
        s->w[0][1] ^= (cur_byte & 0x04) << 29;
        P_sB;
        s->w[0][1] ^= (cur_byte & 0x02) << 30;
        P_sB;
        s->w[0][1] ^= (cur_byte & 0x01) << 31;
        if (i != 15)
        {
            P_sB;
            y += 1;
        }
    }

    // Squeeze K*
    P_sK;
    out->x[0] = s->x[0];
    out->x[1] = s->x[1];
    if (outlen > 16)
    {
        out->x[2] = s->x[2];
    }
}

/******************************************************************************/
/*                                 ISAP_MAC                                   */
/******************************************************************************/

void isap_mac(
    const uint8_t *k,
    const uint8_t *npub,
    const uint8_t *ad, uint64_t adlen,
    const uint8_t *c, uint64_t clen,
    uint8_t *tag)
{
    lane_t t0;
    state_t state;
    state_t *s = &state;

    // Initialize
    to_bit_interleaving(s->l + 0, U64BIG(*(lane_t *)(npub + 0)));
    to_bit_interleaving(s->l + 1, U64BIG(*(lane_t *)(npub + 8)));
    to_bit_interleaving(s->l + 2, U64BIG(*(lane_t *)(ISAP_IV_A + 0)));
    s->x[3] = 0;
    s->x[4] = 0;
    P_sH;

    ABSORB_LANES(s, ad, adlen);

    // Domain seperation
    s->w[4][0] ^= 0x1UL;

    ABSORB_LANES(s, c, clen);

    // Derive KA*
    from_bit_interleaving(&t0, s->l[0]);
    s->l[0] = U64BIG(t0);
    from_bit_interleaving(&t0, s->l[1]);
    s->l[1] = U64BIG(t0);
    isap_rk(k, ISAP_IV_KA, (const uint8_t *)(s->b), s, CRYPTO_KEYBYTES);

    // Create tag
    P_sH;
    from_bit_interleaving(&t0, s->l[0]);
    t0 = U64BIG(t0);
    memcpy(tag + 0, t0.b, 8);
    from_bit_interleaving(&t0, s->l[1]);
    t0 = U64BIG(t0);
    memcpy(tag + 8, t0.b, 8);
}

/******************************************************************************/
/*                                 ISAP_ENC                                   */
/******************************************************************************/

void isap_enc(
    const uint8_t *k,
    const uint8_t *npub,
    const uint8_t *m, uint64_t mlen,
    uint8_t *c)

{
    state_t state;
    state_t *s = &state;

    // Init state
    isap_rk(k, ISAP_IV_KE, npub, s, ISAP_STATE_SZ - CRYPTO_NPUBBYTES);
    to_bit_interleaving(s->l + 3, U64BIG(*(lane_t *)(npub + 0)));
    to_bit_interleaving(s->l + 4, U64BIG(*(lane_t *)(npub + 8)));

    lane_t t0;
    while (mlen >= ISAP_rH / 8)
    {
        // Squeeze full lanes
        P_sE;
        from_bit_interleaving(&t0, s->l[0]);
        *(uint64_t *)c = *(uint64_t *)m ^ U64BIG(t0).x;
        mlen -= ISAP_rH / 8;
        m += ISAP_rH / 8;
        c += ISAP_rH / 8;
    }

    if (mlen > 0)
    {
        // Squeeze partial lane
        P_sE;
        from_bit_interleaving(&t0, s->l[0]);
        t0 = U64BIG(t0);
        uint8_t *tmp0_bytes = (uint8_t *)&t0;
        for (uint8_t i = 0; i < mlen; i++)
        {
            *c = *m ^ tmp0_bytes[i];
            m += 1;
            c += 1;
        }
    }
}

/******************************************************************************/
/*                                Ascon-Hash                                  */
/******************************************************************************/

#if ENABLE_HASH == 1

const uint8_t ASCON_HASH_IV[] = {0x00, 0x40, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00};

int crypto_hash(uint8_t *out, const uint8_t *in, unsigned long long inlen)
{

    state_t state;
    state_t *s = &state;

    // Initialize
    to_bit_interleaving(s->l + 0, U64BIG(*(lane_t *)(ASCON_HASH_IV)));
    s->x[1] = 0;
    s->x[2] = 0;
    s->x[3] = 0;
    s->x[4] = 0;
    P_sH;

    ABSORB_LANES(s, in, inlen);

    lane_t t0;
    for (size_t i = 0; i < 4; i++)
    {
        // Squeeze full lanes
        from_bit_interleaving(&t0, s->l[0]);
        *(uint64_t *)(out + 8 * i) = U64BIG(t0).x;
        if (i < 3)
        {
            P_sH;
        }
    }
    
    return 0;
}

#endif
