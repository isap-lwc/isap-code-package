#ifndef ISAP_H
#define ISAP_H

#ifdef ISAP128

/******************************************************************************/
/*                               Isap-128                                     */
/******************************************************************************/

#ifdef KECCAKP400

/**************************************/
/*        Isap-128 & KeccakP400       */
/**************************************/

// Rate in bits
#define ISAP_R1 144
#define ISAP_R2 1

// Number of rounds
#define ISAP_A 20
#define ISAP_B 12
#define ISAP_C 12
#define ISAP_D 12

// State size in bytes
#define ISAP_STATE_SZ 50

#elif defined(ASCON)

/**************************************/
/*          Isap-128 & Ascon          */
/**************************************/

// Rate in bytes
#define ISAP_R1 64
#define ISAP_R2 1

// Number of rounds
#define ISAP_A 12
#define ISAP_B 12
#define ISAP_C 12
#define ISAP_D 12

// State size in bytes
#define ISAP_STATE_SZ 40

#endif // ASCON

#elif defined(ISAP128A)

/******************************************************************************/
/*                               Isap-128a                                    */
/******************************************************************************/

#ifdef KECCAKP400

/**************************************/
/*       Isap-128a & KeccakP400       */
/**************************************/

// Rate in bits
#define ISAP_R1 144
#define ISAP_R2 1

// Number of rounds
#define ISAP_A 16
#define ISAP_B 1
#define ISAP_C 8
#define ISAP_D 8

// State size in bytes
#define ISAP_STATE_SZ 50

#elif defined(ASCON)

/**************************************/
/*         Isap-128a & Ascon          */
/**************************************/

// Rate in bits
#define ISAP_R1 64
#define ISAP_R2 1

// Number of rounds
#define ISAP_A 12
#define ISAP_B 1
#define ISAP_C 6
#define ISAP_D 12

// State size in bytes
#define ISAP_STATE_SZ 40

#endif // ASCON

#endif // ISAP128A

/******************************************************************************/
/*                                Generic                                     */
/******************************************************************************/

// Size of rate in bytes
#define ISAP_R1_SZ (ISAP_R1/8 + (ISAP_R1%8?1:0))

// Size of zero truncated IV in bytes
#define ISAP_IV_SZ 8

// Size of tag in bytes
#define ISAP_TAG_SZ 16

// Security level
#define ISAP_K 128

void isap_mac(
	const unsigned char *k,
	const unsigned char *npub,
	const unsigned char *ad, const unsigned long adlen,
	const unsigned char *in, const unsigned long *inlen,
	unsigned char *tag
);

void isap_rk(
	const unsigned char *k,
	const unsigned char *iv,
	const unsigned char *in,
	const unsigned long inlen,
	unsigned char *out,
	const unsigned long outlen
);

void isap_enc(
	const unsigned char *k,
	const unsigned char *npub,
	const unsigned char *m, const unsigned long mlen,
	unsigned char *c
);

#endif
