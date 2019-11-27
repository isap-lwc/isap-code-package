#include <string.h>
#include "api.h"
#include "isap.h"
#include "Ascon-bi32-unrolling.macros"

/* -------------------------------------------------------------------------- */

typedef unsigned char u8;
typedef unsigned long u32;

/* -------------------------------------------------------------------------- */

const u8 ISAP_IV1[] = {0x01,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const u8 ISAP_IV2[] = {0x02,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const u8 ISAP_IV3[] = {0x03,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};

/* -------------------------------------------------------------------------- */

#define EXPAND_SHORT(x)\
    x &= 0x0000ffff;\
    x = (x | (x << 8)) & 0x00ff00ff;\
    x = (x | (x << 4)) & 0x0f0f0f0f;\
    x = (x | (x << 2)) & 0x33333333;\
    x = (x | (x << 1)) & 0x55555555;\

/* -------------------------------------------------------------------------- */

#define EXPAND_U32(var,var_o,var_e)\
   /*var 32-bit, and var_o/e 16-bit*/\
   t0_e = (var_e);\
   t0_o = (var_o);\
   EXPAND_SHORT(t0_e);\
   EXPAND_SHORT(t0_o);\
   var = t0_e | (t0_o << 1);\

/* -------------------------------------------------------------------------- */

#define COMPRESS_LONG(x)\
   x &= 0x55555555;\
   x = (x | (x >> 1)) & 0x33333333;\
   x = (x | (x >> 2)) & 0x0f0f0f0f;\
   x = (x | (x >> 4)) & 0x00ff00ff;\
   x = (x | (x >> 8)) & 0x0000ffff;\

/* -------------------------------------------------------------------------- */

#define COMPRESS_U32(var,var_o,var_e)\
  /*var 32-bit, and var_o/e 16-bit*/\
  var_e = var;\
  var_o = var_e >> 1;\
  COMPRESS_LONG(var_e);\
  COMPRESS_LONG(var_o);\

/* -------------------------------------------------------------------------- */

#define ABSORB_LANE(xxx)\
    x0_l = xxx##32[idx32];\
    ENDIAN_SWAP(x0_l);\
    idx32++;\
    x0_h = xxx##32[idx32];\
    ENDIAN_SWAP(x0_h);\
    idx32++;\
    BI_LANE(x0_l,x0_h);\
    x0_o ^= x0_l;\
    x0_e ^= x0_h;\

/* -------------------------------------------------------------------------- */

#define ABSORB_LANE_PARTIAL(xxx)\
    u32 lane32[2];\
    u8 *lane8 = (u8*)lane32;\
    u32 idx8 = idx32*4;\
    for (u32 i = 0; i < 8; i++) {\
        if(i<(rem_bytes)){\
            lane8[i] = xxx##8[idx8];\
            idx8++;\
        } else if(i==rem_bytes){\
            lane8[i] = 0x80;\
        } else {\
            lane8[i] = 0x00;\
        }\
    }\
    x0_l = lane32[0];\
    x0_h = lane32[1];\
    ENDIAN_SWAP(x0_l);\
    ENDIAN_SWAP(x0_h);\
    BI_LANE(x0_l,x0_h);\
    x0_o ^= x0_l;\
    x0_e ^= x0_h;\

/* -------------------------------------------------------------------------- */

#define SQUEEZE_LANE\
    x0_l = x0_o;\
    x0_h = x0_e;\
    IB_LANE(x0_l,x0_h);\
    ENDIAN_SWAP(x0_l);\
    ENDIAN_SWAP(x0_h);\
    c32[idx32] = x0_l ^ m32[idx32];\
    idx32++;\
    c32[idx32] = x0_h ^ m32[idx32];\
    idx32++;\

/* -------------------------------------------------------------------------- */

#define SQUEEZE_LANE_PARTIAL\
    u32 lane32[2];\
    IB_LANE(x0_o,x0_e);\
    ENDIAN_SWAP(x0_o);\
    ENDIAN_SWAP(x0_e);\
    lane32[0] = x0_o;\
    lane32[1] = x0_e;\
    u8 *lane8 = (u8*)lane32;\
    u32 idx8 = idx32*4;\
    for (u32 i = 0; i < rem_bytes; i++) {\
        c[idx8] = lane8[i] ^ m[idx8];\
        idx8++;\
    }\

/* -------------------------------------------------------------------------- */

#define BI_LANE(xo,xe)\
    t0_o=xo;\
    COMPRESS_U32(t0_o,t1_o,t1_e);\
    t1_o<<=16;\
    t1_e<<=16;\
    t0_o=xe;\
    COMPRESS_U32(t0_o,xo,xe);\
    xo|=t1_o;\
    xe|=t1_e;\

/* -------------------------------------------------------------------------- */

#define IB_LANE(xo,xe)\
    EXPAND_U32(t1_o,xo>>16,xe>>16);\
    EXPAND_U32(t1_e,xo,xe);\
    xo=t1_o;\
    xe=t1_e;\

/* -------------------------------------------------------------------------- */

#define ENDIAN_SWAP(x)\
    __asm__ __volatile__ (\
    "rev %[y], %[y]\n\t"\
    : [y] "+r" (x)::);\

/******************************************************************************/
/*                                 ISAP_RK                                    */
/******************************************************************************/

void isap_rk(
	const u8 *k,
	const u8 *iv,
	const u8 *in,
	const u32 inlen,
	u8 *out,
	const u32 outlen
){
	// Init State
    u32 *k32 = (u32 *)k;
    u32 *iv32 = (u32 *)iv;
    u32 *out32 = (u32 *)out;

    u32 t0_o, t1_o, t0_e, t1_e;
    u32 x0_o = k32[0]; ENDIAN_SWAP(x0_o);
    u32 x0_e = k32[1]; ENDIAN_SWAP(x0_e);
    u32 x1_o = k32[2]; ENDIAN_SWAP(x1_o);
    u32 x1_e = k32[3]; ENDIAN_SWAP(x1_e);
    u32 x2_o = iv32[0]; ENDIAN_SWAP(x2_o);
    u32 x2_e = iv32[1]; ENDIAN_SWAP(x2_e);
    u32 x3_o = 0;
    u32 x3_e = 0;
    u32 x4_o = 0;
    u32 x4_e = 0;
    BI_LANE(x0_o,x0_e);
    BI_LANE(x1_o,x1_e);
    BI_LANE(x2_o,x2_e);
    P12_32;

	// Absorb
	for (u32 i = 0; i < inlen*8-1; i++){
		u32 cur_byte_pos = i/8;
		u32 cur_bit_pos = 7-(i%8);
		u8 cur_bit = ((in[cur_byte_pos] >> (cur_bit_pos)) & 0x01) << 7;
		x0_o ^= ((u32)cur_bit)<<(24);
		P1_32;
	}
	u8 cur_bit = ((in[inlen-1]) & 0x01) << 7;
	x0_o ^= ((u32)cur_bit)<<(24);
	P12_32;

    // Extract output
	out32[0] = x0_o;
	out32[1] = x0_e;
	out32[2] = x1_o;
	out32[3] = x1_e;
    if(outlen>16){
        out32[4] = x2_o;
    	out32[5] = x2_e;
    }
}

/******************************************************************************/
/*                                 ISAP_MAC                                   */
/******************************************************************************/

void isap_mac(
	const u8 *k,
	const u8 *npub,
	const u8 *ad, const u32 adlen,
	const u8 *in, const u32 inlen,
	u8 *tag
){
	// Init State
    u32 *npub32 = (u32*)npub;
    u32 *iv32 = (u32*)ISAP_IV1;
    u32 *tag32 = (u32*)tag;

    u32 t0_o, t1_o, t0_e, t1_e;
    u32 x0_o = npub32[0]; ENDIAN_SWAP(x0_o);
    u32 x0_e = npub32[1]; ENDIAN_SWAP(x0_e);
    u32 x1_o = npub32[2]; ENDIAN_SWAP(x1_o);
    u32 x1_e = npub32[3]; ENDIAN_SWAP(x1_e);
    u32 x2_o = iv32[0]; ENDIAN_SWAP(x2_o);
    u32 x2_e = iv32[1]; ENDIAN_SWAP(x2_e);
    u32 x3_o = 0;
    u32 x3_e = 0;
    u32 x4_o = 0;
    u32 x4_e = 0;
    BI_LANE(x0_o,x0_e);
    BI_LANE(x1_o,x1_e);
    BI_LANE(x2_o,x2_e);
	P12_32;

	// Absorb AD
	u32 rem_bytes = adlen;
	u32 *ad32 = (u32 *)ad;
    u8 *ad8 = (u8 *)ad;
	u32 idx32 = 0;
    u32 x0_l, x0_h;

    // Absorb full lanes of AD
    while(rem_bytes>8){
        ABSORB_LANE(ad);
        P12_32;
        rem_bytes -= 8;
    }

    // Absorb last lane of AD (partial)
    if(rem_bytes>0){
        ABSORB_LANE_PARTIAL(ad);
        P12_32;
    }

    // Absorb last lane of AD (empty)
    if(rem_bytes==0){
        x0_o ^= 0x80000000;
        P12_32;
    }

	// Domain Seperation
	x4_e ^= ((u32)0x01);

	// Absorb M
	rem_bytes = inlen;
	u32 *in32 = (u32 *)in;
    u8 *in8 = (u8 *)in;
	idx32 = 0;

    // Absorb full lanes of C/M
    while(rem_bytes>8){
        ABSORB_LANE(in);
        P12_32;
        rem_bytes -= 8;
    }

    // Absorb last lane of C/M (partial)
    if(rem_bytes>0){
        ABSORB_LANE_PARTIAL(in);
        P12_32;
    }

    // Absorb last lane of C/M (empty)
    if(rem_bytes==0){
        x0_o ^= 0x80000000;
        P12_32;
    }

	// Derive Ka*
	u32 y32[CRYPTO_KEYBYTES/4];
    IB_LANE(x0_o,x0_e);
    IB_LANE(x1_o,x1_e);
    ENDIAN_SWAP(x0_o); y32[0] = x0_o;
	ENDIAN_SWAP(x0_e); y32[1] = x0_e;
	ENDIAN_SWAP(x1_o); y32[2] = x1_o;
	ENDIAN_SWAP(x1_e); y32[3] = x1_e;
    u32 ka_star32[CRYPTO_KEYBYTES/4];
	isap_rk(k,ISAP_IV2,(u8*)y32,CRYPTO_KEYBYTES,(u8*)ka_star32,CRYPTO_KEYBYTES);

    // Squeezing Tag
    x0_o = ka_star32[0];
	x0_e = ka_star32[1];
	x1_o = ka_star32[2];
	x1_e = ka_star32[3];
    P12_32;
    IB_LANE(x0_o,x0_e);
    IB_LANE(x1_o,x1_e);
    ENDIAN_SWAP(x0_o); tag32[0] = x0_o;
    ENDIAN_SWAP(x0_e); tag32[1] = x0_e;
    ENDIAN_SWAP(x1_o); tag32[2] = x1_o;
    ENDIAN_SWAP(x1_e); tag32[3] = x1_e;
}

/******************************************************************************/
/*                                 ISAP_ENC                                   */
/******************************************************************************/

void isap_enc(
	const u8 *k,
	const u8 *npub,
	const u8 *m, const u32 mlen,
	u8 *c
){
    // Derive Ke
	u8 ke[ISAP_STATE_SZ-CRYPTO_NPUBBYTES];
	isap_rk(k,ISAP_IV3,npub,CRYPTO_NPUBBYTES,ke,ISAP_STATE_SZ-CRYPTO_NPUBBYTES);

    u32 *ke32 = (u32*)ke;
    u32 *npub32 = (u32*)npub;

    // Init State
    u32 t0_o, t1_o, t0_e, t1_e;
    u32 x0_o = ke32[0];
    u32 x0_e = ke32[1];
    u32 x1_o = ke32[2];
    u32 x1_e = ke32[3];
    u32 x2_o = ke32[4];
    u32 x2_e = ke32[5];
    u32 x3_o = npub32[0]; ENDIAN_SWAP(x3_o);
    u32 x3_e = npub32[1]; ENDIAN_SWAP(x3_e);
    u32 x4_o = npub32[2]; ENDIAN_SWAP(x4_o);
    u32 x4_e = npub32[3]; ENDIAN_SWAP(x4_e);
    BI_LANE(x3_o,x3_e);
    BI_LANE(x4_o,x4_e);

    // Squeeze Keystream
    u32 rem_bytes = mlen;
	u32 *m32 = (u32 *)m;
	u32 *c32 = (u32 *)c;
	u32 idx32 = 0;
    u32 x0_l, x0_h;

    // Squeeze full lanes
	while(rem_bytes>=8){
        P6_32;
		SQUEEZE_LANE;
        rem_bytes -= 8;
	}

    // Squeeze partial lane
    if(rem_bytes>0){
        P6_32;
        SQUEEZE_LANE_PARTIAL;
	}
}
