#ifndef EXACT_MATCHING
#define EXACT_MATCHING

#include "karp_rabin.hpp"
#include "kmp.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

struct viable_occurance {
    int location;
    fingerprint T_f;
    viable_occurance() : location(0), T_f() {}
};

struct pattern_row {
    int row_size, period, count;
    fingerprint P, period_f;
    viable_occurance VOs[2];
    pattern_row() : row_size(0), period(0), count(0), P(), period_f() { }

    void shift_row(fingerprinter& printer, fingerprint& tmp) {
        if (count <= 2) {
            VOs[0].T_f = VOs[1].T_f;
            VOs[0].location = VOs[1].location;
        } else {
            VOs[0].T_f.concat(printer, period_f, tmp);
            VOs[0].T_f = tmp;
            VOs[0].location += period;
        }
        count--;
    }

    void add_occurance(fingerprinter& printer, fingerprint& T_f, int location, fingerprint& tmp) {
        if (count < 2) {
            VOs[count].T_f = T_f;
            VOs[count].location = location;
            count++;
        } else {
            if (count == 2) {
                period = VOs[1].location - VOs[0].location;
                VOs[1].T_f.suffix(printer, VOs[0].T_f, period_f);
            }
            T_f.suffix(printer, VOs[1].T_f, tmp);
            int period = location - VOs[1].location;
            if ((period == period) && (period_f == tmp)) {
                VOs[1].T_f = T_f;
                VOs[1].location = location;
                count++;
            } else printf("Warning: Error in Period occured at location %d. VO discarded.\n", location);
        }
    }

};

int fingerprint_match(string T, string P, int alpha, int *results) {
    int lm = 0, f = 0, i = 0, j, matches = 0;
    while ((1 << lm) <= (int)P.size()) lm++;
    while ((1 << f <= lm)) f++;
    lm -= f + 1;
    j = 1 << f;
    kmp_stream P_f(P, j);
    while ((j < (int)P.size()) && ((P_f.get_failure(j - 1) + 1) << 1 >= j)) {
        P_f.update_pattern(P[j]);
        j++;
    }

    if (j == (int)P.size()) {
        for (i = 0; i < (int)T.size(); i++) if (P_f.kmp_match(T[i], i) != -1) results[matches++] = i;
        return matches;
    }

    fingerprinter printer(T.size(), alpha);
    fingerprint T_f, T_cur, tmp;
    pattern_row *P_i = new pattern_row[lm];
    while (j << 2 < (int)P.size()) {
        P_i[i].row_size = j;
        P_i[i].P.set(printer, &P[j], j);
        j <<= 1;
        i++;
    }
    P_i[i].P.set(printer, &P[j], P.size() - j);
    P_i[i].row_size = (int)P.size() - j;
    lm = i + 1;

    fingerprint *past_prints = new fingerprint[lm];
    j = 0;

    for (i = 0; i < (int)T.size(); i++) {
        T_cur.set(printer, &T[i], 1);
        past_prints[(j) ? j - 1 : lm - 1].concat(printer, T_cur, tmp);
        past_prints[j] = tmp;

        while ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location >= P_i[j].row_size)) {
            T_cur = past_prints[(P_i[j].VOs[0].location + P_i[j].row_size) % lm];
            T_cur.suffix(printer, P_i[j].VOs[0].T_f, T_f);

            if (P_i[j].P == T_f) {
                if (j == lm - 1) results[matches++] = P_i[j].VOs[0].location + P_i[j].row_size;
                else P_i[j + 1].add_occurance(printer, T_cur, P_i[j].VOs[0].location + P_i[j].row_size, tmp);
            }
            P_i[j].shift_row(printer, tmp);
        }
        if (P_f.kmp_match(T[i], i) != -1) {
            P_i[0].add_occurance(printer, past_prints[j], i, tmp);
        }
        if (++j == lm) j = 0;
    }

    while (j < lm) {
        while ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location >= P_i[j].row_size)) {
            T_cur = past_prints[(P_i[j].VOs[0].location + P_i[j].row_size) % lm];
            T_cur.suffix(printer, P_i[j].VOs[0].T_f, T_f);

            if (P_i[j].P == T_f) {
                if (j == lm - 1) results[matches++] = P_i[j].VOs[0].location + P_i[j].row_size;
                else P_i[j + 1].add_occurance(printer, T_cur, P_i[j].VOs[0].location + P_i[j].row_size, tmp);
            }
            P_i[j].shift_row(printer, tmp);
        }
        j++;
    }

    delete[] P_i;
    delete[] past_prints;

    return matches;
}

int fingerprint_match_naive(string T, string P, int alpha, int *results) {
    int count = 0, i;
    fingerprinter printer(T.size(), alpha);
    fingerprint T_f, P_f, T_i, T_m, tmp;
    int size = T.size() - P.size();
    P_f.set(printer, P, P.size());
    T_f.set(printer, T, P.size());
    if (P_f == T_f) results[count++] = 0;
    for (i = 0; i < size; i++) {
        T_i.set(printer, &T[i], 1);
        T_m.set(printer, &T[i + P.size()], 1);
        T_f.suffix(printer, T_i, tmp);
        tmp.concat(printer, T_m, T_f);
        if (P_f == T_f) results[count++] = i + 1;
    }
    results = (int*)realloc(results, count * sizeof(int));
    return count;
}

#endif
