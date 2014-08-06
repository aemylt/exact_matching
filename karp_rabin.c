#include "karp_rabin.h"
#include <gmp.h>

int main(void) {
    int n = 1000000;
    fingerprinter printer = fingerprinter_build(n);
    gmp_printf("%Zd\n", printer->p);
    gmp_printf("%Zd\n", printer->r);

    return 0;
}