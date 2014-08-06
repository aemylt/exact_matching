karp-rabin:
	gcc -Wall -O3 karp_rabin.c -o karp_rabin -L/gmp_install/lib -lgmp

karp-rabin-clean:
	rm karp_rabin