#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

cd ..

echo "#################"
echo "#      AEAD     #"
echo "#################"
declare -a arr=(
"crypto_aead/isapa128av20/opt_64"
"crypto_aead/isapa128av20/opt_32"
"crypto_aead/isapa128av20/opt_32_compact"
"crypto_aead/isapa128av20/opt_32_stp"
"crypto_aead/isapa128av20/opt_32_tiny"
"crypto_aead/isapa128av20/bi_32"
"crypto_aead/isapa128av20/bi_32_compact"
"crypto_aead/isapa128av20/bi_32_stp"
#"crypto_aead/isapa128av20/bi_32_armv6"          # works only on arm devices
#"crypto_aead/isapa128av20/bi_32_armv6m"         # works only on arm devices
#"crypto_aead/isapa128av20/bi_32_armv6m_compact" # works only on arm devices
#"crypto_aead/isapa128av20/bi_32_armv6m_stp"     # works only on arm devices
#"crypto_aead/isapa128av20/bi_32_armv7m"         # works only on arm devices
"crypto_aead/isapa128av20/bi_8"
"crypto_aead/isapa128av20/opt_8"
"crypto_aead/isapa128av20/opt_8_tiny"
"crypto_aead/isapa128av20/ref"
)
for i in "${arr[@]}"
do
	echo "$i"
	gcc -march=native -O3 -DNDEBUG -I$i $i/*.c -DCRYPTO_AEAD -Itests tests/bulk.c -o bulk
	res=$( { time ./bulk; } 2>&1 )
	echo $res | rev | cut -d " " -f 3 | rev
done
