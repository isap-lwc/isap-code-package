all:
	gcc *.c -std=c99 -Wall -Wextra -Wshadow -O3 -march=native -o genkat_aead

debug:
	gcc *.c -std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -O2 -o genkat_aead

clean:
	rm -rf genkat_aead LWC_AEAD_KAT_128_128.txt
