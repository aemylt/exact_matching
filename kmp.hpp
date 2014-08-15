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

        void set_failure(int i, int val) {
            failure[i] = val;
        }

        char get_P(int i) {
            return P[i];
        }

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
                while (failure_index > -1 && get_P(failure_index + 1) != get_P(j)) failure_index = get_failure(failure_index);
                if (get_P(failure_index + 1) == get_P(j)) failure_index++;
                set_failure(j, failure_index);
            }
        }

        ~kmp_stream() {
            free(failure);
        }

        void update_pattern(char P_j) {
            m++;
            P += P_j;
            failure = (int*)realloc(failure, m * sizeof(int));
            while (failure_index > -1 && get_P(failure_index + 1) != P_j) failure_index = get_failure(failure_index);
            if (get_P(failure_index + 1) == P_j) failure_index++;
            set_failure(m - 1, failure_index);
        }

        int get_failure(int i) {
            return failure[i];
        }

        int kmp_match(char T_j, int j) {
            int result = -1;
            while (index > -1 && get_P(index + 1) != T_j) index = get_failure(index);
            if (get_P(index + 1) == T_j) index++;
            if (index == m - 1) {
                result = j;
                index = get_failure(index);
            }
            return result;
        }
} ;

#endif
