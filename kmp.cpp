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
    kmp_stream state(P, m);
    int j;
    for (j = 0; j < n; j++) {
        assert(correct[j] == state.kmp_match(T[j], j));
    }
}

int main(void) {
    int* correct = (int*)malloc(18 * sizeof(int));
    correct[0]  = -1; correct[1]  = -1; correct[2]  = -1; correct[3]  = -1; correct[4]  = -1; correct[5]  = -1;
    correct[6]  = -1; correct[7]  = -1; correct[8]  = -1; correct[9]  = -1; correct[10] = -1; correct[11] = -1;
    correct[12] = 12; correct[13] = -1; correct[14] = -1; correct[15] = -1; correct[16] = -1; correct[17] = 17;
    correct = (int*)realloc(correct, 100 * sizeof(int));
    for (int i = 0; i < 100; i++) {
        correct[i] = -1;
    }
    correct[19] = 19;
    correct[59] = 59;
    correct[99] = 99;
    stream_test("aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbcccccaaaaa", 100, "aaaaabbbbbcccccaaaaa", 20, correct);
    free(correct);
    printf("All tests succeeded!\n");
    return 0;
}