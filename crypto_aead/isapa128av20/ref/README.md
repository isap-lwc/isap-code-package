### This code was implemented by:

* Robert Primas

### This code uses the following files from external sources:

* Ascon-reference.h, Ascon-reference.c :
From: [Ascon Software Repository](https://github.com/ascon/crypto_aead/archive/master.zip)
Copyright: See zip folder

### Building the code:

> make all

### Generate and compare KAT:

> ./genkat
> diff LWC_AEAD_KAT_128_128.txt LWC_AEAD_KAT_128_128_REFERENCE.txt
