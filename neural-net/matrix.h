#pragma once

#include "stddef.h"

void mat_randomize(float *mat, size_t len);

void mat_print(float *mat, size_t row, size_t col);

void mat_multiply(float *res, float *a, float *b, size_t n, size_t m, size_t p);
