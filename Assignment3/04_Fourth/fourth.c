#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef          long int sint64;
typedef unsigned long int uint64;

int main(int argc, char* argv[argc + 1]){
    if (argc < 2) {
        fprintf(stderr, "Usage:\n\t%s <test.txt>\n", argv[0]);
        return EXIT_SUCCESS;
    }
    FILE* file = fopen(argv[1], "rt");
    if (!file){
        fprintf(stderr, "Failed to open file \"%s\"\n", argv[1]);
        return EXIT_SUCCESS;
    }
    
    int bits;
    int ret;
    double num;
    while(1){
        ret = fscanf(file, "%lf %d", &num, &bits);
        if(ret == EOF) break;
        // assert(ret == 2);

        uint64* punum = (uint64*)&num;
        uint64 unum = *punum;
        uint64 sign = unum >> 63; // 0, 1
        uint64 expo = ((unum << 1) >> (64 - 11));
        uint64 frac = unum << 12; // b'1001000000   

        // print sign 1/0
        if (sign == 1) {
            printf("-");
        }

        // 1.0 fraction
        char binary[65];
        for (int i=0; i<65; ++i) {
            binary[i] = '0';
        }
        binary[64] = '\0';

        // algorithm
        uint64 frac2 = frac; // 52 bits
#ifdef  DEBUG
    printf("Frac2: 0x%016lX\n", frac2);
#endif
        for (int i=0; i<64; i++) {
            binary[63-i] = (frac2 & 1) + '0';
            frac2 = frac2 >> 1;
        }
#ifdef  DEBUG
    printf("Frac: 0x%016lX\n", frac);
    printf("Binary: %s\n", binary);
#endif
        printf("1.%.*s", bits, binary);

        // 2 ^ ?
        sint64 expo2 = ((sint64)expo);
        expo2 = expo2 - (1024 - 1);
        printf(" %ld\n", expo2);


#ifdef DEBUG
        printf("==============================\n");
        printf(" binary : %s\n", binary);
        printf("   bits : %d\n", bits);
#endif
    }
    fclose(file);
    return EXIT_SUCCESS;
}
