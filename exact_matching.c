#include <stdio.h>
#include <stdlib.h>
#include "exact_matching.h"

int main(void) {
    int n = 100, m = 20, i;
    int* results = malloc((n - m + 1) * sizeof(int));
    int count = fingerprint_match("aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbcccccaaaaa", n, "aaaaabbbbbcccccaaaaa", m, 0, results);

    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    printf("%d\n", results[count - 1]);
    return 0;
}