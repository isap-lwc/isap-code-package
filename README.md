# ISAP Code Package

ISAP is a family of lightweight authenticated encryption algorithms designed with a focus on **robustness against implementation attacks** and is currently competing in the 2nd round of the [NIST Lightweight Cryptography competition](https://csrc.nist.gov/Projects/Lightweight-Cryptography/Round-2-Candidates) (2019). It is of particular interest for applications like firmeware updates where **robustness against power analysis and fault attacks** is crucial and codesize and a **small footprint in hardware** matters. ISAP's original version was published at [FSE 2017](https://tosc.iacr.org/index.php/ToSC/article/view/585).

This repository contains the following ISAP algorithms:

- `crypto_aead/isapa128av20`: ISAP-A-128a v2.0
- `crypto_aead/isapa128v20`: ISAP-A-128 v2.0
- `crypto_aead/isapk128av20`: ISAP-K-128a v2.0
- `crypto_aead/isapk128v20`: ISAP-K-128 v2.0

and the following implementations:

- `ref`: reference implementation
- `opt_64`: 64-bit optimized C implementation

## Performance results on different CPUs in cycles per byte:

ISAP was especially designed for senarios where implementation security is required. It features, amongst others, **build-in hardening/protection against implementation attacks** such as Differential Power Analysis (DPA), Differential Fault Attacks (DFA), Statistical Fault Attacks (SFA), and Statistical Ineffetive Fault Attacks (SIFA).

### ISAP-A-128a (recommended)

| Message Length in Bytes:   |   64 | 1536 | long |
|:---------------------------|-----:|-----:|-----:|
| AMD Rzyen 7 1700 @ 3.40GHz | 85.7 | 24.5 | 21.9 |
| Intel i5-6200U @ 2.30GHz   |  104 | 34.3 | 31.4 |

### ISAP-A-128

| Message Length in Bytes:   |   64 | 1536 | long |
|:---------------------------|-----:|-----:|-----:|
| AMD Rzyen 7 1700 @ 3.40GHz |  511 | 48.9 | 29.8 |
| Intel i5-6200U @ 2.30GHz   |  698 | 68.1 | 42.0 |

### ISAP-K-128a (recommended)

| Message Length in Bytes:   |   64 | 1536 | long |
|:---------------------------|-----:|-----:|-----:|
| AMD Rzyen 7 1700 @ 3.40GHz |  295 | 64.1 | 54.3 |
| Intel i5-6200U @ 2.30GHz   |  342 | 72.8 | 61.3 |

### ISAP-K-128

| Message Length in Bytes:   |   64 | 1536 | long |
|:---------------------------|-----:|-----:|-----:|
| AMD Rzyen 7 1700 @ 3.40GHz | 2108 |  156 | 75.0 |
| Intel i5-6200U @ 2.30GHz   | 2318 |  173 | 84.0 |

## Run KATs:

* **ISAP-A-128a**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128av20/opt_64 crypto_aead/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat
* **ISAP-A-128**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128v20/opt_64 crypto_aead/isapa128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat
* **ISAP-K-128a**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128av20/opt_64 crypto_aead/isapk128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat
* **ISAP-K-128**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128v20/opt_64 crypto_aead/isapk128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat

Then execute:
    ./genkat

## Run Benchmarks:

* **ISAP-A-128a**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128av20/opt_64 crypto_aead/isapa128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles
* **ISAP-A-128**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapa128v20/opt_64 crypto_aead/isapa128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles
* **ISAP-K-128a**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128av20/opt_64 crypto_aead/isapk128av20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles
* **ISAP-K-128**
    gcc -march=native -O3 -DNDEBUG -Icrypto_aead/isapk128v20/opt_64 crypto_aead/isapk128v20/opt_64/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles

Then execute:
    ./getcycles

## License Information:

See LICENSE.txt

