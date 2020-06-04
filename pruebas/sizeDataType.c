#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>

int main(int argc, char** argv) 
{
    long long v = 9000000000000000000;
    printf("%llu\n", v);
    printf("SIZE_CHAR   :   %d byte\n", sizeof(char));
    printf("SIZE_SHORT  :   %d bytes\n", sizeof(short));
    printf("SIZE_INT    :   %d bytes\n", sizeof(int));
    printf("SIZE_LONG   :   %d bytes\n", sizeof(long long));
    printf("SIZE_FLOAT  :   %d bytes\n", sizeof(float));
    printf("SIZE_DOUBLE :   %d bytes\n", sizeof(double));
    printf("SIZE_LDOUBLE:   %d bytes\n", sizeof(long double));
    printf("CHAR_BIT    :   %d\n", CHAR_BIT);
    printf("CHAR_MAX    :   %d\n", CHAR_MAX);
    printf("CHAR_MIN    :   %d\n", CHAR_MIN);
    printf("INT_MAX     :   %d\n", INT_MAX);
    printf("INT_MIN     :   %d\n", INT_MIN);
    printf("LONG_MAX    :   %ld\n", (long) LONG_MAX);
    printf("LONG_MIN    :   %ld\n", (long) LONG_MIN);
    printf("SCHAR_MAX   :   %d\n", SCHAR_MAX);
    printf("SCHAR_MIN   :   %d\n", SCHAR_MIN);
    printf("SHRT_MAX    :   %d\n", SHRT_MAX);
    printf("SHRT_MIN    :   %d\n", SHRT_MIN);
    printf("UCHAR_MAX   :   %d\n", UCHAR_MAX);
    printf("UINT_MAX    :   %u\n", (unsigned int) UINT_MAX);
    printf("ULONG_MAX   :   %lu\n", (unsigned long) ULONG_MAX);
    printf("USHRT_MAX   :   %d\n", (unsigned short) USHRT_MAX);

    return 0;
}