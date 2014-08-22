/*
    exact_matching.h
    Performs probabilistic exact matching in constant time per character with logarithmic space.
    Based on work by Breslauer and Galil: http://www.dagstuhl.de/mat/Files/11/11081/11081.BreslauerDany.ExtAbstract.pdf
*/

#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.h"
#include "kmp.h"

#include <stdlib.h>
#include <stdio.h>

/*
    typedef struct viable_occurance
    Structure for points where there may be a pattern.
    Components:
        int         location - The location of the occurance
        fingerprint T_f      - The fingerprint of the text up to that location
*/
typedef struct {
    int location;
    fingerprint T_f;
} viable_occurance;

/*
    typedef struct pattern_row
    Structure for each portion of the pattern.
    Components:
        int              row_size - The size of the portion
        int              period   - The length of the current period
        int              count    - The number of items in the portion
        fingerprint      P        - The fingerprint of this portion of the pattern
        fingerprint      period_f - The fingerprint of the current period
        viable_occurance VOs[2]   - The first and last viable occurances
*/
typedef struct {
    int row_size, period, count;
    fingerprint P, period_f;
    viable_occurance VOs[2];
} pattern_row;

/*
    shift_row
    Removes a viable occurance from this portion of the pattern.
    Parameters:
        fingerprinter printer - The printer to use
        pattern_row   *P_i    - The row to shift
        fingerprint   tmp     - Temporary space
    Returns void:
        Value returned by reference in P_i.
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
    add_occurance
    Adds a viable occurance to a row.
    Parameters:
        fingerprinter printer - The printer to use
        fingerprint T_f - The fingerprint of the viable occurance
        int location - The location of the viable occurance
        pattern_row *P_i - The stage to add the viable occurance to
        fingerprint tmp - Temporary space
    Returns void:
        Value returned by reference in P_i.
        If there is a period and the viable occurance doesn't fit the period, the viable occurance will be discarded.
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
    fingerprint_match
    Exact matching on the whole text and pattern using fingerprints.
    Parameters:
        char *T - Text
        int n - Length of text
        char *P - Pattern
        int m - Length of pattern
        char *sigma - Alphabet
        int s_sigma - Size of alphabet
        int *results - Matches
    Returns int:
        Number of matches.
        Location of matches returned by reference in results.
*/
int fingerprint_match(char *T, int n, char *P, int m, char *sigma, int s_sigma, int alpha, int *results) {
    int lm = 0, f = 0, i = 0, j, matches = 0;
    while ((1 << lm) <= m) lm++;
    while ((1 << f <= lm)) f++;
    lm -= f + 1;
    kmp_state P_f = kmp_build(P, 1 << f, m, sigma, s_sigma);
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

/*
    typedef struct fmatch_state
    Structure for the current state of the fingerprint matching algorithm.
    Components:
        int           lm           - Number of rows
        int           row_index    - Current row to check
        int           periodic     - 1 if the pattern is periodic, 0 otherwise
        kmp_state     P_f          - KMP stream of the first log_2(log_2(m)) characters
        fingerprinter printer      - The printer to use
        fingerprint   T_f          - The fingerprint to check any viable occurances with
        fingerprint   T_cur        - The fingerprint of the character that just occured
        fingerprint   tmp          - Temporary space
        fingerprint   *past_prints - The last lm fingerprints to occur
        pattern_row   *P_i         - Array of pattern components
*/
typedef struct {
    int lm, row_index, periodic;
    kmp_state P_f;
    fingerprinter printer;
    fingerprint T_f, T_cur, tmp, *past_prints;
    pattern_row *P_i;
} fmatch_state;

/*
    fmatch_build
    Constructs a fingerprint-matching state.
    Parameters:
        char *P      - The pattern
        int  m       - Length of the pattern
        char *sigma  - The alphabet
        int  s_sigma - Size of the alphabet
        int  n       - Length of the text
        int  alpha   - Desired level of accuracy
    Returns fmatch_state:
        Initial state for fingerprint matching
*/
fmatch_state fmatch_build(char *P, int m, char *sigma, int s_sigma, int n, int alpha) {
    fmatch_state state;
    int f = 0, j;
    state.lm = 0;
    while ((1 << state.lm) <= m) state.lm++;
    while ((1 << f <= state.lm)) f++;
    state.lm -= f + 1;
    state.P_f = kmp_build(P, 1 << f, m, sigma, s_sigma);
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

    state.past_prints = malloc((state.lm - 1) * sizeof(fingerprint));
    state.past_prints[0] = init_fingerprint();
    for (i = 1; i < state.lm - 1; i++) state.past_prints[i] = init_fingerprint();
    state.row_index = 0;
    return state;
}

/*
    fmatch_stream
    Performs next round of fingerprint matching.
    Parameters:
        fmatch_state *state - The current state of the algorithm
        char T_i - The next character of the text
        int i - The index of the text
    Returns int:
        Index of latest match if one was found in this round.
        -1 otherwise
        Parameter state modified by reference to the next state of the algorithm.
    Notes:
        Matches may be found up to log_2(m) rounds after index was entered.
*/
int fmatch_stream(fmatch_state *state, char T_i, int i) {
    int result = -1;
    if (state->periodic) {
        result = kmp_stream(&state->P_f, T_i, i);
    } else {
        int j = state->row_index, lm = state->lm;
        fingerprinter printer = state->printer;
        fingerprint T_f = state->T_f, T_cur = state->T_cur, tmp = state->tmp, *past_prints = state->past_prints;
        pattern_row P_l = state->P_i[lm - 1];
        set_fingerprint(printer, &T_i, 1, T_cur);
        fingerprint_concat(printer, past_prints[(j) ? j - 1 : ((lm - 1) ? lm - 2 : 0)], T_cur, tmp);
        fingerprint_assign(tmp, past_prints[j]);

        if (j < lm - 1) {
            pattern_row P_j = state->P_i[j];
            if ((P_j.count > 0) && (i - P_j.VOs[0].location >= P_j.row_size)) {
                fingerprint_assign(past_prints[(P_j.VOs[0].location + P_j.row_size) % (lm - 1)], T_cur);
                fingerprint_suffix(printer, T_cur, P_j.VOs[0].T_f, T_f);

                if (fingerprint_equals(P_j.P, T_f)) add_occurance(printer, T_cur, P_j.VOs[0].location + P_j.row_size, &state->P_i[j + 1], tmp);
                shift_row(printer, &state->P_i[j], tmp);
            }
        }

        if ((P_l.count > 0) && (i - P_l.VOs[0].location == P_l.row_size)) {
            fingerprint_assign(past_prints[j], T_cur);
            fingerprint_suffix(printer, T_cur, P_l.VOs[0].T_f, T_f);

            if (fingerprint_equals(P_l.P, T_f)) result = i;
            shift_row(printer, &state->P_i[lm - 1], tmp);
        }
        if (kmp_stream(&state->P_f, T_i, i) != -1) {
            add_occurance(printer, past_prints[j], i, &state->P_i[0], tmp);
        }
        if (++state->row_index >= lm - 1) state->row_index = 0;
    }
    return result;
}

/*
    fmatch_free
    Frees a fingerprint matching state.
    Parameters:
        fmatch_state *state - The algorithm to free
*/
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

    fingerprint_free(state->past_prints[0]);
    for (i = 1; i < state->lm - 1; i++) fingerprint_free(state->past_prints[i]);
}

#endif
