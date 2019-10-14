all:
	gcc *.c -std=c99 -Wall -Wextra -Wshadow -O3 -march=native -o genkat

debug:
	gcc *.c -std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -O2 -o genkat

clean:
	rm -rf genkat
