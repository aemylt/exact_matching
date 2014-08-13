CC=g++
CARGS=-Wall -O3
GMPLIB=-L/gmp_install/lib -lgmp

all:
	$(CC) $(CARGS) exact_matching.cpp -o exact_matching $(GMPLIB)

clean:
	rm exact_matching

karp-rabin:
	$(CC) $(CARGS) karp_rabin.cpp -o karp_rabin $(GMPLIB)

karp-rabin-clean:
	rm karp_rabin

kmp:
	$(CC) $(CARGS) kmp.cpp -o kmp

kmp-clean:
	rm kmp