#ifndef KARP_RABIN
#define KARP_RABIN

#include <gmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
    inline int mpz_equals(mpz_t x, mpz_t y)
    Used for seeing if two numbers are equal.
    Parameters:
        mpz_t x - First number
        mpz_t y - Second number
    Returns int:
        1 if x == y
        0 otherwise
*/
inline int mpz_equals(mpz_t x, mpz_t y) {
    return ((x->_mp_size == y->_mp_size) && mpn_cmp(x->_mp_d, y->_mp_d, x->_mp_size) == 0);
}

/*
    inline int compare(mpz_t x, mpz_t y)
    Used for comparing two numbers.
    Parameters:
        mpz_t x - First number
        mpz_t y - Second number
    Returns int:
        1  if x >  y
        0  if x == y
        -1 if x <  y
*/
inline int compare(mpz_t x, mpz_t y) {
    if (x->_mp_size > y->_mp_size) return 1;
    else if (y->_mp_size > x->_mp_size) return -1;
    else return mpn_cmp(x->_mp_d, y->_mp_d, x->_mp_size);
}

/*
    typedef struct fingerprinter_t *fingerprinter
    Structure to hold fingerprint maker.
    Components:
        mpz_t p - Prime number
        mpz_t r - Random number
*/
typedef struct fingerprinter_t {
    mpz_t p;
    mpz_t r;
} *fingerprinter;

/*
    fingerprinter fingerprinter_build(unsigned int n, unsigned int alpha)
    Builds a fingerprint maker.
    Parameters:
        unsigned int n     - Length of text
        unsigned int alpha - Precision argument
    Returns fingerprinter:
        A fingerprinter for a text of this length
*/
fingerprinter fingerprinter_build(unsigned int n, unsigned int alpha) {
    fingerprinter printer = malloc(sizeof(struct fingerprinter_t));

    mpz_init_set_ui(printer->p, n);
    mpz_pow_ui(printer->p, printer->p, 2 + alpha);
    mpz_nextprime(printer->p, printer->p);

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

    mpz_init(printer->r);
    mpz_urandomm(printer->r, state, printer->p);

    return printer;
}

/*
    void fingerprinter_free(fingerprinter printer)
    Frees a fingerprint maker.
    Parameters:
        fingerprinter printer - The fingerprinter to unallocate
*/
void fingerprinter_free(fingerprinter printer) {
    mpz_clear(printer->p);
    mpz_clear(printer->r);
    free(printer);
}

/*
    typedef struct fingerprint_t *fingerprint
    Structure for fingerprints.
    Components:
        mpz_t finger - The fingerprint
        mpz_t r_k    - r^k where r is the random number and k is the text length
        mpz_t r_mk   - r^-k
*/
typedef struct fingerprint_t {
    mpz_t finger;
    mpz_t r_k;
    mpz_t r_mk;
} *fingerprint;

/*
    fingerprint init_fingerprint()
    Allocates memory for an empty fingerprint.
    Returns fingerprint:
        The new fingerprint
*/
fingerprint init_fingerprint() {
    fingerprint finger = malloc(sizeof(struct fingerprint_t));
    mpz_init(finger->finger);
    mpz_init_set_ui(finger->r_k, 1);
    mpz_init_set_ui(finger->r_mk, 1);
    return finger;
}

/*
    void set_fingerprint(fingerprinter printer, char* T, unsigned int l, fingerprint print)
    Sets a fingerprint for some text.
    Parameters:
        fingerprinter printer - The printer to use
        char*         T       - The text
        unsigned int  l       - Text length
        fingerprint   print   - The fingerprint to return
    Returns void:
        Value returned in print parameter
*/
void set_fingerprint(fingerprinter printer, char* T, unsigned int l, fingerprint print) {
    mpz_set_ui(print->r_k, 1);
    int i;

    mpz_set_ui(print->finger, T[0]);

    for (i = 1; i < l; i++) {
        mpz_mul(print->r_k, print->r_k, printer->r);
        mpz_addmul_ui(print->finger, print->r_k, T[i]);
        mpz_mod(print->finger, print->finger, printer->p);
    }
    mpz_mul(print->r_k, print->r_k, printer->r);
    mpz_mod(print->r_k, print->r_k, printer->p);

    mpz_invert(print->r_mk, print->r_k, printer->p);
}

/*
    void fingerprint_assign(fingerprint from, fingerprint to)
    Assigns a fingerprint from another fingerprint.
    Parameters:
        fingerprint from - The original fingerprint
        fingerprint to   - The new fingerprint
    Returns void:
        Value returned in to parameter
*/
void fingerprint_assign(fingerprint from, fingerprint to) {
    mpz_set(to->finger, from->finger);
    mpz_set(to->r_k, from->r_k);
    mpz_set(to->r_mk, from->r_mk);
}

/*
    void fingerprint_suffix(mpz_t p, fingerprint uv, fingerprint u, fingerprint v)
    Removes the prefix from a fingerprint.
    Parameters:
        mpz_t       p  - Prime number
        fingerprint uv - Total fingerprint
        fingerprint u  - Prefix fingerprint
        fingerprint v  - Suffix fingerprint
    Returns void:
        Value returned in v parameter
*/
void fingerprint_suffix(mpz_t p, fingerprint uv, fingerprint u, fingerprint v) {
    mpz_mul(v->r_k, uv->r_k, u->r_mk);
    mpz_mod(v->r_k, v->r_k, p);
    mpz_invert(v->r_mk, v->r_k, p);

    mpz_sub(v->finger, uv->finger, u->finger);
    if (mpz_cmp_si(v->finger, 0) < 0) mpz_add(v->finger, v->finger, p);
    mpz_mul(v->finger, v->finger, u->r_mk);
    mpz_mod(v->finger, v->finger, p);
}

/*
    void fingerprint_prefix(mpz_t p, fingerprint uv, fingerprint v, fingerprint u)
    Removes the suffix from a fingerprint.
    Parameters:
        mpz_t       p  - Prime number
        fingerprint uv - Total fingerprint
        fingerprint v  - Suffix fingerprint
        fingerprint u  - Prefix fingerprint
    Returns void:
        Value returned in u parameter
*/
void fingerprint_prefix(mpz_t p, fingerprint uv, fingerprint v, fingerprint u) {
    mpz_mul(u->r_k, uv->r_k, v->r_mk);
    mpz_mod(u->r_k, u->r_k, p);
    mpz_invert(u->r_mk, u->r_k, p);

    mpz_mul(u->finger, v->finger, u->r_k);
    mpz_mod(u->finger, u->finger, p);
    mpz_sub(u->finger, uv->finger, u->finger);
    if (mpz_cmp_si(u->finger, 0) < 0) mpz_add(u->finger, u->finger, p);
}

/*
    void fingerprint_concat(mpz_t p, fingerprint u, fingerprint v, fingerprint uv)
    Concatenates two fingerprints.
    Parameters:
        mpz_t       p  - Prime number
        fingerprint u  - Prefix fingerprint
        fingerprint v  - Suffix fingerprint
        fingerprint uv - Total fingerprint
    Returns void:
        Value returned in uv parameter
*/
void fingerprint_concat(mpz_t p, fingerprint u, fingerprint v, fingerprint uv) {
    mpz_mul(uv->r_k, u->r_k, v->r_k);
    mpz_mod(uv->r_k, uv->r_k, p);
    mpz_invert(uv->r_mk, uv->r_k, p);

    mpz_mul(uv->finger, v->finger, u->r_k);
    mpz_mod(uv->finger, uv->finger, p);
    mpz_add(uv->finger, u->finger, uv->finger);
    if (compare(uv->finger, p) > 0) mpz_sub(uv->finger, uv->finger, p);
}

/*
    int fingerprint_equals(fingerprint T_f, fingerprint P_f)
    Checks if two fingerprints are equal.
    Parameters:
        fingerprint T_f - First fingerprint
        fingerprint P_f - Second fingerprint
    Returns int:
        1 if T_f == P_f
        0 otherwise
*/
int fingerprint_equals(fingerprint T_f, fingerprint P_f) {
    return (mpz_equals(T_f->r_k, P_f->r_k) && mpz_equals(T_f->r_mk, P_f->r_mk) && mpz_equals(T_f->finger, P_f->finger));
}

/*
    void fingerprint_free(fingerprint finger)
    Frees a fingerprint.
    Parameters:
        fingerprint finger - The fingerprint to deallocate
*/
void fingerprint_free(fingerprint finger) {
    mpz_clear(finger->finger);
    mpz_clear(finger->r_k);
    mpz_clear(finger->r_mk);
    free(finger);
}

#endif
