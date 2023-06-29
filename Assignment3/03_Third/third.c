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
    char from[2];
    char to[2];
    while(1){
        ret = fscanf(file, "%ld %d %s %s", &decimal, &bits, from, to);
        if(ret == EOF) break;
        assert(ret == 4);

#ifdef DEBUG
        printf("==============================\n");
#endif

        // preprocessing
        sint64 num = decimal;
#ifdef DEBUG
        printf("decimal : %ld\n", decimal);
        printf("   bits : %d\n", bits);
        printf("    num : %ld\n", num);
        printf("    max : %ld\n", MAX_SVAL(bits));
        printf("    min : %ld\n", MIN_SVAL(bits));
        printf("     to : %s\n", to);
#endif
        // sint64 snum;
        // uint64 unum;
        // sint64 answer = 0;
        // if (to[0] == 'u') {
        //     unum = num;
        //     unum &= MAX_UVAL(bits);
        //     answer = unum;
        // } else if (to[0] == 's') {
        //     snum = num;
        //     snum &= MAX_UVAL(bits);
        //     snum <<= 64-(bits);
        //     snum >>= 64-(bits);
        //     answer = snum;
        // }

        sint64 answer = num;  //unum, snum
        answer &= MAX_UVAL(bits);
        if (to[0] == 's') {
            answer = answer << (64-(bits)) >> (64-(bits));
        }
#ifdef DEBUG
        printf(" answer : %ld\n", answer);
#endif

        printf("%ld\n", answer);
    }
    fclose(file);
    return EXIT_SUCCESS;
}
