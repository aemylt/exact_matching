/*
    kmp.h
    Implementation of the Knuth-Morris-Pratt pattern matching algorithm.
    More information is available here: http://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
    This variant of the algorithm is from lecture slides courtesy of Dr. Raphaël Clifford: (private link) https://www.cs.bris.ac.uk/Teaching/Resources/COMS21103/material-dima/string_matching.pdf
*/

#ifndef KMP
#define KMP
#include <stdlib.h>

/*
    typedef struct kmp_state_t *kmp_state
    Structure to hold internal state of KMP algorithm during stream.
    Components:
        char* P       - Pattern
        int   m       - Length of pattern
        int   i       - Current index of pattern
        int*  failure - Failure table for pattern
*/
typedef struct {
    char* P;
    int m;
    int i;
    int* failure;
    int period_len;
    char period_break;
    int break_failure;
    int has_break;
} kmp_state;

char get_P_i(kmp_state state, int i) {
    if (i < state.period_len) return state.P[i];
    if ((i == state.m - 1) && (state.has_break)) return state.period_break;
    return state.P[i % state.period_len];
}

int get_failure_i(kmp_state state, int i) {
    if (i < state.period_len) return state.failure[i];
    if ((i == state.m - 1) && (state.has_break)) return state.break_failure;
    return state.failure[state.period_len - 1] + i - state.period_len + 1;
}

/*
    kmp_state kmp_build(char* P, int m)
    Creates an initial KMP state for the pattern
    Parameters:
        char* P - Pattern
        int   m - Length of pattern
    Returns kmp_state:
        An initial KMP state for the pattern, where i = j = 0
*/
kmp_state kmp_build(char* P, int m, int p_len) {
    int i, j;
    kmp_state state;
    state.period_len = m;
    state.has_break = 0;

    state.P = malloc(m * sizeof(char));
    for (i = 0; i < m; i++) state.P[i] = P[i];
    state.m = m;

    state.i = -1;
    state.failure = malloc(m * sizeof(int));
    state.failure[0] = -1;
    i = -1;
    for (j = 1; j < m; j++) {
        while (i > -1 && P[i + 1] != P[j]) i = state.failure[i];
        if (P[i + 1] == P[j]) i++;
        state.failure[j] = i;
    }

    if (((state.failure[m - 1] + 1) << 1) >= m) {
        state.period_len = (m - state.failure[m - 1]) << 1;
        state.P = realloc(state.P, state.period_len * sizeof(char));
        state.failure = realloc(state.failure, state.period_len * sizeof(int));
        while ((state.m < p_len) && (((get_failure_i(state, state.m - 1) + 1) << 1) >= state.m)) {
            state.m++;
            while (i > -1 && P[i + 1] != P[state.m - 1]) i = get_failure_i(state, i);
            if (P[i + 1] == P[state.m - 1]) i++;
            if (((i + 1) << 1) < state.m) {
                state.has_break = 1;
                state.period_break = P[state.m - 1];
                state.break_failure = i;
            }
        }
    }
    return state;
}

/*
    int kmp_stream(kmp_state state, char T_j)
    Returns whether a match occurs for character T_j.
    Parameters:
        kmp_state state - The current state of the algorithm
        char      T_j   - The next character in the text
        int       j     - The current index of the text
    Returns int:
        j  if P == T[j - m + 1:j]
        -1 otherwise
        state parameter is modified by reference
*/
int kmp_stream(kmp_state *state, char T_j, int j) {
    int i = state->i, result = -1;
    while (i > -1 && get_P_i(*state, i + 1) != T_j) i = get_failure_i(*state, i);
    if (get_P_i(*state, i + 1) == T_j) i++;
    if (i == state->m - 1) {
        result = j;
        i = get_failure_i(*state, i);
    }
    state->i = i;
    return result;
}

void kmp_free(kmp_state *state) {
    free(state->P);
    free(state->failure);
}

#endif
