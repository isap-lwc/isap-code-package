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
"crypto_aead/isapa128av20/opt_64_compact"
"crypto_aead/isapa128av20/opt_64_stp"
"crypto_aead/isapa128av20/opt_32"
"crypto_aead/isapa128av20/opt_32_compact"
"crypto_aead/isapa128av20/opt_32_stp"
"crypto_aead/isapa128av20/bi_32"
"crypto_aead/isapa128av20/bi_32_compact"
"crypto_aead/isapa128av20/bi_32_stp"
#"crypto_aead/isapa128av20/bi_32_armv6"			    # works only on arm devices
#"crypto_aead/isapa128av20/bi_32_armv6_compact"     # works only on arm devices
"crypto_aead/isapa128av20/bi_8"
"crypto_aead/isapa128av20/opt_8"
"crypto_aead/isapa128av20/ref"
)
for i in "${arr[@]}"
do
	echo "$i"
	gcc -march=native -O3 -DNDEBUG -I$i $i/*.c -DCRYPTO_AEAD -Itests tests/genkat_aead.c -o genkat
	./genkat
	res=$(diff LWC_AEAD_KAT_128_128.txt crypto_aead_hash/isapa128av20/LWC_AEAD_KAT_128_128.txt)
	if [[ $res == "" ]]; then
	  	printf "AEAD ${GREEN}PASS!${NC}\n"
	else
		printf "AEAD ${RED}FAIL!${NC}\n"
	fi
done

echo "########################"
echo "#      AEAD + HASH     #"
echo "########################"
declare -a arr=(
"crypto_aead_hash/isapa128av20/opt_64"
"crypto_aead_hash/isapa128av20/opt_64_compact"
"crypto_aead_hash/isapa128av20/opt_64_stp"
"crypto_aead_hash/isapa128av20/opt_32"
"crypto_aead_hash/isapa128av20/opt_32_compact"
#"crypto_aead_hash/isapa128av20/opt_32_stp"
"crypto_aead_hash/isapa128av20/bi_32"
"crypto_aead_hash/isapa128av20/bi_32_compact"
"crypto_aead_hash/isapa128av20/bi_32_stp"
#"crypto_aead_hash/isapa128av20/bi_32_armv6"	        # works only on arm devices
#"crypto_aead_hash/isapa128av20/bi_32_armv6_compact"    # works only on arm devices
"crypto_aead_hash/isapa128av20/bi_8"
"crypto_aead_hash/isapa128av20/opt_8"
)
for i in "${arr[@]}"
do
	echo "$i"
	gcc -march=native -O3 -DNDEBUG -I$i $i/*.c -DCRYPTO_AEAD -DCRYPTO_HASH -Itests tests/genkat_aead.c -o genkat
	./genkat
	res=$(diff LWC_AEAD_KAT_128_128.txt crypto_aead_hash/isapa128av20/LWC_AEAD_KAT_128_128.txt)
	if [[ $res == "" ]]; then
	  	printf "AEAD ${GREEN}PASS!${NC}\n"
	else
		printf "AEAD ${RED}FAIL!${NC}\n"
	fi
	gcc -march=native -O3 -DNDEBUG -I$i $i/*.c -DCRYPTO_HASH -DCRYPTO_HASH -Itests tests/genkat_hash.c -o genkat
	./genkat
	res=$(diff LWC_HASH_KAT_256.txt crypto_aead_hash/isapa128av20/LWC_HASH_KAT_256.txt)
	if [[ $res == "" ]]; then
	  	printf "HASH ${GREEN}PASS!${NC}\n"
	else
		printf "HASH ${RED}FAIL!${NC}\n"
	fi
done

