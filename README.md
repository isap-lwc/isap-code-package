## ISAP Code Package

ISAP is a family of lightweight authenticated encryption algorithms designed with a focus on **robustness against implementation attacks** and is currently competing in the 2nd round of the [NIST Lightweight Cryptography competition](https://csrc.nist.gov/Projects/Lightweight-Cryptography/Round-2-Candidates) (2019). It is of particular interest for applications like firmeware updates where **robustness against power analysis and fault attacks** is crucial and codesize and a **small footprint in hardware** matters. ISAP's original version was published at [FSE 2017](https://tosc.iacr.org/index.php/ToSC/article/view/585).

This repository contains reference code of ISAP, as well as optimized code for various platforms.

### External Sources:

* KeccakP-400-SnP.h, KeccakP-400-reference.c:
  * From: [eXtended Keccak Code Package](https://github.com/XKCP/XKCP/tree/master/lib/low/KeccakP-400/Reference)
  * Copyright: See file header

* brg_endian.h:
  * From: [eXtended Keccak Code Package](https://github.com/XKCP/XKCP/tree/master/lib/common)
  * Copyright: See file header

* Ascon-reference.h, Ascon-reference.c:
  * From: [Ascon Software Repository](https://github.com/ascon/crypto_aead/archive/master.zip)
  * Copyright: See zip folder

### License Information:

See LICENSE.txt
