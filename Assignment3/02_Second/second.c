#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define max(a,b) (((a)>(b))?(a):(b))

typedef          long int sint64;
typedef unsigned long int uint64;

#define MAX_SVAL(bits) (sint64)(( ( 1llu<<( (bits) - 1) ) - 1 ))
#define MIN_SVAL(bits) (sint64)(~(MAX_SVAL(bits)))

#define MAX_UVAL(bits) ((~0llu)>>(64-(bits)))
#define MIN_UVAL(bits) (0llu)

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
    
    sint64 decimal;
    int bits;
    int ret;
    while(1){
        ret = fscanf(file, "%ld %d", &decimal, &bits);
        if(ret == EOF) break;
        assert(ret == 2);

        char binary[65];
        for (int i=0; i<65; ++i) {
            binary[i] = '0';
        }
        binary[64] = '\0';
#ifdef DEBUG
        printf("==============================\n");
        printf(" binary : %s\n", binary);
#endif

        // preprocessing
        sint64 num = decimal;
#ifdef DEBUG
        printf("decimal : %ld\n", decimal);
        printf("   bits : %d\n", bits);
        printf("    num : %ld\n", num);
        printf("    max : %ld\n", MAX_SVAL(bits));
        printf("    min : %ld\n", MIN_SVAL(bits));
#endif
        if (num > MAX_SVAL(bits)) {
            num = MAX_SVAL(bits);
        } else if (num < MIN_SVAL(bits)){
            num = MIN_SVAL(bits);
        }
#ifdef DEBUG
        printf("    num : %ld\n", num);
#endif

        // algorithm
        for (int i=0; i<64; i++) {
            binary[63-i] = (num & 1) + '0';
            num = num >> 1;
        }
#ifdef DEBUG
        printf(" binary : %s\n", &binary[64-bits]);
#endif
        printf("%s\n", &binary[64-bits]);
    }
    fclose(file);
    return EXIT_SUCCESS;
}
