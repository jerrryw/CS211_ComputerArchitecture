#include <stdio.h>
#include <stdlib.h>
#define bool int
#define true (1)
#define false (0)

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n > 2 && n % 2 == 0) return false;
    if (n > 3 && n % 3 == 0) return false;
    int i;
    for (i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}


bool isTwinPrime(int n) {
    // printf("isPrime(%d) == %d\n", n, isPrime(n));
    // printf("isPrime(%d) == %d\n", n-2, isPrime(n-2));
    // printf("isPrime(%d) == %d\n", n+2, isPrime(n+2));
    return isPrime(n) && (isPrime(n-2) || isPrime(n+2));
}

int main(int argc, char* argv[argc + 1]){
    FILE* file = fopen(argv[1], "r");
    if (!file){
        fprintf(stderr, "Failed to open file \"%s\"\n", argv[1]);
        return EXIT_SUCCESS;
    }
    
    int value;
    int ret;
    while(1){
        ret = fscanf(file, "%d", &value);
        // printf("value == %d\n", value);
        if (ret == EOF) break;
        if (isTwinPrime(value)){
            printf("yes\n");
        } else {
            printf("no\n");
        }
    }
    return EXIT_SUCCESS;
}
