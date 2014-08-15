#include "exact_matching.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

int main(void) {
    string T = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbbbbbbaaaaaaaaaabbbbbcccccaaaaa", P = "aaaaabbbbbcccccaaaaa";
    int i, *results = (int*)malloc((T.size() - P.size() + 1) * sizeof(int)), alpha = 0;

    int count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccddddd";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    T = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaaabbbbbcccccaaaaaaaaaabbbbbcccccddddd";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcc";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    P = "aaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaa";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    T = "aaaaaaabbbbbcccccaaaaaaaaaabbbaaaaabbbbbcccccaaaaaaaaaabbbbbcccccdddddaaaaabbbbbcccccaaaaaaaaa";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    T = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    P = "aaaaaaaaaaaaaaaaaaaa";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    T = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab";
    P = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");

    T = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb";
    P = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb";
    count = fingerprint_match(T, P, alpha, results);
    for (i = 0; i < count - 1; i++) {
        printf("%d, ", results[i]);
    }
    if (count) printf("%d\n", results[count - 1]);
    else printf("No matches.\n");
    return 0;

}
