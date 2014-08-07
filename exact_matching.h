#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"

#include <gmp.h>
#include <memory.h>
#include <stdlib.h>

typedef struct {
    int location;
    fingerprint T_f;
} viable_occurance;

typedef struct {
    int row_size;
    int end;
    fingerprint P;
    viable_occurance *VOs;
} pattern_row;

int fingerprint_match_allcrosses(char* T, int n, char* P, int m, int alpha, int* results) {
    int lm = 0, i, j, matches = 0;
    while ((1 << lm) <= m) lm++;
    lm++;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint tmp = init_fingerprint(), tmp2 = init_fingerprint(), T_f = init_fingerprint();
    pattern_row *P_i = malloc(lm * sizeof(pattern_row));
    P_i[0].P = init_fingerprint();
    set_fingerprint(printer, P, 1, P_i[0].P);
    P_i[0].row_size = 1;
    P_i[0].end = 0;
    P_i[0].VOs = malloc(2 * sizeof(viable_occurance));
    P_i[0].VOs[0].T_f = init_fingerprint();
    P_i[0].VOs[0].location = 0;
    P_i[0].VOs[1].T_f = init_fingerprint();
    P_i[0].VOs[1].location = 0;
    for (i = 1; i < lm - 1; i++) {
        P_i[i].P = init_fingerprint();
        j = 1 << (i - 1);
        set_fingerprint(printer, &P[j], j, tmp);
        fingerprint_concat(printer->p, P_i[i - 1].P, tmp, P_i[i].P);
        P_i[i].row_size = j << 1;
        P_i[i].end = 0;
        P_i[i].VOs = malloc(P_i[i].row_size * sizeof(viable_occurance));
        for (j = 0; j < P_i[i].row_size; j++) {
            P_i[i].VOs[j].T_f = init_fingerprint();
            P_i[i].VOs[j].location = 0;
        }
    }
    P_i[lm - 1].P = init_fingerprint();
    j = 1 << (lm - 2);
    set_fingerprint(printer, &P[j], m - j, tmp);
    fingerprint_concat(printer->p, P_i[lm - 2].P, tmp, P_i[lm - 1].P);
    P_i[lm - 1].row_size = 0;

    for (i = 0; i < n; i++) {
        j = lm - 2;
        if ((P_i[j].end > 0) && (i - P_i[j].VOs[0].location == m - P_i[j].row_size)) {
            set_fingerprint(printer, &T[i - m + 1], P_i[j].row_size, tmp);
            set_fingerprint(printer, &T[i - m + P_i[j].row_size + 1], m - P_i[j].row_size, tmp2);
            fingerprint_concat(printer->p, tmp, tmp2, T_f);

            if (fingerprint_equals (P_i[j + 1].P, T_f)) results[matches++] = i + 1;
            memmove(P_i[j].VOs, &P_i[j].VOs[1], ((P_i[j].row_size << 1) - 1) * sizeof(pattern_row));
            P_i[j].end--;
        }

        for (j = lm - 3; j >= 0; j--) {
            if ((P_i[j].end > 0) && (i - P_i[j].VOs[0].location == P_i[j].row_size)) {
                set_fingerprint(printer, &T[i - (P_i[j].row_size << 1) + 1], P_i[j].row_size, tmp);
                set_fingerprint(printer, &T[i - P_i[j].row_size + 1], P_i[j].row_size, tmp2);
                fingerprint_concat(printer->p, tmp, tmp2, T_f);
                if (fingerprint_equals (P_i[j + 1].P, T_f)) {
                    P_i[j + 1].VOs[P_i[j + 1].end].T_f = T_f;
                    P_i[j + 1].VOs[P_i[j + 1].end].location = i;
                    P_i[j + 1].end++;
                }
                memmove(P_i[j].VOs, &P_i[j].VOs[1], ((P_i[j].row_size << 1) - 1) * sizeof(pattern_row));
                P_i[j].end--;
            }
        }
        set_fingerprint(printer, &T[i], 1, T_f);
        if (fingerprint_equals (P_i[0].P, T_f)) {
            P_i[0].VOs[P_i[0].end].T_f = T_f;
            P_i[0].VOs[P_i[0].end].location = i;
            P_i[0].end++;
        }
    }
    return matches;
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
