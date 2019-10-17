### Configuring Code:

* Enable/disable debug mode -> see CMakeLists.txt
* Choose ISAP instance -> see CMakeLists.txt

### Building Code:

    mkdir build
    cd build
    cmake ..
    make

### Executing Code:

    ./genkat_aead

then:

    diff LWC_AEAD_KAT_128_128.txt ../kat/isapk128/LWC_AEAD_KAT_128_128.txt

or:

    diff LWC_AEAD_KAT_128_128.txt ../kat/isapk128a/LWC_AEAD_KAT_128_128.txt

or:

    diff LWC_AEAD_KAT_128_128.txt ../kat/isapa128/LWC_AEAD_KAT_128_128.txt

or:

    diff LWC_AEAD_KAT_128_128.txt ../kat/isapa128a/LWC_AEAD_KAT_128_128.txt

### Implementation By:

* [Robert Primas](https://rprimas.github.io)
