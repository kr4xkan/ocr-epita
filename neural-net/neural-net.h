#pragma once

#include <stddef.h>

struct LabeledImage {
    float data[28*28];
    int label;
};

typedef struct LabeledImage LabeledImage;

void neural_test();

void setup_network(int *layers_node_count, int layer_count);

void get_input(float *res, char *image_path);

LabeledImage *load_dataset(char *path, size_t *len);

void get_target_array(float arr[10], LabeledImage img);
