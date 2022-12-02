#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <err.h>

typedef struct {
    void* data;
    size_t size;
    size_t capacity;
    size_t head;
} Buffer;

Buffer* new_buffer();
void free_buffer(Buffer* buf);
void save_buffer(Buffer* buf, char* path);
Buffer* load_buffer(char* path);
void reserve_space(Buffer* buf, size_t size);
void write_buffer(Buffer* buf, void* value, size_t size);
void read_buffer(Buffer* buf, void* dest, size_t size);

typedef struct Matrix {
    double* v;
    size_t n;
    size_t p;
} Matrix;

void serialize_matrix(Buffer* buf, Matrix* x);
Matrix deserialize_matrix(Buffer* buf);
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
double sum_abs(Matrix a);
void relu(Matrix a, Matrix res);
Matrix relu_deriv(Matrix a);
void softmax(Matrix a, Matrix res);
void crossentropy(Matrix a, Matrix expected, Matrix res);
void print_mat(Matrix a);
