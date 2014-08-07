#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"

int fingerprint_match(char* T, int n, char* P, int m, int alpha, int* results) {
    int count = 0, i;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint T_f, P_f, T_i, T_m;
    P_f = get_fingerprint(printer, P, m);
    int size = n - m;
    T_f = get_fingerprint(printer, T, m);
    if (fingerprint_equals(T_f, P_f)) results[count++] = 0;
    for (i = 0; i < size; i++) {
        T_i = get_fingerprint(printer, &T[i], 1);
        T_m = get_fingerprint(printer, &T[i + m], 1);
        T_f = fingerprint_suffix(printer->p, T_f, T_i);
        T_f = fingerprint_concat(printer->p, T_f, T_m);
        if (fingerprint_equals(T_f, P_f)) results[count++] = i + 1;
    }
    results = realloc(results, count * sizeof(int));
    return count;
}

#endif
