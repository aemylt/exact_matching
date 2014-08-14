#include "karp_rabin.hpp"
#include <gmp.h>
#include <stdio.h>
#include <assert.h>

int main(void) {
    int n = 100;
    fingerprinter printer(n, 0);
    gmp_printf("p = %Zd\n", printer.p);
    gmp_printf("r = %Zd\n", printer.r);

    fingerprint print;
    print.set(printer, "aaaaabbbbbcccccaaaaa", 20);

    fingerprint prefix;
    prefix.set(printer, "aaaaa", 5);

    fingerprint v;
    print.suffix(printer, prefix, v);

    fingerprint suffix;
    suffix.set(printer, "bbbbbcccccaaaaa", 15);
    assert(v == suffix);

    fingerprint u;
    print.prefix(printer, suffix, u);
    assert(u == prefix);

    fingerprint uv;
    prefix.concat(printer, suffix, uv);
    assert(uv == print);

    fingerprint empty;
    print.suffix(printer, empty, v);
    assert(v == print);

    print.prefix(printer, empty, v);
    assert(v == print);

    print.concat(printer, empty, v);
    assert(v == print);

    print.prefix(printer, print, u);
    assert(u == empty);

    print.suffix(printer, print, v);
    assert(v == empty);

    empty.concat(printer, print, v);
    assert(v == print);

    fingerprint nostring;
    nostring.set(printer, "bbbbbcccccaaaaa", 0);
    assert(nostring == empty);

    return 0;
}