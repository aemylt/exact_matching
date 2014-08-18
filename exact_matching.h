#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"
#include "parameterised_matching/kmp.h"

#include <stdlib.h>
#include <stdio.h>

/*
    typedef struct viable_occurance
    Structure for viable occurances (VO).
    Components:
        int         location - The location of the occurance
        fingerprint T_f      - The fingerprint of the occurance
*/
typedef struct {
    int location;
    fingerprint T_f;
} viable_occurance;

/*
    typedef struct pattern_row
    Structure for rows of the pattern.
    Components:
        int         row_size - The size of the row
        int         period   - The length of the given period in the row
        int         count    - The number of current VOs
        fingerprint P        - The pattern's fingerprint
        fingerprint period_f - The fingerprint of the current period
*/
typedef struct {
    int row_size, period, count;
    fingerprint P, period_f;
    viable_occurance VOs[2];
} pattern_row;

/*
    void shift_row(fingerprinter printer, pattern_row *P_i, fingerprint tmp)
    Removes the first VO from the row.
    Parameters:
        fingerprinter printer - The printer for this case
        pattern_row   *P_i    - Address of the row
        fingerprint   tmp     - Temporary fingerprint to avoid memory allocation
    Returns void:
        Value modified in P_i
*/
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

/*
    void add_occurance(fingerprinter printer, fingerprint T_f, int location, pattern_row *P_i, fingerprint tmp)
    Adds a VO to the row. If there are less than two occurances then it is simply added, otherwise it is added if it fits the period properties.
    Parameters:
        fingerprinter printer  - The printer for this case
        fingerprint   T_f      - The fingerprint to add
        int           location - The location in the text of the fingerprint
        pattern_row   *P_i     - The address if the row
        fingerprint   tmp      - Temporary fingerprint to avoid memory allocation
    Returns void:
        Value modified in P_i
*/
void add_occurance(fingerprinter printer, fingerprint T_f, int location, pattern_row *P_i, fingerprint tmp) {
    if (P_i->count < 2) {
        fingerprint_assign(T_f, P_i->VOs[P_i->count].T_f);
        P_i->VOs[P_i->count].location = location;
        P_i->count++;
    } else {
        if (P_i->count == 2) {
            P_i->period = P_i->VOs[1].location - P_i->VOs[0].location;
            fingerprint_suffix(printer, P_i->VOs[1].T_f, P_i->VOs[0].T_f, P_i->period_f);
        }
        fingerprint_suffix(printer, T_f, P_i->VOs[1].T_f, tmp);
        int period = location - P_i->VOs[1].location;
        if ((period == P_i->period) && (fingerprint_equals(tmp, P_i->period_f))) {
            fingerprint_assign(T_f, P_i->VOs[1].T_f);
            P_i->VOs[1].location = location;
            P_i->count++;
        } else printf("Warning: Error in Period occured at location %d. VO discarded.\n", location);
    }
}

/*
    int fingerprint_match(char* T, int n, char* P, int m, int alpha, int* results)
    Exact matching in O(nlogm) time with fingerprints.
    Parameters:
        char *T       - Text
        int  n        - Size of text
        char *P       - Pattern
        int  m        - Size of pattern
        int  alpha    - Error constant
        int  *results - Array for results
    Returns int:
        Number of matches
        Index of each match returned in results
*/
int fingerprint_match(char *T, int n, char *P, int m, int alpha, int *results) {
    int lm = 0, f = 0, i = 0, j, matches = 0;
    while ((1 << lm) <= m) lm++;
    while ((1 << f <= lm)) f++;
    lm -= f + 1;
    j = 1 << f;
    kmp_state P_f = kmp_build(P, j);
    fingerprinter printer = fingerprinter_build(n, alpha);
    fingerprint T_f = init_fingerprint(), T_cur = init_fingerprint(), tmp = init_fingerprint();
    pattern_row *P_i = malloc(lm * sizeof(pattern_row));
    while (j << 2 < m) {
        P_i[i].row_size = j;
        P_i[i].period = 0;
        P_i[i].count = 0;
        P_i[i].P = init_fingerprint();
        set_fingerprint(printer, &P[j], j, P_i[i].P);
        P_i[i].period_f = init_fingerprint();
        P_i[i].VOs[0].T_f = init_fingerprint();
        P_i[i].VOs[0].location = 0;
        P_i[i].VOs[1].T_f = init_fingerprint();
        P_i[i].VOs[1].location = 0;
        j <<= 1;
        i++;
    }
    P_i[i].period = 0;
    P_i[i].count = 0;
    P_i[i].P = init_fingerprint();
    set_fingerprint(printer, &P[j], m - j, P_i[i].P);
    P_i[i].period_f = init_fingerprint();
    P_i[i].VOs[0].T_f = init_fingerprint();
    P_i[i].VOs[0].location = 0;
    P_i[i].VOs[1].T_f = init_fingerprint();
    P_i[i].VOs[1].location = 0;
    P_i[i].row_size = m - j;

    lm = i + 1;

    P_i = realloc(P_i, lm * sizeof(pattern_row));

    fingerprint *past_prints = malloc(lm * sizeof(fingerprint));
    for (i = 0; i < lm; i++) past_prints[i] = init_fingerprint();
    j = 0;

    for (i = 0; i < n; i++) {
        set_fingerprint(printer, &T[i], 1, T_cur);
        fingerprint_concat(printer, past_prints[(j) ? j - 1 : lm - 1], T_cur, tmp);
        fingerprint_assign(tmp, past_prints[j]);

        if ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location >= P_i[j].row_size)) {
            fingerprint_assign(past_prints[(P_i[j].VOs[0].location + P_i[j].row_size) % lm], T_cur);
            fingerprint_suffix(printer, T_cur, P_i[j].VOs[0].T_f, T_f);

            if (fingerprint_equals(P_i[j].P, T_f)) {
                if (j == lm - 1) results[matches++] = P_i[j].VOs[0].location + P_i[j].row_size;
                else add_occurance(printer, T_cur, P_i[j].VOs[0].location + P_i[j].row_size, &P_i[j + 1], tmp);
            }
            shift_row(printer, &P_i[j], tmp);
        }
        if (kmp_stream(P_f, T[i], i) != -1) {
            add_occurance(printer, past_prints[j], i, &P_i[0], tmp);
        }
        if (++j == lm) j = 0;
    }

    fingerprinter_free(printer);
    fingerprint_free(T_f);
    fingerprint_free(T_cur);
    fingerprint_free(tmp);
    for (i = 0; i < lm; i++) {
        fingerprint_free(P_i[i].P);
        fingerprint_free(P_i[i].period_f);
        fingerprint_free(P_i[i].VOs[0].T_f);
        fingerprint_free(P_i[i].VOs[1].T_f);
    }
    free(P_i);

    return matches;
}

int fingerprint_match_naive(char *T, int n, char *P, int m, int alpha, int *results) {
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
