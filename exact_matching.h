#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"

#include <stdlib.h>

typedef struct {
    int location;
    fingerprint T_f;
} viable_occurance;

typedef struct {
    int row_size, period, count;
    fingerprint P, period_f;
    viable_occurance VOs[2];
} pattern_row;

void shift_row(fingerprinter printer, pattern_row *P_i, fingerprint tmp) {
    if (P_i->count <= 2) {
        fingerprint_assign(P_i->VOs[1].T_f, P_i->VOs[0].T_f);
        P_i->VOs[0].location = P_i->VOs[1].location;
    } else {
        fingerprint_concat(printer, P_i->VOs[0].T_f, P_i->period_f, tmp);
        fingerprint_assign(tmp, P_i->VOs[0].T_f);
        P_i->VOs[0].location += P_i->period;
    }
    P_i->count--;
}

void add_occurance(fingerprinter printer, fingerprint T_f, int location, pattern_row *P_i) {
    if (P_i->count < 2) {
        fingerprint_assign(T_f, P_i->VOs[P_i->count].T_f);
        P_i->VOs[P_i->count].location = location;
    } else {
        if (P_i->count == 2) {
            P_i->period = P_i->VOs[1].location - P_i->VOs[0].location;
            fingerprint_suffix(printer, P_i->VOs[1].T_f, P_i->VOs[0].T_f, P_i->period_f);
        }
        fingerprint_assign(T_f, P_i->VOs[1].T_f);
        P_i->VOs[1].location = location;
    }
    P_i->count++;
}


int fingerprint_match_allcrosses(char* T, int n, char* P, int m, int alpha, int* results) {
    int lm = 0, i, j, matches = 0;
    while ((1 << lm) <= m) lm++;
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint T_next = init_fingerprint(), T_f = init_fingerprint(), T_prev = init_fingerprint(), T_cur = init_fingerprint(), tmp = init_fingerprint();
    pattern_row *P_i = malloc(lm * sizeof(pattern_row));
    P_i[0].row_size = 1;
    P_i[0].period = 0;
    P_i[0].count = 0;
    P_i[0].P = init_fingerprint();
    set_fingerprint(printer, P, 1, P_i[0].P);
    P_i[0].period_f = init_fingerprint();
    P_i[0].VOs[0].T_f = init_fingerprint();
    P_i[0].VOs[0].location = 0;
    P_i[0].VOs[1].T_f = init_fingerprint();
    P_i[0].VOs[1].location = 0;
    for (i = 1; i < lm - 1; i++) {
        j = 1 << (i - 1);
        P_i[i].row_size = j << 1;
        P_i[i].period = 0;
        P_i[i].count = 0;
        P_i[i].P = init_fingerprint();
        set_fingerprint(printer, &P[j], j, P_i[i].P);
        P_i[i].period_f = init_fingerprint();
        P_i[i].VOs[0].T_f = init_fingerprint();
        P_i[i].VOs[0].location = 0;
        P_i[i].VOs[1].T_f = init_fingerprint();
        P_i[i].VOs[1].location = 0;
    }
    j = 1 << (lm - 2);
    P_i[lm - 1].P = init_fingerprint();
    set_fingerprint(printer, &P[j], m - j, P_i[lm - 1].P);
    P_i[lm - 1].row_size = 0;

    for (i = 0; i < n; i++) {
        set_fingerprint(printer, &T[i], 1, T_cur);
        fingerprint_concat(printer, T_prev, T_cur, T_next);

        j = lm - 2;
        if ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location == m - P_i[j].row_size)) {
            fingerprint_suffix(printer, T_next, P_i[j].VOs[0].T_f, T_f);

            if (fingerprint_equals(P_i[j + 1].P, T_f)) results[matches++] = i + 1;
            shift_row(printer, &P_i[j], tmp);
        }

        for (j = lm - 3; j >= 0; j--) {
            if ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location == P_i[j].row_size)) {
                fingerprint_suffix(printer, T_next, P_i[j].VOs[0].T_f, T_f);

                if (fingerprint_equals(P_i[j + 1].P, T_f)) {
                    add_occurance(printer, T_next, i, &P_i[j + 1]);
                }
                shift_row(printer, &P_i[j], tmp);
            }
        }
        if (fingerprint_equals (P_i[0].P, T_cur)) {
            add_occurance(printer, T_next, i, &P_i[0]);
        }
        fingerprint_assign(T_next, T_prev);
    }

    fingerprinter_free(printer);
    fingerprint_free(T_next);
    fingerprint_free(T_f);
    fingerprint_free(T_prev);
    fingerprint_free(T_cur);
    fingerprint_free(tmp);
    for (i = 0; i < lm - 1; i++) {
        fingerprint_free(P_i[i].P);
        fingerprint_free(P_i[i].period_f);
        fingerprint_free(P_i[i].VOs[0].T_f);
        fingerprint_free(P_i[i].VOs[1].T_f);
    }
    free(P_i);

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
        fingerprint_suffix(printer, T_f, T_i, tmp);
        fingerprint_concat(printer, tmp, T_m, T_f);
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
