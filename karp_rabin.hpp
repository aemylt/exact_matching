#ifndef KARP_RABIN
#define KARP_RABIN

#include <gmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

using namespace std;

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
inline bool mpz_equals(mpz_t x, mpz_t y) {
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
struct fingerprinter {
        mpz_t p,r;
        fingerprinter(unsigned int n, unsigned int alpha) {
            mpz_init_set_ui(p, n);
            mpz_pow_ui(p, p, 2 + alpha);
            mpz_nextprime(p, p);

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
        }

        ~fingerprinter() {
            mpz_clear(p);
            mpz_clear(r);
        }
};

class fingerprint {
        mpz_t r_k;
        mpz_t r_mk;
        mpz_t finger;

    public:
        fingerprint() {
            mpz_init(finger);
            mpz_init_set_ui(r_k, 1);
            mpz_init_set_ui(r_mk, 1);
        }

        ~fingerprint() {
            mpz_clear(finger);
            mpz_clear(r_k);
            mpz_clear(r_mk);
        }

        void set(fingerprinter& printer, string T, unsigned int l) {
            if (l) {
                mpz_set_ui(r_k, 1);
                unsigned int i;

                mpz_set_ui(finger, T[0]);

                for (i = 1; i < l; i++) {
                    mpz_mul(r_k, r_k, printer.r);
                    mpz_addmul_ui(finger, r_k, T[i]);
                    mpz_mod(finger, finger, printer.p);
                }
                mpz_mul(r_k, r_k, printer.r);
                mpz_mod(r_k, r_k, printer.p);

                mpz_invert(r_mk, r_k, printer.p);
            }
        }

        fingerprint& operator = (fingerprint& from) {
            mpz_set(finger, from.finger);
            mpz_set(r_k, from.r_k);
            mpz_set(r_mk, from.r_mk);
            return *this;
        }

        void suffix(fingerprinter& printer, fingerprint& u, fingerprint& v) {
            mpz_mul(v.r_k, r_k, u.r_mk);
            mpz_mod(v.r_k, v.r_k, printer.p);
            mpz_invert(v.r_mk, v.r_k, printer.p);

            mpz_sub(v.finger, finger, u.finger);
            if (mpz_cmp_si(v.finger, 0) < 0) mpz_add(v.finger, v.finger, printer.p);
            mpz_mul(v.finger, v.finger, u.r_mk);
            mpz_mod(v.finger, v.finger, printer.p);
        }

        void prefix(fingerprinter& printer, fingerprint& v, fingerprint& u) {
            mpz_mul(u.r_k, r_k, v.r_mk);
            mpz_mod(u.r_k, u.r_k, printer.p);
            mpz_invert(u.r_mk, u.r_k, printer.p);

            mpz_mul(u.finger, v.finger, u.r_k);
            mpz_mod(u.finger, u.finger, printer.p);
            mpz_sub(u.finger, finger, u.finger);
            if (mpz_cmp_si(u.finger, 0) < 0) mpz_add(u.finger, u.finger, printer.p);
        }

        void concat(fingerprinter& printer, fingerprint& v, fingerprint& uv) {
            mpz_mul(uv.r_k, r_k, v.r_k);
            mpz_mod(uv.r_k, uv.r_k, printer.p);
            mpz_invert(uv.r_mk, uv.r_k, printer.p);

            mpz_mul(uv.finger, v.finger, r_k);
            mpz_mod(uv.finger, uv.finger, printer.p);
            mpz_add(uv.finger, finger, uv.finger);
            if (compare(uv.finger, printer.p) > 0) mpz_sub(uv.finger, uv.finger, printer.p);
        }

        bool operator == (fingerprint& P_f) {
            return (mpz_equals(r_k, P_f.r_k) && mpz_equals(r_mk, P_f.r_mk) && mpz_equals(finger, P_f.finger));
        }

};

#endif
