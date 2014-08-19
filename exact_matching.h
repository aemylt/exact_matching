#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"
#include "kmp.h"

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
    kmp_state P_f = kmp_build(P, 1 << f, m);
    j = P_f.m;
    if (j == m) {
        for (i = 0; i < n; i++) if (kmp_stream(&P_f, T[i], i) != -1) results[matches++] = i;
        kmp_free(&P_f);
        return matches;
    }

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
        if (kmp_stream(&P_f, T[i], i) != -1) {
            add_occurance(printer, past_prints[j], i, &P_i[0], tmp);
        }
        if (++j == lm) j = 0;
    }

    while (j < lm) {
        if ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location >= P_i[j].row_size)) {
            fingerprint_assign(past_prints[(P_i[j].VOs[0].location + P_i[j].row_size) % lm], T_cur);
            fingerprint_suffix(printer, T_cur, P_i[j].VOs[0].T_f, T_f);

            if (fingerprint_equals(P_i[j].P, T_f)) {
                if (j == lm - 1) results[matches++] = P_i[j].VOs[0].location + P_i[j].row_size;
                else add_occurance(printer, T_cur, P_i[j].VOs[0].location + P_i[j].row_size, &P_i[j + 1], tmp);
            }
            shift_row(printer, &P_i[j], tmp);
        }
        j++;
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
    kmp_free(&P_f);

    return matches;
}

typedef struct {
    int lm, row_index;
    kmp_state P_f;
    fingerprinter printer;
    fingerprint T_f, T_cur, tmp, *past_prints;
    pattern_row *P_i;
    int periodic;
} fmatch_state;

fmatch_state fmatch_build(char *P, int m, int n, int alpha) {
    fmatch_state state;
    int f = 0, j;
    state.lm = 0;
    while ((1 << state.lm) <= m) state.lm++;
    while ((1 << f <= state.lm)) f++;
    state.lm -= f + 1;
    state.P_f = kmp_build(P, 1 << f, m);
    j = state.P_f.m;
    if (j == m) {
        state.periodic = 1;
        return state;
    }

    state.periodic = 0;
    state.printer = fingerprinter_build(n, alpha);
    state.T_f = init_fingerprint();
    state.T_cur = init_fingerprint();
    state.tmp = init_fingerprint();
    state.P_i = malloc(((state.lm) ? state.lm : 1) * sizeof(pattern_row));

    int i = 0;
    while (j << 2 < m) {
        state.P_i[i].row_size = j;
        state.P_i[i].period = 0;
        state.P_i[i].count = 0;
        state.P_i[i].P = init_fingerprint();
        set_fingerprint(state.printer, &P[j], j, state.P_i[i].P);
        state.P_i[i].period_f = init_fingerprint();
        state.P_i[i].VOs[0].T_f = init_fingerprint();
        state.P_i[i].VOs[0].location = 0;
        state.P_i[i].VOs[1].T_f = init_fingerprint();
        state.P_i[i].VOs[1].location = 0;
        j <<= 1;
        i++;
    }
    state.P_i[i].period = 0;
    state.P_i[i].count = 0;
    state.P_i[i].P = init_fingerprint();
    set_fingerprint(state.printer, &P[j], m - j, state.P_i[i].P);
    state.P_i[i].period_f = init_fingerprint();
    state.P_i[i].VOs[0].T_f = init_fingerprint();
    state.P_i[i].VOs[0].location = 0;
    state.P_i[i].VOs[1].T_f = init_fingerprint();
    state.P_i[i].VOs[1].location = 0;
    state.P_i[i].row_size = m - j;

    state.lm = i + 1;

    state.P_i = realloc(state.P_i, state.lm * sizeof(pattern_row));

    state.past_prints = malloc(state.lm * sizeof(fingerprint));
    for (i = 0; i < state.lm; i++) state.past_prints[i] = init_fingerprint();
    state.row_index = 0;
    return state;
}

int fmatch_stream(fmatch_state *state, char T_i, int i) {
    int result = -1;
    if (state->periodic) {
        result = kmp_stream(&state->P_f, T_i, i);
    } else {
        int j = state->row_index, lm = state->lm;
        fingerprinter printer = state->printer;
        fingerprint T_f = state->T_f, T_cur = state->T_cur, tmp = state->tmp, *past_prints = state->past_prints;
        pattern_row P_j = state->P_i[j];
        set_fingerprint(printer, &T_i, 1, T_cur);
        fingerprint_concat(printer, past_prints[(j) ? j - 1 : lm - 1], T_cur, tmp);
        fingerprint_assign(tmp, past_prints[j]);

        if ((P_j.count > 0) && (i - P_j.VOs[0].location >= P_j.row_size)) {
            fingerprint_assign(past_prints[(P_j.VOs[0].location + P_j.row_size) % lm], T_cur);
            fingerprint_suffix(printer, T_cur, P_j.VOs[0].T_f, T_f);

            if (fingerprint_equals(P_j.P, T_f)) {
                if (j == lm - 1) result = P_j.VOs[0].location + P_j.row_size;
                else add_occurance(printer, T_cur, P_j.VOs[0].location + P_j.row_size, &state->P_i[j + 1], tmp);
            }
            shift_row(printer, &state->P_i[j], tmp);
        }
        if (kmp_stream(&state->P_f, T_i, i) != -1) {
            add_occurance(printer, past_prints[j], i, &state->P_i[0], tmp);
        }
        if (++state->row_index == lm) state->row_index = 0;
    }
    return result;
}

void fmatch_free(fmatch_state *state) {
    kmp_free(&state->P_f);
    if (state->periodic) return;

    fingerprinter_free(state->printer);
    fingerprint_free(state->T_f);
    fingerprint_free(state->T_cur);
    fingerprint_free(state->tmp);

    int i;
    for (i = 0; i < state->lm; i++) {
        fingerprint_free(state->P_i[i].P);
        fingerprint_free(state->P_i[i].period_f);
        fingerprint_free(state->P_i[i].VOs[0].T_f);
        fingerprint_free(state->P_i[i].VOs[1].T_f);
    }
    free(state->P_i);
}

typedef struct {
    fmatch_state fmatch;
    kmp_state kmp;
    int text_index, m, lm, *buffer;
} exactmatch_state;

exactmatch_state exactmatch_build(char *P, int m, int n, int alpha) {
    exactmatch_state state;
    state.m = m - 1;
    int lm = 0;
    while ((1 << lm) <= m) lm++;
    state.fmatch = fmatch_build(P, m - lm, n, alpha);
    state.kmp = kmp_build(&P[m - lm], lm, lm);
    state.lm = lm;
    state.buffer = malloc((lm << 1) * sizeof(int));
    state.text_index = 0;
    return state;
}

int exactmatch_stream(exactmatch_state *state, char T_i) {
    int result = -1, kmp_result, fmatch_result, i = state->text_index;
    kmp_result = kmp_stream(&state->kmp, T_i, i);
    fmatch_result = fmatch_stream(&state->fmatch, T_i, i);
    if (fmatch_result != -1) state->buffer[fmatch_result % (state->lm << 1)] = fmatch_result;

    if (i >= state->m) {
        int buffer_index = i % state->lm;
        if ((kmp_result == i) && ((state->buffer[buffer_index] == i - state->lm) || (state->buffer[buffer_index + state->lm] == i - state->lm))) result = i;
    }
    state->text_index++;

    return result;
}

void exactmatch_free(exactmatch_state *state) {
    fmatch_free(&state->fmatch);
    kmp_free(&state->kmp);
    free(state->buffer);
}

#endif
