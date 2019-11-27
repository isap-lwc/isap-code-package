/*
Implementation by Robert Primas, hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#ifndef ISAP_CRYPTO_AEAD
#define ISAP_CRYPTO_AEAD

int crypto_aead_encrypt(
	unsigned char *c, unsigned long *clen,
	const unsigned char *m, unsigned long mlen,
	const unsigned char *ad, unsigned long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
);

int crypto_aead_decrypt(
	unsigned char *m, unsigned long *mlen,
	unsigned char *nsec,
	const unsigned char *c, unsigned long clen,
	const unsigned char *ad, unsigned long adlen,
	const unsigned char *npub,
	const unsigned char *k
);

#endif
