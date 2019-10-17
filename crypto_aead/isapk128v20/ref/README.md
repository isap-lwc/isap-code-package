### Building Code:

    make all

### Executing Code:

    ./genkat_aead
    diff LWC_AEAD_KAT_128_128.txt LWC_AEAD_KAT_128_128_REF.txt

### External Sources:

* KeccakP-400-SnP.h, KeccakP-400-reference.c:
  * From: [eXtended Keccak Code Package](https://github.com/XKCP/XKCP/tree/master/lib/low/KeccakP-400/Reference)
  * Copyright: See file header

* brg_endian.h:
  * From: [eXtended Keccak Code Package](https://github.com/XKCP/XKCP/tree/master/lib/common)
  * Copyright: See file header

### Implementation By:

* [Robert Primas](https://rprimas.github.io)

