ISAP Code Package
=================

ISAP is a family of lightweight authenticated encryption algorithms designed with a focus on **robustness against implementation attacks** and is currently competing in the final round of the NIST Lightweight Cryptography competition ([spec](https://csrc.nist.gov/Projects/lightweight-cryptography/finalists),[web](https://isap.iaik.tugraz.at)). It is of particular interest for applications like firmware updates where **robustness against power analysis and fault attacks** is crucial and code size and a **small footprint in hardware** matters. ISAP's original version was published at [FSE 2017](https://tosc.iacr.org/index.php/ToSC/article/view/585).

This repository contains implementations of the following algorithms:

- `crypto_aead/isapa128av20`: ISAP-A-128a (primary recommendation)
- `crypto_aead/isapa128v20`: ISAP-A-128
- `crypto_aead/isapk128av20`: ISAP-K-128a
- `crypto_aead/isapk128v20`: ISAP-K-128
- `crypto_aead_hash/isapa128av20`: ISAP-A-128a + Ascon-Hash

and the following implementations:

- `avx512`: AVX-512 implementation in C.
- `bi_32`: Bit-interleaved 32-bit implementation in C.
    - These implementations should perform well on 32-bit processors that feature a rotation instruction such as ARM Cortex-M3/4.
- `bi_32_compact`: More compact variant of `bi_32`.
- `bi_32_armv6`: Bit-interleaved 32-bit implementation using ARMv6 assembly.
- `bi_32_armv6_compact`: More compact variant of `bi_32_armv6`.
- `bi_32_stp`: Variant of `bi_32_compact` that includes a leakage-resilient tag comparison.
- `opt_64`: Optimized 64-bit implementation in C.
- `opt_64_compact`: More compact variant of `opt_64`.
  - On microprocessors without rotation instruction, or microprocessors with word sizes smaller than 32 bits, this variant might perform better than the `bi_32*` implementations.
- `opt_64_stp`: Variant of `opt_64_compact` that includes a leakage-resilient tag comparison.
- `ref`: Easy-to-read implementation in C.

Performance on different platforms (cycles/byte):
-------------------------------------------------

**ISAP-A-128a (primary recommendation)**

| Message Length in Bytes: |    64 |  1536 |  long |
|:-------------------------|------:|------:|------:|
| AMD  EPYC 7742 (x64)     |     - |     - |  18.9 |
| AMD Ryzen 7 1700 (x64)   |  85.7 |  24.5 |  21.9 |
| Intel i5-6200U (x64)     |   104 |  34.3 |  31.4 |
| Raspberry Pi 1B (ARMv6)  |   551 |   171 |   159 |
| STM32F303 (ARMv7m)       |   542 |   168 |   150 |

For up-to-date benchmark results click [here](https://isap.iaik.tugraz.at/implementations.html)

Run KATs:
---------

1. Compile code for the desired architecture.
2. Execute: `./genkat`

**x64: ISAP-A-128a**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/opt_64 crypto_aead_hash/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat
```

**x64: ISAP-A-128a + Ascon-Hash**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/opt_64 crypto_aead_hash/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -DCRYPTO_HASH -Itests tests/genkat_hash.c -o genkat
```

**ARMv6: ISAP-A-128a**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/bi_32_armv6 crypto_aead_hash/isapa128av20/bi_32_armv6/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat
```

**ARMv6: ISAP-A-128a + Ascon-Hash**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/bi_32_armv6 crypto_aead_hash/isapa128av20/bi_32_armv6/*.c -DCRYPTO_AEAD -DCRYPTO_HASH -Itests tests/genkat_hash.c -o genkat
```

Run Benchmarks:
---------------

1. Compile code for the desired architecture.
2. Execute: `./getcycles`

**x64: ISAP-A-128a**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/opt_64 crypto_aead_hash/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles
```

**x64: ISAP-A-128a + Ascon-Hash**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/opt_64 crypto_aead_hash/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -DCRYPTO_HASH -Itests tests/getcycles.c -o getcycles
```

**ARMv6: ISAP-A-128a**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/bi_32_armv6 crypto_aead_hash/isapa128av20/bi_32_armv6/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles
```

**ARMv6: ISAP-A-128a + Ascon-Hash**

```
gcc -march=native -O3 -DNDEBUG -Icrypto_aead_hash/isapa128av20/bi_32_armv6 crypto_aead_hash/isapa128av20/bi_32_armv6/*.c -DCRYPTO_AEAD -DCRYPTO_HASH -Itests tests/getcycles.c -o getcycles
```

License Information:
--------------------

See LICENSE.txt
