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
#"crypto_aead/isapa128av20/bi_32_armv6"         # works only on arm devices
#"crypto_aead/isapa128av20/bi_32_armv6_compact" # works only on arm devices
#"crypto_aead/isapa128av20/bi_32_armv6_stp"     # works only on arm devices
"crypto_aead/isapa128av20/bi_8"
"crypto_aead/isapa128av20/opt_8"
"crypto_aead/isapa128av20/opt_8_tiny"
"crypto_aead/isapa128av20/ref"
)
for i in "${arr[@]}"
do
	echo "$i"
	gcc -march=native -O3 -DNDEBUG -I$i $i/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles
	res=$(./getcycles | tail -n1)
	echo "AEAD   64:"$(echo $res | rev | cut -d "|" -f 6 | rev)
	echo "AEAD 1536:"$(echo $res | rev | cut -d "|" -f 4 | rev)
	echo "AEAD long:"$(echo $res | rev | cut -d "|" -f 2 | rev)
done

echo "########################"
echo "#      AEAD + HASH     #"
echo "########################"
declare -a arr=(
"crypto_aead_hash/isapa128av20/opt_64"
"crypto_aead_hash/isapa128av20/opt_32"
"crypto_aead_hash/isapa128av20/opt_32_compact"
"crypto_aead_hash/isapa128av20/opt_32_stp"
"crypto_aead_hash/isapa128av20/opt_32_tiny"
"crypto_aead_hash/isapa128av20/bi_32"
"crypto_aead_hash/isapa128av20/bi_32_compact"
"crypto_aead_hash/isapa128av20/bi_32_stp"
#"crypto_aead_hash/isapa128av20/bi_32_armv6"         # works only on arm devices
#"crypto_aead_hash/isapa128av20/bi_32_armv6_compact" # works only on arm devices
#"crypto_aead_hash/isapa128av20/bi_32_armv6_stp"     # works only on arm devices
"crypto_aead_hash/isapa128av20/bi_8"
"crypto_aead_hash/isapa128av20/opt_8"
"crypto_aead_hash/isapa128av20/opt_8_tiny"
)
for i in "${arr[@]}"
do
	echo "$i"
	gcc -march=native -O3 -DNDEBUG -I$i $i/*.c -DCRYPTO_AEAD -Itests tests/getcycles.c -o getcycles
	res=$(./getcycles | tail -n1)
	echo "AEAD  64:"$(echo $res | rev | cut -d "|" -f 6 | rev)
	echo "AEAD 536:"$(echo $res | rev | cut -d "|" -f 4 | rev)
	echo "AEAD ong:"$(echo $res | rev | cut -d "|" -f 2 | rev)
	gcc -march=native -O3 -DNDEBUG -I$i $i/*.c -DCRYPTO_HASH -Itests tests/getcycles.c -o getcycles
	res=$(./getcycles | tail -n1)
	echo "HASH   64:"$(echo $res | rev | cut -d "|" -f 6 | rev)
	echo "HASH 1536:"$(echo $res | rev | cut -d "|" -f 4 | rev)
	echo "HASH long:"$(echo $res | rev | cut -d "|" -f 2 | rev)
done
