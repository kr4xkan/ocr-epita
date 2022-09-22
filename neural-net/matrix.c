#include "matrix.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void mat_randomize(float *mat, size_t len) {
    for (size_t i = 0; i < len; i++) {
        mat[i] = ((float)rand() / (float)RAND_MAX) * 2 - 1;
    }
}

void mat_print(float *mat, size_t row, size_t col) {
    for (size_t c = 0; c < row; c++) {
        for (size_t r = 0; r < col; r++) {
            printf("%9.3f", mat[r * col + c]);
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

void mat_multiply_scalar(float *res, float *a, float b, size_t n, size_t m) {
    for (size_t c = 0; c < m; c++) {
        for (size_t r = 0; r < n; r++) {
            res[r*m+c] = a[r*m+c] * b;
        }
    }
}

void mat_add(float *res, float *a, float *b, size_t n, size_t m) {
    for (size_t c = 0; c < m; c++) {
        for (size_t r = 0; r < n; r++) {
            res[r*m+c] = a[r*m+c] + b[r*m+c];
        }
    }
}

void mat_substract(float *res, float *a, float *b, size_t n, size_t m) {
    for (size_t c = 0; c < m; c++) {
        for (size_t r = 0; r < n; r++) {
            res[r*m+c] = a[r*m+c] - b[r*m+c];
        }
    }
}

void mat_apply_sigmoid(float *res, size_t n) {
    for (size_t r = 0; r < n; r++) {
        float d = res[r];
        res[r] = 1 / (1 + expf(-d));
    }
}

void mat_apply_softmax(float *res, size_t n) {
    float sum = 0;
    for (size_t r = 0; r < n; r++) {
        sum += expf(res[r]);
    }
    for (size_t r = 0; r < n; r++) {
        res[r] = expf(res[r]) / sum;
    }
}

void test_matrix() {
    float a[] = {1, 5, 7, 1, 9, 3, 2, 1, 2, 5, 7, 6};
    float b[] = {8, 5, 1, 3, 9, 3, 9, 1, 8, 1, 3, 5};
    float c[] = {8, 3, 9, 1, 5, 9, 1, 3, 1, 3, 8, 5};
    size_t n = 4;
    size_t m = 3;

    float r_add[] = {9, 10, 8, 4, 18, 6, 11, 2, 10, 6, 10, 11};
    float r_sub[] = {-7, 0, 6, -2, 0, 0, -7, 0, -6, 4, 4, 1};
    float r_mul[] = {40, 69, 70, 51, 56, 93, 42,  43,
                     23, 21, 35, 15, 81, 96, 100, 56};
    float r_mul_sc[] = {3, 15, 21, 3, 27, 9, 6, 3, 6, 15, 21, 18};

    float t_add[12];
    mat_add(t_add, a, b, n, m);
    for (int i = 0; i < 12; i++) {
        if (t_add[i] != r_add[i]) {
            printf("add %d: %f != %f\n", i, t_add[i], r_add[i]);
        }
    }

    float t_sub[12];
    mat_substract(t_sub, a, b, n, m);
    for (int i = 0; i < 12; i++) {
        if (t_sub[i] != r_sub[i]) {
            printf("sub %d: %f != %f\n", i, t_sub[i], r_sub[i]);
        }
    }

    float t_mul[16];
    mat_multiply(t_mul, a, c, n, m, n);
    for (int i = 0; i < 16; i++) {
        if (t_mul[i] != r_mul[i]) {
            printf("mul %d: %f != %f\n", i, t_mul[i], r_mul[i]);
        }
    }

    float t_mul_sc[12];
    mat_multiply_scalar(t_mul_sc, a, 3, n, m);
    for (int i = 0; i < 12; i++) {
        if (t_mul_sc[i] != r_mul_sc[i]) {
            printf("mul_sc %d: %f != %f\n", i, t_mul_sc[i], r_mul_sc[i]);
        }
    }
}
