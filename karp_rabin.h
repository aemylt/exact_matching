#ifndef KARP_RABIN
#define KARP_RABIN

#include <gmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

inline int mpz_equals(mpz_t x, mpz_t y) {
    return ((x->_mp_size == y->_mp_size) && mpn_cmp(x->_mp_d, y->_mp_d, x->_mp_size) == 0);
}

inline int compare(mpz_t x, mpz_t y) {
    if (x->_mp_size > y->_mp_size) return 1;
    else if (y->_mp_size > x->_mp_size) return -1;
    else return mpn_cmp(x->_mp_d, y->_mp_d, x->_mp_size);
}

typedef struct fingerprinter_t {
    mpz_t p;
    mpz_t r;
} *fingerprinter;

fingerprinter fingerprinter_build(unsigned int n, unsigned int alpha) {
    mpz_t p, num, r;

    mpz_init(p);
    mpz_init_set_ui(num, n);
    mpz_pow_ui(num, num, 2 + alpha);
    mpz_nextprime(p, num);

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

typedef struct fingerprint_t {
    mpz_t finger;
    unsigned int k;
    mpz_t r_k;
    mpz_t r_mk;
} *fingerprint;

fingerprint get_fingerprint(fingerprinter printer, char* T, unsigned int l) {
    mpz_t finger, r_i, r_mk;
    mpz_init_set_ui(r_i, 1);
    int i;

    mpz_init_set_ui(finger, T[0]);

    for (i = 1; i < l; i++) {
        mpz_mul(r_i, r_i, printer->r);
        mpz_addmul_ui(finger, r_i, T[i]);
        mpz_mod(finger, finger, printer->p);
    }
    mpz_mul(r_i, r_i, printer->r);
    mpz_mod(r_i, r_i, printer->p);

    mpz_init(r_mk);
    mpz_invert(r_mk, r_i, printer->p);

    fingerprint print = malloc(sizeof(struct fingerprint_t));
    print->k = l;
    mpz_init_set(print->finger, finger);
    mpz_init_set(print->r_k, r_i);
    mpz_init_set(print->r_mk, r_mk);
    return print;
}

fingerprint fingerprint_suffix(mpz_t p, fingerprint uv, fingerprint u) {
    fingerprint v = malloc(sizeof(struct fingerprint_t));
    v->k = uv->k - u->k;
    mpz_init(v->finger);
    mpz_init(v->r_k);
    mpz_init(v->r_mk);

    mpz_mul(v->r_k, uv->r_k, u->r_mk);
    mpz_mod(v->r_k, v->r_k, p);
    mpz_invert(v->r_mk, v->r_k, p);

    mpz_sub(v->finger, uv->finger, u->finger);
    if (mpz_cmp_si(v->finger, 0) < 0) mpz_add(v->finger, v->finger, p);
    mpz_mul(v->finger, v->finger, u->r_mk);
    mpz_mod(v->finger, v->finger, p);
    return v;
}

fingerprint fingerprint_prefix(mpz_t p, fingerprint uv, fingerprint v) {
    fingerprint u = malloc(sizeof(struct fingerprint_t));
    u->k = uv->k - v->k;
    mpz_init(u->finger);
    mpz_init(u->r_k);
    mpz_init(u->r_mk);

    mpz_mul(u->r_k, uv->r_k, v->r_mk);
    mpz_mod(u->r_k, u->r_k, p);
    mpz_invert(u->r_mk, u->r_k, p);

    mpz_mul(u->finger, v->finger, u->r_k);
    mpz_mod(u->finger, u->finger, p);
    mpz_sub(u->finger, uv->finger, u->finger);
    if (mpz_cmp_si(u->finger, 0) < 0) mpz_add(u->finger, u->finger, p);
    return u;
}

fingerprint fingerprint_concat(mpz_t p, fingerprint u, fingerprint v) {
    fingerprint uv = malloc(sizeof(struct fingerprint_t));
    uv->k = u->k + v->k;
    mpz_init(uv->finger);
    mpz_init(uv->r_k);
    mpz_init(uv->r_mk);

    mpz_mul(uv->r_k, u->r_k, v->r_k);
    mpz_mod(uv->r_k, uv->r_k, p);
    mpz_invert(uv->r_mk, uv->r_k, p);

    mpz_mul(uv->finger, v->finger, u->r_k);
    mpz_mod(uv->finger, uv->finger, p);
    mpz_add(uv->finger, u->finger, uv->finger);
    if (compare(uv->finger, p) > 0) mpz_sub(uv->finger, uv->finger, p);
    return uv;
}

int fingerprint_equals(fingerprint T_f, fingerprint P_f) {
    return ((T_f->k == P_f->k) && mpz_equals(T_f->r_k, P_f->r_k) && mpz_equals(T_f->r_mk, P_f->r_mk) && mpz_equals(T_f->finger, P_f->finger));
}

int fingerprint_match(char* T, int n, char* P, int m, int alpha, int* results) {
    int count = 0, i;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint T_f, P_f;
    P_f = get_fingerprint(printer, P, m);
    int size = n - m + 1;
    for (i = 0; i < size; i++) {
        T_f = get_fingerprint(printer, &T[i], m);
        if (fingerprint_equals(T_f, P_f)) results[count++] = i;
    }
    results = realloc(results, count * sizeof(int));
    return count;
}

#endif
