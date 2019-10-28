 #!/usr/bin/bash

pass=1

cd isapa128av20

cd ref
make all
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ..

cd opt_64
rm -rf build
mkdir build
cd build
cmake ..
make
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt ../LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ../..

cd ..
cd isapa128v20

cd ref
make all
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ..

cd opt_64
rm -rf build
mkdir build
cd build
cmake ..
make
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt ../LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ../..

cd ..
cd isapk128av20

cd ref
make all
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ..

cd opt_64
rm -rf build
mkdir build
cd build
cmake ..
make
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt ../LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ../..

cd ..
cd isapk128v20

cd ref
make all
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ..

cd opt_64
rm -rf build
mkdir build
cd build
cmake ..
make
./genkat_aead
diff LWC_AEAD_KAT_128_128.txt ../LWC_AEAD_KAT_128_128_REF.txt
if (($?!=0)); then pass=0; fi
cd ../..

echo "Should be 1:"
echo $pass
