# ISAP Code Package

ISAP is a family of lightweight authenticated encryption algorithms designed with a focus on **robustness against implementation attacks** and is currently competing in the 2nd round of the NIST Lightweight Cryptography competition ([spec](https://csrc.nist.gov/Projects/Lightweight-Cryptography/Round-2-Candidates),[web](https://isap.iaik.tugraz.at)). It is of particular interest for applications like firmware updates where **robustness against power analysis and fault attacks** is crucial and code size and a **small footprint in hardware** matters. ISAP's original version was published at [FSE 2017](https://tosc.iacr.org/index.php/ToSC/article/view/585).

This repository contains the following ISAP algorithms:

- `crypto_aead/isapa128av20`: ISAP-A-128a v2.0
- `crypto_aead/isapa128v20`: ISAP-A-128 v2.0
- `crypto_aead/isapk128av20`: ISAP-K-128a v2.0
- `crypto_aead/isapk128v20`: ISAP-K-128 v2.0

and the following implementations:

- `ref`: reference implementation
- `opt_64`: 64-bit optimized C implementation
- `opt_32`: 32-bit optimized C implementation
- `opt_32_armv67`: 32-bit optimized C/ASM implementation for ARMv6 and ARMv7

## Performance results on different CPUs in cycles per byte:

ISAP was especially designed for scenarios where implementation security is required. It features, amongst others, **build-in hardening/protection against implementation attacks** such as Differential Power Analysis (DPA), Differential Fault Attacks (DFA), Statistical Fault Attacks (SFA), and Statistical Ineffective Fault Attacks (SIFA).

### ISAP-A-128a (recommended)

| Message Length in Bytes: |    64 |  1536 |  long |
|:-------------------------|------:|------:|------:|
| AMD Ryzen 7 1700 (x64)   |  85.7 |  24.5 |  21.9 |
| Intel i5-6200U (x64)     |   104 |  34.3 |  31.4 |
| Raspberry Pi 1B (ARMv6m) |   551 |   171 |   159 |
| STM32F0405 (ARMv7m)      |  1223 |   352 |   311 |

### ISAP-K-128a (recommended)

| Message Length in Bytes: |    64 |  1536 |  long |
|:-------------------------|------:|------:|------:|
| AMD Ryzen 7 1700 (x64)   |   295 |  64.1 |  54.3 |
| Intel i5-6200U (x64)     |   342 |  72.8 |  61.3 |
| Raspberry Pi 1B (ARMv6m) |  3464 |   743 |   635 |
| STM32F0405 (ARMv7m)      |  4007 |   808 |   658 |

### ISAP-A-128

| Message Length in Bytes: |    64 |  1536 |  long |
|:-------------------------|------:|------:|------:|
| AMD Ryzen 7 1700 (x64)   |   511 |  48.9 |  29.8 |
| Intel i5-6200U (x64)     |   698 |  68.1 |  42.0 |
| Raspberry Pi 1B (ARMv6m) |  3209 |   324 |   212 |
| STM32F0405 (ARMv7m)      |  6818 |   675 |   406 |

### ISAP-K-128

| Message Length in Bytes: |    64 |  1536 |  long |
|:-------------------------|------:|------:|------:|
| AMD Ryzen 7 1700 (x64)   |  2108 |   156 |  75.0 |
| Intel i5-6200U (x64)     |  2318 |   173 |  84.0 |
| Raspberry Pi 1B (ARMv6m) | 23917 |  1790 |   878 |
| STM32F0405 (ARMv7m)      | 25472 |  1909 |   869 |

## Run KATs:

First, compile code for the desired architecture. Then execute: `./genkat`

### x64

* **ISAP-A-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128av20/opt_64 crypto_aead/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`
* **ISAP-A-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128v20/opt_64 crypto_aead/isapa128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`
* **ISAP-K-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128av20/opt_64 crypto_aead/isapk128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`
* **ISAP-K-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128v20/opt_64 crypto_aead/isapk128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`
    
### ARMv6/ARMv7

* **ISAP-A-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128av20/opt_32_armv67m crypto_aead/isapa128av20/opt_32_armv67m/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`
* **ISAP-A-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128v20/opt_32_armv67m crypto_aead/isapa128v20/opt_32_armv67m/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`
* **ISAP-K-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128av20/opt_32_armv67m crypto_aead/isapk128av20/opt_32_armv67m/*.c crypto_aead/isapk128av20/opt_32_armv67m/KeccakP-400-armv6m-le-gcc.s -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`
* **ISAP-K-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128v20/opt_32_armv67m crypto_aead/isapk128v20/opt_32_armv67m/*.c crypto_aead/isapk128v20/opt_32_armv67m/KeccakP-400-armv6m-le-gcc.s -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat`

## Run Benchmarks:

First, compile code for the desired architecture. Then execute: `./getcycles`

### x64

* **ISAP-A-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128av20/opt_64 crypto_aead/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`
* **ISAP-A-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128v20/opt_64 crypto_aead/isapa128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`
* **ISAP-K-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128av20/opt_64 crypto_aead/isapk128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`
* **ISAP-K-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128v20/opt_64 crypto_aead/isapk128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`
    
### ARMv6/ARMv7

* **ISAP-A-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128av20/opt_32_armv67m crypto_aead/isapa128av20/opt_32_armv67m/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`
* **ISAP-A-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128v20/opt_32_armv67m crypto_aead/isapa128v20/opt_32_armv67m/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`
* **ISAP-K-128a**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128av20/opt_32_armv67m crypto_aead/isapk128av20/opt_32_armv67m/*.c crypto_aead/isapk128av20/opt_32_armv67m/KeccakP-400-armv6m-le-gcc.s -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`
* **ISAP-K-128**
    `gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128v20/opt_32_armv67m crypto_aead/isapk128v20/opt_32_armv67m/*.c crypto_aead/isapk128v20/opt_32_armv67m/KeccakP-400-armv6m-le-gcc.s -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles`

## License Information:

See LICENSE.txt

