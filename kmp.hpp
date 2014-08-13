/*
    kmp.h
    Implementation of the Knuth-Morris-Pratt pattern matching algorithm.
    More information is available here: http://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
    This variant of the algorithm is from lecture slides courtesy of Dr. Raphaël Clifford: (private link) https://www.cs.bris.ac.uk/Teaching/Resources/COMS21103/material-dima/string_matching.pdf
*/

#ifndef KMP
#define KMP
#include <stdlib.h>
#include <string>

using namespace std;

/*
    void kmp_failure(char* P, int m, int* failure)
    Constructs the failure table for pattern P.
    Parameters:
        char* P       - Pattern
        int   m       - Length of pattern
        int*  failure - Variable for failure table to be returned in
    Returns void:
        Value returned in failure parameter
        failure[i] = longest prefix of P[0:i] that is also a suffix of P[0:i]
    Notes:
        |P| == |failure| == m
*/
void kmp_failure(string P, int m, int* failure) {
    int i = 0, j;
    failure[0] = 0;
    for (j = 1; j < m; j++) {
        while (i > 0 && P[i] != P[j]) i = failure[i];
        if (P[i] == P[j]) i++;
        failure[j] = (i) ? i - 1 : 0;
    }
}

/*
    int kmp_match(char* T, int n, char* P, int m, int* output)
    Finds all occurances of P in T via the Knuth-Morris-Pratt algorithm.
    Parameters:
        char* T      - Text to search
        int   n      - Length of text
        char* P      - Pattern to search for
        int   m      - Length of pattern
        int*  output - Variable for match indexes to be returned in
    Returns int:
        Number of matches found
        Match indexes returned in output parameter
    Notes:
        |T| == n
        |P| == m
        |output| == n - m + 1 (maximum number of matches)
*/
int kmp_match(string T, int n, string P, int m, int* output) {
    int* failure = (int*)malloc(m * sizeof(int));
    kmp_failure(P, m, failure);
    int i = 0, j, matches = 0;
    for (j = 0; j < n; j++) {
        while (i > 0 && P[i] != T[j]) i = failure[i];
        if (P[i] == T[j]) i++;
        if (i == m) {
            output[matches++] = j - m + 1;
            i = failure[i - 1];
        }
    }
    output = (int*)realloc(output, matches * sizeof(int));
    free(failure);
    return matches;
}

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
    string P;
    int m;
    int i;
    int* failure;
} kmp_state;

/*
    kmp_state kmp_build(char* P, int m)
    Creates an initial KMP state for the pattern
    Parameters:
        char* P - Pattern
        int   m - Length of pattern
    Returns kmp_state:
        An initial KMP state for the pattern, where i = j = 0
*/
kmp_state kmp_build(string P, int m) {
    kmp_state state;
    state.P = P;
    state.m = m;
    state.i = 0;
    state.failure = (int*)malloc(m * sizeof(int));
    kmp_failure(P, m, state.failure);
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
    string P = state->P;
    int i = state->i, result = -1;
    int* failure = state->failure;
    while (i > 0 && P[i] != T_j) i = failure[i];
    if (P[i] == T_j) i++;
    if (i == state->m) {
        result = j;
        i = failure[i - 1];
    }
    state->i = i;
    return result;
}

#endif
