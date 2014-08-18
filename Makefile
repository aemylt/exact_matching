CC=gcc
CARGS=-Wall -O3
GMPLIB=-L/gmp_install/lib -lgmp

all:
	$(CC) $(CARGS) exact_matching.c -o exact_matching $(GMPLIB)

clean:
	rm exact_matching

karp-rabin:
	$(CC) $(CARGS) karp_rabin.c -o karp_rabin $(GMPLIB)

karp-rabin-clean:
	rm karp_rabin