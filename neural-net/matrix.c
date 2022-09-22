#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

void mat_randomize(float *mat, size_t len) {
    for (size_t i = 0; i < len; i++) {
        mat[i] = ((float)rand() / (float)RAND_MAX) * 2 - 1;
    }
}

void mat_print(float *mat, size_t row, size_t col) {
    for (size_t c = 0; c < col; c++) {
        for (size_t r = 0; r < row; r++) {
            printf("%9.3f", mat[c * row + r]);
        }
        printf("\n");
    }
}

void mat_multiply(float *res, float *a, float *b, size_t n, size_t m,
                  size_t p) {
    for (size_t c = 0; c < p; c++) {
        for (size_t r = 0; r < n; r++) {
            float sum = 0;
            for (size_t k = 0; k < m; k++) {
                sum += a[r*m+k] * b[k*p+c];
            }
            res[r * p + c] = sum;
        }
    }
}
