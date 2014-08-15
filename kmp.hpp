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
        int failure_index;
    public:
        kmp_stream(string Pattern, int P_len) {
            P = Pattern.substr(0, P_len);
            m = P_len;
            index = -1;
            failure = (int*)malloc(m * sizeof(int));
            failure_index = -1;
            int j;
            failure[0] = -1;
            for (j = 1; j < P_len; j++) {
                while (failure_index > -1 && Pattern[failure_index + 1] != Pattern[j]) failure_index = failure[failure_index];
                if (Pattern[failure_index + 1] == Pattern[j]) failure_index++;
                failure[j] = failure_index;
            }
        }

        ~kmp_stream() {
            free(failure);
        }

        void update_pattern(char P_j) {
            m++;
            P += P_j;
            failure = (int*)realloc(failure, m * sizeof(int));
            while (failure_index > -1 && P[failure_index + 1] != P_j) failure_index = failure[failure_index];
            if (P[failure_index + 1] == P_j) failure_index++;
            failure[m - 1] = failure_index;
        }

        int get_failure(int i) {
            return failure[i];
        }

        int kmp_match(char T_j, int j) {
            int result = -1;
            while (index > -1 && P[index + 1] != T_j) index = failure[index];
            if (P[index + 1] == T_j) index++;
            if (index == m - 1) {
                result = j;
                index = failure[index];
            }
            return result;
        }
} ;

#endif
