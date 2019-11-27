#include "api.h"
#include "isap.h"
#include "KeccakP-400-SnP.h"

const unsigned char ISAP_IV1[] = {0x01,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const unsigned char ISAP_IV2[] = {0x02,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const unsigned char ISAP_IV3[] = {0x03,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};

/****************************************/
/*               IsapRk                 */
/****************************************/

void isap_rk(
	const unsigned char *k,
	const unsigned char *iv,
	const unsigned char *in,
	const unsigned long inlen,
	unsigned char *out,
	const unsigned long outlen
){
	// Init State
	unsigned char state[ISAP_STATE_SZ];
	KeccakP400_Initialize(state);
	KeccakP400_AddBytes(state,k,0,CRYPTO_KEYBYTES);
	KeccakP400_AddBytes(state,iv,CRYPTO_KEYBYTES,ISAP_IV_SZ);

	// KeccakP400_Bi_State(state);
	// KeccakP400_Permute_Nrounds_Bi(state,ISAP_sK);
	KeccakP400_Permute_Nrounds(state,ISAP_sK);

	// Absorb
	for (unsigned long i = 0; i < inlen*8-1; i++){
		unsigned long cur_byte_pos = i/8;
		unsigned long cur_bit_pos = 7-(i%8);
		unsigned char cur_bit = ((in[cur_byte_pos] >> (cur_bit_pos)) & 0x01) << 7;
		state[0] ^= cur_bit;
		KeccakP400_Permute_Nrounds(state,ISAP_sB);
	}
	unsigned char cur_bit = ((in[inlen-1]) & 0x01) << 7;
	// state[3] ^= cur_bit;
	state[0] ^= cur_bit;
	// KeccakP400_Permute_Nrounds_Bi(state,ISAP_sK);
	KeccakP400_Permute_Nrounds(state,ISAP_sK);

	// Squeeze K*
	KeccakP400_ExtractBytes(state,out,0,outlen);
}

/****************************************/
/*               IsapMac                */
/****************************************/

void isap_mac(
	const unsigned char *k,
	const unsigned char *npub,
	const unsigned char *ad, const unsigned long adlen,
	const unsigned char *c, const unsigned long clen,
	unsigned char *tag
){
	// Init State
	unsigned char state[ISAP_STATE_SZ];
	unsigned long *state32 = (unsigned long *)state;

	KeccakP400_Initialize(state);
	KeccakP400_AddBytes(state,npub,0,CRYPTO_NPUBBYTES);
	KeccakP400_AddBytes(state,ISAP_IV1,CRYPTO_NPUBBYTES,ISAP_IV_SZ);
	KeccakP400_Permute_Nrounds(state,ISAP_sH);

	// Absorb AD
	unsigned long rem_bytes = adlen;
	unsigned long idx = 0;
	while(1){
		if(rem_bytes>ISAP_rH_SZ){
			KeccakP400_AddBytes(state,ad+idx,0,ISAP_rH_SZ);
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			idx += ISAP_rH_SZ;
			rem_bytes -= ISAP_rH_SZ;
		} else if(rem_bytes==ISAP_rH_SZ){
			KeccakP400_AddBytes(state,ad+idx,0,ISAP_rH_SZ);
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			// Absorb padding
			state[0] ^= 0x80;
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			break;
		} else {
			// Absorb partial lane
			unsigned char lane[ISAP_rH_SZ];
			for (unsigned long i = 0; i < ISAP_rH_SZ; i++) {
				if(i<(rem_bytes)){
					lane[i] = ad[idx];
					idx++;
				} else if(i==rem_bytes){
					// Absorb padding
					lane[i] = 0x80;
				} else {
					// Absorb padding
					lane[i] = 0x00;
				}
			}
			KeccakP400_AddBytes(state,lane,0,ISAP_rH_SZ);
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			break;
		}
	}

	// Domain Seperation
	unsigned char dom_sep = 0x01;
	state[ISAP_STATE_SZ-1] ^= 0x01;

	// Absorb M
	rem_bytes = clen;
	idx = 0;
	while(1){
		if(rem_bytes>ISAP_rH_SZ){
			// Absorb full lane
			KeccakP400_AddBytes(state,c+idx,0,ISAP_rH_SZ);
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			idx += ISAP_rH_SZ;
			rem_bytes -= ISAP_rH_SZ;
		} else if(rem_bytes==ISAP_rH_SZ){
			// Absorb full lane
			KeccakP400_AddBytes(state,c+idx,0,ISAP_rH_SZ);
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			// Absorb padding
			state[0] ^= 0x80;
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			break;
		} else {
			// Absorb partial lane
			unsigned char lane[ISAP_rH_SZ];
			for (unsigned long i = 0; i < ISAP_rH_SZ; i++) {
				if(i<(rem_bytes)){
					lane[i] = c[idx];
					idx++;
				} else if(i==rem_bytes){
					// Absorb padding
					lane[i] = 0x80;
				} else {
					// Absorb padding
					lane[i] = 0x00;
				}
			}
			KeccakP400_AddBytes(state,lane,0,ISAP_rH_SZ);
			KeccakP400_Permute_Nrounds(state,ISAP_sH);
			break;
		}
	}

	// Derive Ka*
	unsigned char y[CRYPTO_KEYBYTES];
	unsigned char ka_star[CRYPTO_KEYBYTES];
	KeccakP400_ExtractBytes(state,y,0,CRYPTO_KEYBYTES);
	isap_rk(k,ISAP_IV2,y,CRYPTO_KEYBYTES,ka_star,CRYPTO_KEYBYTES);

	// Squeezing Tag
	KeccakP400_OverwriteBytes(state,ka_star,0,CRYPTO_KEYBYTES);
	KeccakP400_Permute_Nrounds(state,ISAP_sH);
	KeccakP400_ExtractBytes(state,tag,0,CRYPTO_KEYBYTES);
}

/****************************************/
/*               IsapEnc                */
/****************************************/

void isap_enc(
	const unsigned char *k,
	const unsigned char *npub,
	const unsigned char *m, const unsigned long mlen,
	unsigned char *c
){
	// unsigned char state[ISAP_STATE_SZ];
	// unsigned long *state32 = (unsigned long *)state;
	//
	// isap_rk(k,ISAP_IV3,npub,CRYPTO_NPUBBYTES,state,ISAP_STATE_SZ-CRYPTO_NPUBBYTES);
	// KeccakP400_OverwriteBytes(state,npub,ISAP_STATE_SZ-CRYPTO_NPUBBYTES,CRYPTO_NPUBBYTES);
	//
	// // Squeeze Keystream
	// unsigned long key_bytes_avail = 0;
	// for (unsigned long i = 0; i < mlen; i++) {
	// 	if(key_bytes_avail == 0){
	// 		KeccakP400_Permute_Nrounds(state,ISAP_sE);
	// 		key_bytes_avail = ISAP_rH_SZ;
	// 	}
	// 	unsigned char keybyte;
	// 	KeccakP400_ExtractBytes(state,&keybyte,i%ISAP_rH_SZ,1);
	// 	c[i] = m[i] ^ keybyte;
	// 	key_bytes_avail--;
	// }
}
