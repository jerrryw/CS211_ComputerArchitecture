#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define max(a,b) (((a)>(b))?(a):(b))

void reverseStr(char* str){
    int size = strlen(str);
    char temp;
    for (int i = 0; i < size/2; ++i) {
        temp = str[i];
        str[i] = str[size - 1 - i];
        str[size - 1 - i] = temp;
    }
}

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
    
    unsigned long int decimal;
    int bits;
    int ret;
    while(1){
        ret = fscanf(file, "%lu %d", &decimal, &bits);
        if(ret == EOF) break;
        assert(ret == 2);

        char binary[65] = {'\0'};
        int i = 0;
        unsigned long long int num = decimal;
        while (num > 0) {
            binary[i] = (num % 2) + '0';
            num = num / 2;
            i++;
        }
        binary[i] = '\0';
        if (decimal == 0) {
            binary[0] = '0';
            i = 1;
        }

        reverseStr(binary);
        char padding[] = "0000" "0000" "0000" "0000" "0000" "0000" "0000" "0000"
            "0000" "0000" "0000" "0000" "0000" "0000" "0000" "0000";
            
        //printf("%llu %d\t", decimal, bits);
        // printf("%.*s | %s\n", max(0, bits - i), padding, &binary[max(0, i - bits)]);
        printf("%.*s%s\n", max(0, bits - i), padding, &binary[max(0, i - bits)]);
    }
    fclose(file);
    return EXIT_SUCCESS;
}
