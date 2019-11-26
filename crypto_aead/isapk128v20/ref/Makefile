all:
	gcc *.c -march=native -O3 -DNDEBUG  -o genkat_aead

debug:
	gcc *.c -std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -O2 -o genkat_aead

clean:
	rm -rf genkat_aead LWC_AEAD_KAT_128_128.txt
