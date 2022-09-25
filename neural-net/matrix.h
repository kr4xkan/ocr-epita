#pragma once

#include "stddef.h"

void test_matrix();

void mat_randomize(float *mat, size_t len);

void mat_print(float *mat, size_t row, size_t col);

void mat_copy(float *src, float *dest, size_t len);

void mat_multiply(float *res, float *a, float *b, size_t n, size_t m, size_t p);

void mat_multiply_scalar(float *res, float *a, float b, size_t n, size_t m);

void mat_multiply_hadamard(float *res, float *a, float *b, size_t n, size_t m);

void mat_add(float *res, float *a, float *b, size_t n, size_t m);

void mat_substract(float *res, float *a, float *b, size_t n, size_t m);

void mat_transpose(float *res, float *a, size_t n, size_t m);

void mat_apply_sigmoid(float *res, size_t n);

void mat_apply_dsigmoid(float *res, size_t n);

void mat_apply_softmax(float *res, size_t n);

void mat_apply_dsoftmax(float *res, size_t n);
