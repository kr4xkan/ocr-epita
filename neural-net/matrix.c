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
    for (size_t r = 0; r < row; r++) {
        for (size_t c = 0; c < col; c++) {
            printf("%9.3f", mat[r * col + c]);
        }
        printf("\n");
    }
}

void mat_copy(float *src, float *dest, size_t len) {
    for (size_t i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}

void mat_multiply(float *res, float *a, float *b, size_t m, size_t n,
                  size_t p) {
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < p; j++) {
            float sum = 0;
            for (size_t k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * p + j];
            }
            res[i * p + j] = sum;
        }
    }
}

void mat_multiply_scalar(float *res, float *a, float b, size_t m, size_t n) {
    for (size_t c = 0; c < n; c++) {
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = a[r * n + c] * b;
        }
    }
}

void mat_multiply_hadamard(float *res, float *a, float *b, size_t m, size_t n) {
    for (size_t c = 0; c < n; c++) {
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = a[r * n + c] * b[r * n + c];
        }
    }
}

void mat_add(float *res, float *a, float *b, size_t m, size_t n) {
    for (size_t c = 0; c < n; c++) {
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = a[r * n + c] + b[r * n + c];
        }
    }
}

void mat_add_repeat(float *res, float *a, float *b, size_t m, size_t n,
                    size_t j) {
    for (size_t c = 0; c < n; c++) {
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = a[r * n + c] + b[r * j + (c%j)];
        }
    }
}

void mat_substract(float *res, float *a, float *b, size_t m, size_t n) {
    for (size_t c = 0; c < n; c++) {
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = a[r * n + c] - b[r * n + c];
        }
    }
}

void mat_substract_ew(float *res, float *a, float b, size_t m, size_t n) {
    for (size_t c = 0; c < n; c++) {
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = a[r * n + c] - b;
        }
    }
}

void mat_transpose(float *res, float *a, size_t m, size_t n) {
    for (size_t c = 0; c < n; c++) {
        for (size_t r = 0; r < m; r++) {
            res[c * m + r] = a[r * n + c];
        }
    }
}

void mat_sum_vector(float *res, float *a, size_t m, size_t n) {
    for (size_t r = 0; r < m; r++) {
        float sum = 0;
        for (size_t c = 0; c < n; c++) {
            sum += a[r * n + c];
        }
        res[r] = sum;
    }
}

void mat_apply_relu(float *res, size_t n) {
    for (size_t r = 0; r < n; r++) {
        res[r] = (res[r] > 0) * res[r];
    }
}

void mat_apply_drelu(float *res, size_t n) {
    for (size_t r = 0; r < n; r++) {
        res[r] = res[r] > 0;
    }
}

void mat_apply_sigmoid(float *res, size_t n) {
    for (size_t r = 0; r < n; r++) {
        float d = res[r];
        res[r] = 1 / (1 + expf(-d));
    }
}

void mat_apply_dsigmoid(float *res, size_t n) {
    for (size_t r = 0; r < n; r++) {
        float d = res[r];
        res[r] = d * (1 - d);
    }
}

void mat_apply_softmax(float *res, size_t m, size_t n) {
    for (size_t c = 0; c < n; c++) {
        float sum = 0;
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = expf(res[r * n + c]);
            sum += res[r * n + c];
        }
        for (size_t r = 0; r < m; r++) {
            res[r * n + c] = res[r * n + c] / sum;
        }
    }
}

void mat_apply_dsoftmax(float *res, size_t n) {
    float sum = 0;
    for (size_t r = 0; r < n; r++) {
        res[r] = expf(res[r]);
        sum += res[r];
    }
    for (size_t r = 0; r < n; r++) {
        res[r] = (res[r] * sum - res[r] * res[r]) / (sum * sum);
    }
}

void test_matrix() {
    float a[] = {1, 5, 7, 1, 9, 3, 2, 1, 2, 5, 7, 6};
    float b[] = {8, 5, 1, 3, 9, 3, 9, 1, 8, 1, 3, 5};
    float c[] = {8, 3, 9, 1, 5, 9, 1, 3, 1, 3, 8, 5};
    float d[] = {1, 2, 3, 4, 5, 6};
    float e[] = {2, 1, 2, 1, 2, 1};
    size_t m = 3;
    size_t n = 4;

    float r_add[] = {9, 10, 8, 4, 18, 6, 11, 2, 10, 6, 10, 11};
    float r_sub[] = {-7, 0, 6, -2, 0, 0, -7, 0, -6, 4, 4, 1};
    float r_mul[] = {23, 57, 66, 80, 56, 115, 46, 100, 100};
    float r_mul_sc[] = {3, 15, 21, 3, 27, 9, 6, 3, 6, 15, 21, 18};
    float r_hadamard[] = {2, 2, 6, 4, 10, 6};
    float r_transpose[] = {1, 4, 2, 5, 3, 6};

    float t_add[12];
    mat_add(t_add, a, b, m, n);
    for (int i = 0; i < 12; i++) {
        if (t_add[i] != r_add[i]) {
            printf("add %d: %f != %f\n", i, t_add[i], r_add[i]);
        }
    }

    float t_sub[12];
    mat_substract(t_sub, a, b, m, n);
    for (int i = 0; i < 12; i++) {
        if (t_sub[i] != r_sub[i]) {
            printf("sub %d: %f != %f\n", i, t_sub[i], r_sub[i]);
        }
    }

    float t_mul[9];
    mat_multiply(t_mul, a, c, m, n, m);
    for (int i = 0; i < 9; i++) {
        if (t_mul[i] != r_mul[i]) {
            printf("mul %d: %f != %f\n", i, t_mul[i], r_mul[i]);
        }
    }

    float t_mul_sc[12];
    mat_multiply_scalar(t_mul_sc, a, 3, m, n);
    for (int i = 0; i < 12; i++) {
        if (t_mul_sc[i] != r_mul_sc[i]) {
            printf("mul_sc %d: %f != %f\n", i, t_mul_sc[i], r_mul_sc[i]);
        }
    }

    float t_hadamard[6];
    mat_multiply_hadamard(t_hadamard, d, e, 3, 2);
    for (int i = 0; i < 6; i++) {
        if (t_hadamard[i] != r_hadamard[i]) {
            printf("hadamard %d: %f != %f\n", i, t_hadamard[i], r_hadamard[i]);
        }
    }

    float t_transpose[6];
    mat_transpose(t_transpose, d, 2, 3);
    for (int i = 0; i < 6; i++) {
        if (t_transpose[i] != r_transpose[i]) {
            printf("transpose %d: %f != %f\n", i, t_transpose[i],
                   r_transpose[i]);
        }
    }
}
