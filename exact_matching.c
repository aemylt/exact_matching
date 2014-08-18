#include "exact_matching.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n = 100, m = 20, i, *results = malloc((81) * sizeof(int)), alpha = 0;
    char *T = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbcccccaaaaa", *P = "aaaaabbbbbcccccaaaaa";
    int count = fingerprint_match_naive(T, n, P, m, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    printf("%d\n", results[count - 1]);

    count = fingerprint_match(T, n, P, m, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);

    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccddddd";
    m = 40;
    count = fingerprint_match(T, n, P, m, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);

    T = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbcccccaaaaaaaaaabbbbbcccccddddd";
    count = fingerprint_match(T, n, P, m, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);

    T = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    P = "aaaaaaaaaaaaaaaaaaaa";
    n = 100; m = 20;
    count = fingerprint_match(T, n, P, m, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    return 0;
}
