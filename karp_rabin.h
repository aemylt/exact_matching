#ifndef KARP_RABIN
#define KARP_RABIN

#include <gmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct fingerprinter_t {
    mpz_t p;
    mpz_t r;
} *fingerprinter;

fingerprinter fingerprinter_build(unsigned int n) {
    mpz_t p, num, num_sq, r;

    mpz_inits(p, num_sq);
    mpz_init_set_ui(num, n);
    mpz_mul_ui(num_sq, num, n);
    mpz_nextprime(p, num_sq);

    gmp_randstate_t state;
    unsigned long seed;
    size_t seed_len = 0;
    int f = open("/dev/urandom", O_RDONLY);
    while (seed_len < sizeof seed) {
        size_t result = read(f, ((char*)&seed) + seed_len, (sizeof seed) - seed_len);
        seed_len += result;
    }
    close(f);
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);

    mpz_init(r);
    mpz_urandomm(r, state, p);

    fingerprinter printer = malloc(sizeof(struct fingerprinter_t));
    mpz_init_set(printer->p, p);
    mpz_init_set(printer->r, r);

    return printer;
}

#endif
