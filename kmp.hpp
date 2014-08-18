#ifndef KMP
#define KMP
#include <stdlib.h>
#include <string>

using namespace std;

/*
    class kmp_stream
    Class for streaming KMP.
    Components:
        string P             - Pattern
        int    m             - Length of pattern
        int    index         - Current index of pattern
        int    *failure      - Failure table
        int    failure_index - Current index of failure table
        int    period_len    - Length of period
        char   period_break  - Value of last character in pattern
        int    break_failure - Failure value of last character in pattern
        bool   has_break     - true if the period breaks, false otherwise
*/
class kmp_stream {
        string P;
        int m;
        int index;
        int* failure;
        int failure_index;
        int period_len;
        char period_break;
        int break_failure;
        bool has_break;

/*
    kmp_stream::char get_P(int i)
    Gets the character at index i.
    Parameters:
        int i - The index of the pattern
    Returns char:
        P[i]
*/
        char get_P(int i) {
            if (i < period_len) return P[i];
            if ((i == m - 1) && (has_break)) return period_break;
            return P[i % period_len];
        }

    public:
/*
    kmp_stream::kmp_stream(string Pattern, int P_len)
    Constructor.
    Parameters:
        string Pattern - The pattern
        int    P_len   - Length of the pattern
*/
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

/*
    kmp_stream::~kmp_stream()
    Destructor.
*/
        ~kmp_stream() {
            free(failure);
        }

/*
    kmp_stream::void update_pattern(char P_j)
    Updates the pattern. To be used if there is a period.
    Parameters:
        char P_j - Next character in the pattern.
*/
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

/*
    kmp_stream::int get_failure(int i)
    Gets the failure point at index i.
    Parameters:
        int i - The index
    Returns int:
        failure[i]
*/
        int get_failure(int i) {
            if (i < period_len) return failure[i];
            if ((i == m - 1) && (has_break)) return break_failure;
            return failure[period_len - 1] + i - period_len + 1;
        }

/*
    kmp_stream::int kmp_match(char T_j, int j)
    Performs KMP matching on next character.
    Parameters:
        char T_j - Next character in the text
        int  j   - Index of next character
    Returns int:
        j if there is a match
        -1 otherwise
*/
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
