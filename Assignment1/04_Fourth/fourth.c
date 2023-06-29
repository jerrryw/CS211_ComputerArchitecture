#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void print_matrix(int* square, int n){
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (j != 0) printf("\t");
            printf("%d", *(square + i*n + j));
        }
        printf("\n");
    }
}

void build(int n){
    int i, j, counter;
    int square[n][n];

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            square[i][j] = 0;
        }
    }

    i = 0;
    j = n/2;
    counter = 1;

    while (1) {
        square[i][j] = counter++;
        if (counter > n * n) break;

        int i_next = (i+n-1) % n; // move up
        int j_next = (j+1) % n; // move right

        if (square[i_next][j_next]) {
            i_next = (i+1) % n;
            j_next = j;
        }

        i = i_next;
        j = j_next;
    }
    print_matrix(&square[0][0], n);
}

int main(int argc, char* argv[argc + 1]){
    int n = atoi(argv[1]);
    if (n % 2 == 0) {
        printf("error");
        return EXIT_SUCCESS;
    }
    build(n);
    return EXIT_SUCCESS;
}