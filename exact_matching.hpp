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
};

void shift_row(fingerprinter& printer, pattern_row& P_i, fingerprint& tmp) {
    if (P_i.count <= 2) {
        P_i.VOs[0].T_f = P_i.VOs[1].T_f;
        P_i.VOs[0].location = P_i.VOs[1].location;
    } else {
        P_i.VOs[0].T_f.concat(printer, P_i.period_f, tmp);
        P_i.VOs[0].T_f = tmp;
        P_i.VOs[0].location += P_i.period;
    }
    P_i.count--;
}

void add_occurance(fingerprinter& printer, fingerprint& T_f, int location, pattern_row& P_i, fingerprint& tmp) {
    if (P_i.count < 2) {
        P_i.VOs[P_i.count].T_f = T_f;
        P_i.VOs[P_i.count].location = location;
        P_i.count++;
    } else {
        if (P_i.count == 2) {
            P_i.period = P_i.VOs[1].location - P_i.VOs[0].location;
            P_i.VOs[1].T_f.suffix(printer, P_i.VOs[0].T_f, P_i.period_f);
        }
        T_f.suffix(printer, P_i.VOs[1].T_f, tmp);
        int period = location - P_i.VOs[1].location;
        if ((period == P_i.period) && (P_i.period_f == tmp)) {
            P_i.VOs[1].T_f = T_f;
            P_i.VOs[1].location = location;
            P_i.count++;
        } else printf("Warning: Error in Period occured at location %d. VO discarded.\n", location);
    }
}

int fingerprint_match(string T, string P, int alpha, int *results) {
    int lm = 0, f = 0, i, j, matches = 0;
    while ((1 << lm) <= (int)P.size()) lm++;
    while ((1 << f <= lm)) f++;
    lm -= f + 1;
    kmp_stream P_f(P, 1 << f);
    fingerprinter printer(T.size(), alpha);
    fingerprint T_f, T_cur, tmp;
    pattern_row *P_i = new pattern_row[lm];
    j = 1 << f;
    for (i = 0; i < lm - 1; i++) {
        P_i[i].row_size = j;
        P_i[i].P.set(printer, &P[j], j);
        j <<= 1;
    }
    P_i[lm - 1].P.set(printer, &P[j], P.size() - j);
    P_i[lm - 1].row_size = (int)P.size() - (1 << (lm - 1 + f));

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
                if (j == lm - 1) {
                    results[matches++] = P_i[j].VOs[0].location + P_i[j].row_size + 1;
                }
                else add_occurance(printer, T_cur, P_i[j].VOs[0].location + P_i[j].row_size, P_i[j + 1], tmp);
            }
            shift_row(printer, P_i[j], tmp);
        }
        if (P_f.kmp_match(T[i], i) != -1) {
            add_occurance(printer, past_prints[j], i, P_i[0], tmp);
        }
        if (++j == lm) j = 0;
    }

    while (j < lm) {
        while ((P_i[j].count > 0) && (i - P_i[j].VOs[0].location >= P_i[j].row_size)) {
            T_cur = past_prints[(P_i[j].VOs[0].location + P_i[j].row_size) % lm];
            T_cur.suffix(printer, P_i[j].VOs[0].T_f, T_f);

            if (P_i[j].P == T_f) {
                if (j == lm - 1) {
                    results[matches++] = P_i[j].VOs[0].location + P_i[j].row_size + 1;
                }
                else add_occurance(printer, T_cur, P_i[j].VOs[0].location + P_i[j].row_size, P_i[j + 1], tmp);
            }
            shift_row(printer, P_i[j], tmp);
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
