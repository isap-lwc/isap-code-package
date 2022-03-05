#include <stdio.h>
#include <string.h>

#include "crypto_aead.h"
#include "api.h"

#define MESSAGE_LENGTH			1536
#define ASSOCIATED_DATA_LENGTH	0

#define RUNS 10000

void init_buffer(unsigned char *buffer, unsigned long long numbytes)
{
	for (unsigned long long i = 0; i < numbytes; i++)
		buffer[i] = (unsigned char)i;
}

int main()
{
	unsigned char       key[CRYPTO_KEYBYTES];
	unsigned char		nonce[CRYPTO_NPUBBYTES];
	unsigned char       msg[MESSAGE_LENGTH];
	unsigned char		ad[ASSOCIATED_DATA_LENGTH];
	unsigned char		ct[MESSAGE_LENGTH + CRYPTO_ABYTES];
	unsigned long long	clen;

	init_buffer(key, sizeof(key));
	init_buffer(nonce, sizeof(nonce));
	init_buffer(msg, sizeof(msg));
	init_buffer(ad, sizeof(ad));

	unsigned long long mlen = MESSAGE_LENGTH;
	unsigned long long adlen = ASSOCIATED_DATA_LENGTH;
	
	for (int i = 0; i < RUNS; i++) {
		crypto_aead_encrypt(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
	}
	
	return 0;
}

