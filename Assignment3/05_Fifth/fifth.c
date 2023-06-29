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
    
    double num;
    int bits;
    int bits_expo;
    int bits_frac;
    int ret;
    while(1){
        ret = fscanf(file, "%lf %d %d %d", &num, &bits, &bits_expo, &bits_frac);
        dprintf("========================\n");
        dprintf("%lf %d %d %d\n", num, bits, bits_expo, bits_frac);
        if(ret == EOF) break;
        assert(ret == 4);

        uint64* punum = (uint64*)&num;
        uint64 unum = *punum;
        uint64 sign = unum >> 63; // 0, 1
        uint64 expo = ((unum << 1) >> (64 - 11));
        uint64 frac = unum & MASK64(52); // b'1001000000   

        sint64 expo_decoded = expo - MASK64(11-1); // decode
        dprintf("2 ^ %ld\n", expo_decoded);
        uint64 expo_new = expo_decoded + MASK64(bits_expo-1); // 4 bits encode

        /*
        round to decimal bits_frac=3 bits

             [51] [50] [49]|[48] [47]  .... [1] [0]
        frac   1    0    1 |  1    0   ....  0   0
         mid   1    0    1 |  1    0   ....  0   0

        frac   1    0    0 |  1    0   ....  0   0
         mid   1    0    0 |  1    0   ....  0   0
         
        frac   1    1    0 |  0    1   ....  0   1
         mid   1    1    0    0    1   ....  0   1   // mid = frac;
         mid   1    1    0    0    0   ....  0   0   // mid &= ~MASK64(52-bits_frac);
         mid   1    1    0    1    0   ....  0   0   // mid |= BIT64(52-bits_frac-1);

        frac   1    1    1
        frac 1 0    0    0
        */
        int isRoundingUp = 0;
        uint64 mid = frac;
        mid &= ~MASK64(52-bits_frac);
        mid |= BIT64(52-bits_frac-1);

        if (frac > mid) {
            isRoundingUp = 1;
        } else if (frac == mid && (frac & BIT64(52-bits_frac))) {
            isRoundingUp = 1;
        }

        frac >>= 52 - bits_frac;
        dprintf("isRoundingUp: %d\n", isRoundingUp);
        dprintf("expo_new: 0x%016lX\n", expo_new);
        dprintf("frac: 0x%016lX\n", frac);
        if (isRoundingUp) {
            if (frac == MASK64(bits_frac)) {
                frac = 0;
                expo_new += 1;
            } else {
                frac += 1;
            }
            dprintf("expo_new: 0x%016lX\n", expo_new);
            dprintf("frac: 0x%016lX\n", frac);
        }

        // S [bits_expo] [bits_frac]
        // 000000000000S 00000S0000000
        // 000000000EEEE 000000EEEE000
        // 0000000000FFF 0000000000FFF
        //               00000SEEEEFFF
        expo_new &= MASK64(bits_expo);
        frac &= MASK64(bits_frac);
        unum = (sign << (bits-1)) | (expo_new << bits_frac) | (frac);

        char str[65];
        str[64] = 0;
        for (int i = 64-1; i >= 0; --i) {
            str[i] = (unum & 1) + '0';
            unum >>= 1;
        }
        
        printf("%s\n", &str[64-bits]);
    }
    fclose(file);
    return EXIT_SUCCESS;
}
