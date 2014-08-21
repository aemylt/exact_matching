#include "exact_matching.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define test_check(correct, correct_len, results, results_len) assert((correct_len == results_len) && (check_results(correct, results, correct_len)))

int check_results(int* correct, int* result, int size) {
    int i;
    for (i = 0; i < size; i++) if (correct[i] != result[i]) return 0;
    return 1;
}

void stream_test(char *T, int n, char *P, int m, char *sigma, int s_sigma, int* correct, int num_correct) {
    int i, counter = 0;
    exactmatch_state state = exactmatch_build(P, m, sigma, s_sigma, n, 0);
    for (i = 0; i < n; i++) {
        if ((counter < num_correct) && (i == correct[counter])) assert(exactmatch_stream(&state, T[i]) == correct[counter++]);
        else assert(exactmatch_stream(&state, T[i]) == -1);
    }

    exactmatch_free(&state);
}

int main(void) {
    char *T = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbcccccaaaaa", *P = "aaaaabbbbbcccccaaaaa";
    int i, *results = (int*)malloc(81 * sizeof(int)), alpha = 0, *correct = (int*)malloc(81 * sizeof(int)), correct_len;
    correct[0] = 19; correct[1] = 59; correct[2] = 99;
    correct_len = 3;
    int results_len = fingerprint_match(T, 100, P, 20, "abcd", 4, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 100, P, 20, "abcd", 4, correct, correct_len);

    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccddddd";
    correct[0] = 39;
    correct_len = 1;
    results_len = fingerprint_match(T, 100, P, 40, "abcd", 4, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 100, P, 40, "abcd", 4, correct, correct_len);

    T = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbcccccaaaaaaaaaabbbbbcccccddddd";
    correct[0] = 39; correct[1] = 99;
    correct_len = 2;
    results_len = fingerprint_match(T, 100, P, 40, "abcd", 4, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 100, P, 40, "abcd", 4, correct, correct_len);

    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcc";
    correct[0] = 31; correct[1] = 71; correct[2] = 91;
    correct_len = 3;
    results_len = fingerprint_match(T, 100, P, 32, "abcd", 4, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 100, P, 32, "abcd", 4, correct, correct_len);

    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaa";
    correct[0] = 63;
    correct_len = 1;
    results_len = fingerprint_match(T, 100, P, 64, "abcd", 4, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 100, P, 64, "abcd", 4, correct, correct_len);

    T = "aaaaaaabbbbbcccccaaaaaaaaaabbbaaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaa";
    correct[0] = 93;
    correct_len = 1;
    results_len = fingerprint_match(T, 94, P, 64, "abcd", 4, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 94, P, 64, "abcd", 4, correct, correct_len);

    T = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    P = "aaaaaaaaaaaaaaaaaaaa";
    for (i = 0; i < 81; i++) {
        correct[i] = i + 19;
    }
    correct_len = 81;
    results_len = fingerprint_match(T, 100, P, 20, "a", 1, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 100, P, 20, "a", 1, correct, correct_len);

    T = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab";
    P = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab";
    correct[0] = 63; correct[1] = 199;
    correct_len = 2;
    results_len = fingerprint_match(T, 200, P, 64, "ab", 2, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 200, P, 64, "ab", 2, correct, correct_len);

    T = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb";
    P = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb";
    results_len = fingerprint_match(T, 200, P, 64, "ab", 2, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 200, P, 64, "ab", 2, correct, correct_len);

    T = "aaaaaaabbbbbcccccaaaaaaaaaabbaaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaa";
    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaa";
    correct[0] = 92;
    correct_len = 1;
    results_len = fingerprint_match(T, 93, P, 64, "abcd", 4, alpha, results);
    test_check(correct, correct_len, results, results_len);
    stream_test(T, 93, P, 64, "abcd", 4, correct, correct_len);

    free(results);
    free(correct);
    return 0;
}
