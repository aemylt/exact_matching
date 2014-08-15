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
        int period_len;
        char period_break;
        char break_failure;
        bool has_break;

        char get_P(int i) {
            if (i < period_len) return P[i];
            if ((i == m - 1) && (has_break)) return period_break;
            return P[i % period_len];
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
            period_len = m;
            has_break = false;
            for (j = 1; j < P_len; j++) {
                while (failure_index > -1 && get_P(failure_index + 1) != get_P(j)) failure_index = get_failure(failure_index);
                if (get_P(failure_index + 1) == get_P(j)) failure_index++;
                failure[j] = failure_index;
            }
            if ((failure[P_len - 1] + 1) << 1 >= P_len) {
                period_len = (P_len - failure[P_len - 1] - 1) << 1;
                P = Pattern.substr(0, period_len);
                failure = (int*)realloc(failure, period_len * sizeof(int));
            }
        }

        ~kmp_stream() {
            free(failure);
        }

        void update_pattern(char P_j) {
            m++;
            while (failure_index > -1 && get_P(failure_index + 1) != P_j) failure_index = get_failure(failure_index);
            if (get_P(failure_index + 1) == P_j) failure_index++;
            if ((failure_index + 1) << 1 < m) {
                has_break = true;
                period_break = P_j;
                break_failure = failure_index;
            }
        }

        int get_failure(int i) {
            if (i < period_len) return failure[i];
            if ((i == m - 1) && (has_break)) return break_failure;
            return failure[period_len - 1] + i - period_len + 1;
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
