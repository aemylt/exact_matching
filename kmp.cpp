#include "kmp.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

int compare(int* A, int* B, int m) {
    int i;
    for (i = 0; i < m; i++) {
        if (A[i] != B[i]) return 0;
    }
    return 1;
}

void stream_test(string T, int n, string P, int m, int* correct) {
    kmp_state state = kmp_build(P, m);
    int j;
    for (j = 0; j < n; j++) {
        assert(correct[j] == kmp_stream(&state, T[j], j));
    }
}

int main(void) {
    int n = 18, m = 5;
    int correct_matches = 2;
    int* correct = (int*)malloc(correct_matches * sizeof(int));
    correct[0] = 8; correct[1] = 13;
    int* output = (int*)malloc((n - m + 1) * sizeof(int));
    int matches = kmp_match("ababaabbababbababb", n, "ababb", m, output);
    assert(correct_matches == matches && compare(output, correct, matches));
    correct = (int*)realloc(correct, n * sizeof(int));
    correct[0]  = -1; correct[1]  = -1; correct[2]  = -1; correct[3]  = -1; correct[4]  = -1; correct[5]  = -1;
    correct[6]  = -1; correct[7]  = -1; correct[8]  = -1; correct[9]  = -1; correct[10] = -1; correct[11] = -1;
    correct[12] = 12; correct[13] = -1; correct[14] = -1; correct[15] = -1; correct[16] = -1; correct[17] = 17;
    stream_test("ababaabbababbababb", n, "ababb", m, correct);
    output = (int*)realloc(output, 81 * sizeof(int));
    matches = kmp_match("aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbcccccaaaaa", 100, "aaaaabbbbbcccccaaaaa", 20, output);
    int i;
    for (i = 0; i < matches - 1; i++) {
        printf("%d, ", output[i]);
    }
    printf("%d\n", output[matches - 1]);
    free(correct);
    free(output);
    printf("All tests succeeded!\n");
    return 0;
}