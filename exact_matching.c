#include <stdio.h>
#include <stdlib.h>
#include "exact_matching.h"

int main(void) {
    int n = 100, m = 20, i, *results = malloc((n - m + 1) * sizeof(int)), alpha = 0;
    char *T = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbcccccaaaaa", *P = "aaaaabbbbbcccccaaaaa";
    int count = fingerprint_match_naive(T, n, P, m, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    printf("%d\n", results[count - 1]);
    results = realloc(results, (n - m + 1) * sizeof(int));

    count = fingerprint_match_allcrosses(T, n, P, m, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    printf("%d\n", results[count - 1]);
    return 0;
}