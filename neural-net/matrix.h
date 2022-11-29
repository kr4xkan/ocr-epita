#pragma once

#include "stddef.h"

typedef struct Matrix {
    double* v;
    size_t n;
    size_t p;
} Matrix;

Matrix new_matrix(size_t n, size_t p);
Matrix new_random_matrix(size_t n, size_t p);
void free_matrix(Matrix* m);
Matrix transpose(Matrix a);
void multiply(Matrix a, Matrix b, Matrix res);
void multiply_scalar(Matrix a, double alpha, Matrix res);
void multiply_ew(Matrix a, Matrix b, Matrix res);
void add(Matrix a, Matrix b, Matrix res);
void add_scalar(Matrix a, double alpha, Matrix res);
void sub(Matrix a, Matrix b, Matrix res);
double sum(Matrix a);
void relu(Matrix a, Matrix res);
Matrix relu_deriv(Matrix a);
void softmax(Matrix a, Matrix res);
void print_mat(Matrix a);

void test_matrix();

void mat_randomize(float *mat, size_t len);

void mat_print(float *mat, size_t row, size_t col);

void mat_copy(float *src, float *dest, size_t len);

void mat_multiply(float *res, float *a, float *b, size_t n, size_t m, size_t p);

void mat_multiply_scalar(float *res, float *a, float b, size_t n, size_t m);

void mat_multiply_hadamard(float *res, float *a, float *b, size_t n, size_t m);

void mat_add(float *res, float *a, float *b, size_t n, size_t m);

void mat_add_repeat(float *res, float *a, float *b, size_t n, size_t m,
                    size_t j);

void mat_substract(float *res, float *a, float *b, size_t n, size_t m);

void mat_substract_ew(float *res, float *a, float b, size_t m, size_t n);

void mat_transpose(float *res, float *a, size_t n, size_t m);

void mat_sum_vector(float *res, float *a, size_t m, size_t n);

void mat_apply_relu(float *res, size_t n);

void mat_apply_drelu(float *res, size_t n);

void mat_apply_sigmoid(float *res, size_t n);

void mat_apply_dsigmoid(float *res, size_t n);

void mat_apply_softmax(float *res, size_t m, size_t n);

void mat_apply_dsoftmax(float *res, size_t n);
