### This code was implemented by:

* Robert Primas

### This code uses the following files from external sources:

* KeccakP-400-SnP.h, KeccakP-400-reference.c :
From: [eXtended Keccak Code Package](https://github.com/XKCP/XKCP/tree/master/lib/low/KeccakP-400/Reference)
Copyright: See file header

* brg_endian.h :
From: [eXtended Keccak Code Package](https://github.com/XKCP/XKCP/tree/master/lib/common)
Copyright: See file header

### Building the code:

> make all

### Generate and compare KAT:

> ./genkat
> diff LWC_AEAD_KAT_128_128.txt LWC_AEAD_KAT_128_128_REFERENCE.txt
