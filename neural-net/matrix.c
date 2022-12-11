#include "matrix.h"
#include <stdio.h>

Buffer *new_buffer() {
    Buffer *buf = malloc(sizeof(Buffer));
    buf->capacity = 1;
    buf->size = 0;
    buf->head = 0;
    buf->data = malloc(sizeof(char));
    return buf;
}

void free_buffer(Buffer *buf) {
    free(buf->data);
    free(buf);
}

void save_buffer(Buffer *buf, char *path) {
    FILE *f;
    f = fopen(path, "w");
    if (f == NULL)
        errx(1, "Could not save network at '%s'", path);

    fwrite(buf->data, sizeof(char), buf->size, f);
    fclose(f);
}

Buffer *load_buffer(char *path) {
    FILE *f;
    f = fopen(path, "rb");
    if (f == NULL)
        errx(1, "Could not save network at '%s'", path);

    Buffer *buf = new_buffer();

    fseek(f, 0, SEEK_END);
    size_t fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    reserve_space(buf, fileSize);
    fread(buf->data, sizeof(char), fileSize, f);

    fclose(f);
    return buf;
}

void reserve_space(Buffer *buf, size_t size) {
    if (buf->size + size > buf->capacity) {
        while (buf->size + size > buf->capacity) {
            buf->capacity *= 2;
        }
        buf->data = realloc(buf->data, buf->capacity);
    }
}

void write_buffer(Buffer *buf, void *value, size_t size) {
    memcpy(buf->data + buf->size, value, size);
    buf->size += size;
    // printf("wrote %zu bytes\n", size);
}

void read_buffer(Buffer *buf, void *dest, size_t size) {
    memcpy(dest, buf->data + buf->head, size);
    // printf("read %zu bytes at %zu\n", size, buf->head);
    buf->head += size;
}

void serialize_matrix(Buffer *buf, Matrix *x) {
    size_t len = x->n * x->p;
    size_t required_size = len * sizeof(double) + 2 * sizeof(size_t);
    reserve_space(buf, required_size);

    write_buffer(buf, &x->n, sizeof(size_t));
    write_buffer(buf, &x->p, sizeof(size_t));
    write_buffer(buf, x->v, len * sizeof(double));
}

Matrix deserialize_matrix(Buffer *buf) {
    Matrix m;
    size_t i;
    read_buffer(buf, &i, sizeof(size_t));
    m.n = i;
    read_buffer(buf, &i, sizeof(size_t));
    m.p = i;

    size_t len = m.n * m.p;
    m.v = calloc(len, sizeof(double));
    read_buffer(buf, m.v, len * sizeof(double));
    return m;
}

Matrix new_matrix(size_t n, size_t p) {
    Matrix m;
    m.v = calloc(n * p, sizeof(double));
    m.n = n;
    m.p = p;
    return m;
}

Matrix new_random_matrix(size_t n, size_t p) {
    Matrix m;
    m.v = malloc(n * p * sizeof(double));
    m.n = n;
    m.p = p;
    for (size_t i = 0; i < n * p; i++)
        m.v[i] = ((double)rand() / (double)RAND_MAX) * 2 - 1;
    return m;
}

void free_matrix(Matrix *m) { free(m->v); }

Matrix transpose(Matrix a) {
    Matrix t = new_matrix(a.p, a.n);
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            t.v[j * t.p + i] = a.v[i * a.p + j];
        }
    }
    return t;
}

void multiply(Matrix a, Matrix b, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < b.p; j++) {
            double sum = 0;
            for (size_t k = 0; k < a.p; k++) {
                sum += a.v[i * a.p + k] * b.v[k * b.p + j];
            }
            res.v[i * b.p + j] = sum;
        }
    }
}

void multiply_ew(Matrix a, Matrix b, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * a.p + j] = a.v[i * a.p + j] * b.v[i * b.p + j];
        }
    }
}

void multiply_scalar(Matrix a, double alpha, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * a.p + j] = a.v[i * a.p + j] * alpha;
        }
    }
}

void add(Matrix a, Matrix b, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * a.p + j] = a.v[i * a.p + j] + b.v[i * b.p + (j % b.p)];
        }
    }
}

void add_scalar(Matrix a, double alpha, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * a.p + j] = a.v[i * a.p + j] + alpha;
        }
    }
}

void sub(Matrix a, Matrix b, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * a.p + j] = a.v[i * a.p + j] - b.v[i * b.p + j];
        }
    }
}

double sum(Matrix a) {
    double res = 0;
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res += a.v[i * a.p + j];
        }
    }
    return res;
}

double sum_abs(Matrix a) {
    double res = 0;
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res += fabs(a.v[i * a.p + j]);
        }
    }
    return res;
}

void sum_vector(Matrix a, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * res.p + (j % res.p)] += a.v[i * a.p + j];
        }
    }
}

void relu(Matrix a, Matrix res) {
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * a.p + j] = a.v[i * a.p + j] * (a.v[i * a.p + j] > 0);
        }
    }
}

Matrix relu_deriv(Matrix a) {
    Matrix res = new_matrix(a.n, a.p);
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            res.v[i * a.p + j] = a.v[i * a.p + j] > 0;
        }
    }
    return res;
}

void softmax(Matrix a, Matrix res) {
    double max = a.v[0];
    for (size_t i = 0; i < a.n; i++) {
        for (size_t j = 0; j < a.p; j++) {
            if (a.v[i * a.p + j] > max)
                max = a.v[i * a.p + j];
        }
    }
    for (size_t j = 0; j < a.p; j++) {
        double sum = 0;
        for (size_t i = 0; i < a.n; i++) {
            res.v[i * a.p + j] = expf(a.v[i * a.p + j] - max);
            sum += res.v[i * a.p + j];
        }
        for (size_t i = 0; i < a.n; i++) {
            res.v[i * a.p + j] /= sum;
        }
    }
}

void print_mat(Matrix a) {
    for (size_t r = 0; r < a.n; r++) {
        for (size_t c = 0; c < a.p; c++) {
            printf("%9.3f", a.v[r * a.p + c]);
        }
        printf("\n");
    }
}
