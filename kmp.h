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
typedef struct kmp_state_t {
    char* P;
    int m;
    int i;
    int* failure;
} *kmp_state;

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
    kmp_state state = malloc(sizeof(struct kmp_state_t));
    state->P = P;
    state->i = -1;
    state->failure = malloc(p_len * sizeof(int));
    int i = -1, j;
    state->failure[0] = -1;
    for (j = 1; j < m; j++) {
        while (i > -1 && P[i + 1] != P[j]) i = state->failure[i];
        if (P[i + 1] == P[j]) i++;
        state->failure[j] = i;
    }
    while ((m < p_len) && (((state->failure[m - 1] + 1) << 1) >= m)) {
        m++;
        while (i > -1 && P[i + 1] != P[m - 1]) i = state->failure[i];
        if (P[i + 1] == P[m - 1]) i++;
        state->failure[m - 1] = i;
    }
    state->m = m;
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
int kmp_stream(kmp_state state, char T_j, int j) {
    char* P = state->P;
    int i = state->i, result = -1;
    int* failure = state->failure;
    while (i > -1 && P[i + 1] != T_j) i = failure[i];
    if (P[i + 1] == T_j) i++;
    if (i == state->m - 1) {
        result = j;
        i = failure[i];
    }
    state->i = i;
    return result;
}

#endif
