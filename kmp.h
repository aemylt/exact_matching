/*
    kmp.h
    Stream-based pattern matching via Knuth-Morris-Pratt (KMP): http://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
    This variant of the algorithm is from lecture slides courtesy of Dr. Raphaël Clifford: (private link) https://www.cs.bris.ac.uk/Teaching/Resources/COMS21103/material-dima/string_matching.pdf
    Deamortised using variant of Simon's Algorithm explained by Dr. Markus Jalsenius: http://www.cs.bris.ac.uk/home/markus/slides/cpm12.pdf
    Pattern is compressed using a technique by Breslauer and Galil (page 8): http://www.dagstuhl.de/mat/Files/11/11081/11081.BreslauerDany.ExtAbstract.pdf
*/

#ifndef KMP
#define KMP
#include "hash_lookup.h"
#include <stdlib.h>

/*
    typedef struct kmp_state
    Structure to hold state of KMP algorithm.
    Components:
        char        *P            - The pattern
        hash_lookup *lookup       - The failure tables of the pattern
        int         m             - Length of the pattern
        int         period_len    - Length of the period
        int         i             - Current index of pattern
        int         matched_reset - Length of the longest prefix of P that is also a suffix of P
        int         has_break     - 1 if the period breaks at the end of the pattern, 0 otherwise
        char        period_break  - The character that breaks the period in the pattern
        hash_lookup break_lookup  - The failure table of the character that breaks the period
*/
typedef struct {
    char *P, period_break;
    int m, i, matched_reset, period_len, has_break;
    hash_lookup *lookup, break_lookup;
} kmp_state;

/*
    get_P_i
    Returns the character at that point in the pattern.
    Parameters:
        kmp_state state - The current state of the algorithm
        int       i     - The index
    Returns char:
        P[i]
*/
char get_P_i(kmp_state state, int i) {
    if (i < state.period_len) return state.P[i];
    if ((i == state.m - 1) && (state.has_break)) return state.period_break;
    return state.P[i % state.period_len];
}

/*
    get_failure_i
    Returns the failure count at that point in the pattern. Only used in preprocessing.
    Parameters:
        kmp_state state    - The current state of the algorithm
        int       *failure - The failure table
        int       i        - The index
    Returns char:
        failure[i]
*/
int get_failure_i(kmp_state state, int* failure, int i, int double_period) {
    if (i < double_period) return failure[i];
    return failure[double_period - 1] + i - double_period + 1;
}

/*
    get_hash_i
    Returns the failure count at that point in the pattern for that character.
    Parameters:
        kmp_state state - The current state of the algorithm
        int       i     - The index
        char      a     - The character to lookup
    Returns char:
        lookup[i][a]
*/
int get_hash_i(kmp_state state, int i, char a) {
    if (i < (state.period_len << 1)) return hashlookup_search(state.lookup[i], a);
    if ((i == state.m - 1) && (state.has_break)) return hashlookup_search(state.break_lookup, a);
    return hashlookup_search(state.lookup[(i % state.period_len) + state.period_len], a);
}

/*
    kmp_build
    Constructs a Knuth-Morris-Pratt algorithm for a pattern.
    Parameters:
        char *P      - The pattern
        int  m       - Minimum length of the pattern to preprocess
        int  p_len   - Maximum length of the pattern to preprocess
        char *sigma  - The alphabet
        int  s_sigma - The size of the alphabet
    Returns kmp_state:
        The starting state for the algorithm
*/
kmp_state kmp_build(char *P, int m, int p_len, char *sigma, int s_sigma) {
    int i, j, k, l, count, *values, *failure;
    char **keys;
    kmp_state state;
    state.period_len = m;
    state.has_break = 0;

    state.P = malloc(m * sizeof(char));
    for (i = 0; i < m; i++) state.P[i] = P[i];
    state.m = m;

    state.i = -1;
    failure = malloc(m * sizeof(int));
    failure[0] = -1;
    i = -1;

    keys = malloc(s_sigma * sizeof(char*));
    values = malloc(s_sigma * sizeof(int));

    for (j = 1; j < m; j++) {
        while (i > -1 && P[i + 1] != P[j]) i = failure[i];
        if (P[i + 1] == P[j]) i++;
        failure[j] = i;
    }
    state.matched_reset = failure[m - 1];

    if (((failure[m - 1] + 1) << 1) >= m) {
        state.period_len = m - failure[m - 1];
        int double_period = state.period_len << 1;
        state.P = realloc(state.P, state.period_len * sizeof(char));
        failure = realloc(failure, (state.period_len << 1) * sizeof(int));

        state.lookup = malloc((state.period_len << 1) * sizeof(hash_lookup));
        state.lookup[0] = hashlookup_build(keys, values, 0);

        for (j = 1; j < double_period; j++) {
            count = 0;
            for (k = 0; k < s_sigma; k++) {
                if (P[j] != sigma[k]) {
                    l = failure[j - 1];
                    if (P[l + 1] == sigma[k]) {
                        keys[count] = &sigma[k];
                        values[count++] = l + 1;
                    } else {
                        l = hashlookup_search(state.lookup[l + 1], sigma[k]);
                        if (l != -1) {
                            keys[count] = &sigma[k];
                            values[count++] = l;
                        }
                    }
                }
            }

            state.lookup[j] = hashlookup_build(keys, values, count);
        }

        while ((state.m < p_len) && (((i + 1) << 1) >= state.m)) {
            state.m++;
            while (i > -1 && P[i + 1] != P[state.m - 1]) i = get_failure_i(state, failure, i, double_period);
            if (P[i + 1] == P[state.m - 1]) i++;
            state.matched_reset = i;
            if (((i + 1) << 1) < state.m) {
                state.has_break = 1;
                state.period_break = P[state.m - 1];
                count = 0;
                for (k = 0; k < s_sigma; k++) {
                    if (P[state.m - 1] != sigma[k]) {
                        l = get_failure_i(state, failure, state.m - 2, double_period);
                        if (P[l + 1] == sigma[k]) {
                            keys[count] = &sigma[k];
                            values[count++] = l + 1;
                        } else {
                            l = get_hash_i(state, l + 1, sigma[k]);
                            if (l != -1) {
                                keys[count] = &sigma[k];
                                values[count++] = l;
                            }
                        }
                    }
                }
                state.break_lookup = hashlookup_build(keys, values, count);
            }
        }

    } else {
        state.lookup = malloc(m * sizeof(hash_lookup));
        state.lookup[0] = hashlookup_build(keys, values, 0);

        for (j = 1; j < m; j++) {
            count = 0;
            for (k = 0; k < s_sigma; k++) {
                if (P[j] != sigma[k]) {
                    l = failure[j - 1];
                    if (P[l + 1] == sigma[k]) {
                        keys[count] = &sigma[k];
                        values[count++] = l + 1;
                    } else {
                        l = hashlookup_search(state.lookup[l + 1], sigma[k]);
                        if (l != -1) {
                            keys[count] = &sigma[k];
                            values[count++] = l;
                        }
                    }
                }
            }

            state.lookup[j] = hashlookup_build(keys, values, count);
        }
    }

    free(keys);
    free(values);
    free(failure);

    return state;
}

/*
    kmp_stream
    Performs one round of KMP on the next character in the text.
    Parameters:
        kmp_state *state - The current state
        char      T_j    - The next character in the text
        int       j      - The index of the text
    Returns int:
        j if there is a match at index j
        -1 otherwise
        Parameter state modified by reference to the next state of the algorithm.
*/
int kmp_stream(kmp_state *state, char T_j, int j) {
    int i = state->i, result = -1;
    if (get_P_i(*state, i + 1) != T_j) i = get_hash_i(*state, i + 1, T_j);
    else i++;

    if (i == state->m - 1) {
        result = j;
        i = state->matched_reset;
    }
    state->i = i;
    return result;
}

/*
    kmp_free
    Frees a kmp_state object from memory.
    Parameters:
        kmp_state *state - The state to free
*/
void kmp_free(kmp_state *state) {
    free(state->P);

    int k, distance = (state->period_len == state->m) ? state->m : state->period_len << 1;
    for (k = 0; k < distance; k++) {
        hashlookup_free(&state->lookup[k]);
    }
    free(state->lookup);

    if (state->has_break) hashlookup_free(&state->break_lookup);
}

#endif
