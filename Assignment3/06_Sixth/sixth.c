#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef          long int sint64;
typedef unsigned long int uint64;

#ifdef DEBUG
    #define dprintf printf
#else
    #define dprintf if(0) printf
#endif

#define MASK64(bits) ((~0lu) >> (64-(bits)))
#define BIT64(bits) (1lu << (bits))

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
    int bits_expo;
    int bits_frac;
    uint64 hex;
    int digits;

    int ret;
    while(1){
        ret = fscanf(file, "%d %d %d %lx %d", &bits, &bits_expo, &bits_frac, &hex, &digits);
        if(ret == EOF) break;
        assert(ret == 5);

        uint64 sign = hex;  // ??????????????????SEEEEFFFFFFF
        sign >>= (bits_expo + bits_frac);
        sign &= MASK64(1);
        
        int bias_given         = MASK64(bits_expo - 1);
        uint64 expo = hex;  // ??????????????????SEEEEFFFFFFF
        expo >>= bits_frac;
        expo &= MASK64(bits_expo);

        sint64 expo_decoded = expo - bias_given;
        
        uint64 frac = hex;  // ??????????????????SEEEEFFFFFFF
        frac &= MASK64(bits_frac);

        dprintf("==========================\n");
        dprintf("%d %d %d %lx %d\n", bits, bits_expo, bits_frac, hex, digits);
        dprintf("sign: 0x%016lX\n", sign);
        dprintf("expo: 0x%016lX\n", expo);
        dprintf("expo_decoded: 0x%016lX = 2 ^ %ld\n", expo_decoded, expo_decoded);
        dprintf("frac: 0x%016lX\n", frac);

        // 8 4 3 0x4d 2
        // 0 1001 010 == 1.100 * 2 ^ 2
        double answer = 1.0;
        if (expo == 0) {
            // denormalized
            answer = 0.0;
            expo_decoded = -(bias_given-1);
        }
        double base = 0.5;
        for (int i = bits_frac-1; i >= 0; --i) {
            if (frac & BIT64(i)) {
                answer += base;
            }
            base /= 2.0;
        }

        // expo = 3 or -3
        if (expo_decoded > 0) {
            for (int i = 0; i < expo_decoded; ++i) {
                answer *= 2.0;
            }
        } else { 
            for (int i = 0; i < -expo_decoded; ++i) {
                answer /= 2.0;
            }
        }

        if (sign) {
            answer = -answer;
        }

        printf("%.*lf\n", digits, answer);
    }
    fclose(file);
    return EXIT_SUCCESS;
}
