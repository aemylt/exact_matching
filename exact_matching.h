#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"

int fingerprint_match(char* T, int n, char* P, int m, int alpha, int* results) {
    int count = 0, i;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint T_f, P_f, T_i, T_m, tmp;
    P_f = init_fingerprint();
    set_fingerprint(printer, P, m, P_f);
    int size = n - m;
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
    return count;
}

#endif
