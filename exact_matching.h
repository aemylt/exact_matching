#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"

int fingerprint_match(char* T, int n, char* P, int m, int alpha, int* results) {
    int count = 0, i;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint T_f, P_f, T_i, T_m, tmp;
    int size = n - m;
    P_f = init_fingerprint();
    set_fingerprint(printer, P, m, P_f);
    T_f = init_fingerprint();
    set_fingerprint(printer, T, m, T_f);
    T_i = init_fingerprint();
    T_m = init_fingerprint();
    tmp = init_fingerprint();
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
