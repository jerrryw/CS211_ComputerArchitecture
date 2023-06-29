#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct s_matrix{
    int *data;
    int N;
} Matrix;

void printMatrix(Matrix* matrix){
    int i, j, N = matrix->N;
    for(i = 0; i < N; ++i){
        for(j = 0; j < N; ++j){
            // printf("%d ", matrix[i][j]);
            printf("%d\t", matrix->data[i * N + j]);
        }
        printf("\n");
    }
}

void getSubmatrix(Matrix* matrix, Matrix* submatrix, int col){
    assert(matrix->data != NULL);
    assert(submatrix->data != NULL);

    int count = 0;
    int i, j, N = matrix->N;
    for(i = 1; i < N; ++i){
        for(j = 0; j < N; ++j){
            if (col == j) { continue; } // skip col
            submatrix->data[count++] = matrix->data[i * N + j];
        }
    }
    assert(count == (N-1)*(N-1));
}

int determinant(Matrix* matrix){
    assert(matrix->N >= 1);
    assert(matrix->data != NULL);
    if (matrix->N == 1) return matrix->data[0];
    if (matrix->N == 2) {
        int a = matrix->data[0];
        int b = matrix->data[1];
        int c = matrix->data[2];
        int d = matrix->data[3];
        return a * d - b * c;
    }

    int sum = 0;
    int sign = 1;
    int j, N = matrix->N;
    Matrix submatrix; 
    submatrix.N = N - 1;
    submatrix.data = (int*) malloc(submatrix.N * submatrix.N * sizeof(int));

    for (j = 0; j < N; ++j) {
        getSubmatrix(matrix, &submatrix, j);
        // printf("%+d * %d\n", sign, matrix->data[0 * N + j]);
        // printMatrix(&submatrix);
        // printf("\n\n");
        sum += sign * matrix->data[0 * N + j] * determinant(&submatrix);
        sign = -sign;
    }

    free(submatrix.data);
    return sum;
}


int main(int argc, char* argv[argc + 1]){
    FILE* file = fopen(argv[1], "r");
    if (!file){
        return EXIT_SUCCESS;
    }

    int N;
    fscanf(file, "%d", &N);

    Matrix matrix;
    matrix.data = (int*) malloc(N * N * sizeof(int));
    matrix.N = N;
    int i;
    for (i = 0; i < N * N; i++) {
        fscanf(file, "%d", &matrix.data[i]);
        // matrix[i][j] = matrix[i*N+j];
    }

    //printMatrix(&matrix);
    printf("%d\n", determinant(&matrix));

    matrix.N = 0;
    free(matrix.data);
    return EXIT_SUCCESS;
}