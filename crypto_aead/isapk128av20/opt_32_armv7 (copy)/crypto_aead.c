#include <stdio.h>
#include <string.h>
#include "api.h"
#include "Isap.h"

int crypto_aead_encrypt(
	unsigned char *c,unsigned long *clen,
	const unsigned char *m,unsigned long mlen,
	const unsigned char *ad,unsigned long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
){
		// Ciphertext length is mlen + tag length
		*clen = mlen+ISAP_TAG_SZ;

		// Encrypt plaintext
		isap_enc(k,npub,m,mlen,c);

		// Generate tag
		unsigned char *tag = c+mlen;
		isap_mac(k,npub,ad,adlen,c,&mlen,tag);
		return 0;
}

// int crypto_aead_decrypt(
// 	unsigned char *m,unsigned long *mlen,
// 	unsigned char *nsec,
// 	const unsigned char *c,unsigned long clen,
// 	const unsigned char *ad,unsigned long adlen,
// 	const unsigned char *npub,
// 	const unsigned char *k
// ){
// 		// Plaintext length is clen - tag length
// 		*mlen = clen-ISAP_TAG_SZ;
//
// 		// Generate tag
// 		unsigned char tag[ISAP_TAG_SZ];
// 		isap_mac(k,npub,ad,adlen,c,mlen,tag);
//
// 		// Compare tag
// 		char tag_valid = 0xFF;
// 		for(size_t i = 0; i < ISAP_TAG_SZ; i++) {
// 			tag_valid &= ~(tag[i] ^ c[(*mlen)+i]);
// 		}
//
// 		// Perform decryption if tag is correct
// 		if(tag_valid == (char)0xFF){
// 			isap_enc(k,npub,c,*mlen,m);
// 			return 0;
// 		} else {
// 			return -1;
// 		}
// }
