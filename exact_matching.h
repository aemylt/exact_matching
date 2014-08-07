#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"

#include <gmp.h>

int fingerprint_match_allcrosses(char* T, int n, char* P, int m, int alpha, int* results) {
    int lm = 0, i, j;
    while ((1 << lm) <= m) lm++;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint tmp = init_fingerprint(), *P_i = malloc(lm * sizeof(fingerprint));
    P_i[0] = init_fingerprint();
    set_fingerprint(printer, P, 1, P_i[0]);
    gmp_printf("%Zd, ", P_i[0]->finger);
    for (i = 1; i < lm - 1; i++) {
        P_i[i] = init_fingerprint();
        j = 1 << (i - 1);
        set_fingerprint(printer, &P[j], j, tmp);
        fingerprint_concat(printer->p, P_i[i - 1], tmp, P_i[i]);
        gmp_printf("%Zd, ", P_i[i]->finger);
    }
    P_i[lm - 1] = init_fingerprint();
    j = 1 << (lm - 2);
    set_fingerprint(printer, &P[j], m - j, tmp);
    fingerprint_concat(printer->p, P_i[lm - 2], tmp, P_i[lm - 1]);
    gmp_printf("%Zd\n", P_i[lm - 1]->finger);
    return 0;
}

int fingerprint_match_naive(char* T, int n, char* P, int m, int alpha, int* results) {
    int count = 0, i;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint T_f = init_fingerprint(), P_f = init_fingerprint(), T_i = init_fingerprint(), T_m = init_fingerprint(), tmp = init_fingerprint();
    int size = n - m;
    set_fingerprint(printer, P, m, P_f);
    set_fingerprint(printer, T, m, T_f);
    if (fingerprint_equals(T_f, P_f)) results[count++] = 0;
    for (i = 0; i < size; i++) {
        set_fingerprint(printer, &T[i], 1, T_i);
        set_fingerprint(printer, &T[i + m], 1, T_m);
        fingerprint_suffix(printer->p, T_f, T_i, tmp);
        fingerprint_concat(printer->p, tmp, T_m, T_f);
        if (fingerprint_equals(T_f, P_f)) results[count++] = i + 1;
    }
    results = realloc(results, count * sizeof(int));
    fingerprinter_free(printer);
    fingerprint_free(T_f);
    fingerprint_free(P_f);
    fingerprint_free(T_i);
    fingerprint_free(T_m);
    fingerprint_free(tmp);
    return count;
}

#endif
