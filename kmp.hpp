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

class kmp_stream {
        string P;
        int m;
        int index;
        int* failure;
    public:
        kmp_stream(string Pattern, int P_len) {
            P = Pattern;
            m = P_len;
            index = 0;
            failure = (int*)malloc(m * sizeof(int));
            int i = 0, j;
            failure[0] = 0;
            for (j = 1; j < P_len; j++) {
                while (i > 0 && Pattern[i] != Pattern[j]) i = failure[i];
                if (Pattern[i] == Pattern[j]) i++;
                failure[j] = (i) ? i - 1 : 0;
            }
        }

        ~kmp_stream() {
            free(failure);
        }

        int kmp_match(char T_j, int j) {
            int result = -1;
            while (index > 0 && P[index] != T_j) index = failure[index];
            if (P[index] == T_j) index++;
            if (index == m) {
                result = j;
                index = failure[index - 1];
            }
            return result;
        }
} ;

#endif
