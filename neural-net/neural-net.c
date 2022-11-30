#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

#include "../utils.h"
#include "matrix.h"
#include "neural-net.h"

void free_network(NeuralNetwork* nn) {
    for (szt i = 1; i < nn->layer_count; i++) {
        free_layer(&nn->layers[i]);
    }
    free(nn->layers);
    free_matrix(&nn->updates.dZ2);
    free_matrix(&nn->updates.dW2);
    free_matrix(&nn->updates.mW2);
    free_matrix(&nn->updates.dB2);
    free_matrix(&nn->updates.mB2);
    free_matrix(&nn->updates.dZ1);
    free_matrix(&nn->updates.dW1);
    free_matrix(&nn->updates.mW1);
    free_matrix(&nn->updates.dB1);
    free_matrix(&nn->updates.mB1);
    fclose(nn->csv);
}

void free_layer(Layer* layer) {
    free_matrix(&layer->Z);
    free_matrix(&layer->A);
    free_matrix(&layer->W);
    free_matrix(&layer->B);
}

Layer new_layer(
        enum ActivationFunction activation,
        size_t num_nodes,
        size_t prev_num_nodes
) {
    Layer l;
    l.size = num_nodes;
    l.activation = activation;
    l.A = new_matrix(num_nodes, 1);
    if (prev_num_nodes != 0) {
        l.Z = new_matrix(num_nodes, 1);
        l.B = new_random_matrix(num_nodes, 1);
        l.W = new_random_matrix(num_nodes, prev_num_nodes);
    }
    return l;
}

NeuralNetwork new_network(int argc, char** argv) {
    NeuralNetwork nn;
    nn.layer_count = 3;
    nn.learning_rate = 0.005;
    nn.decay = 0.00000002;
    nn.momentum = 1;
    nn.updates.current_learning_rate = nn.learning_rate;
    nn.updates.iterations = 0;
    nn.layers = calloc(nn.layer_count, sizeof(Layer));
    nn.layers[0] = new_layer(ReLU, 784, 0);
    nn.layers[1] = new_layer(ReLU, 15, 784);
    nn.layers[2] = new_layer(SoftMax, 10, 15);
    free(nn.layers[0].A.v);
    nn.updates.dZ2 = new_matrix(nn.layers[2].Z.n, nn.layers[2].Z.p);
    nn.updates.dW2 = new_matrix(nn.layers[2].W.n, nn.layers[2].W.p);
    nn.updates.mW2 = new_matrix(nn.layers[2].W.n, nn.layers[2].W.p);
    nn.updates.dB2 = new_matrix(nn.layers[2].B.n, nn.layers[2].B.p);
    nn.updates.mB2 = new_matrix(nn.layers[2].B.n, nn.layers[2].B.p);
    nn.updates.dZ1 = new_matrix(nn.layers[1].Z.n, nn.layers[1].Z.p);
    nn.updates.dW1 = new_matrix(nn.layers[1].W.n, nn.layers[1].W.p);
    nn.updates.mW1 = new_matrix(nn.layers[1].W.n, nn.layers[1].W.p);
    nn.updates.dB1 = new_matrix(nn.layers[1].B.n, nn.layers[1].B.p);
    nn.updates.mB1 = new_matrix(nn.layers[1].B.n, nn.layers[1].B.p);
    nn.csv = fopen("stat.csv", "w");
    fprintf(nn.csv, "error,lr\n");
    return nn;
}

void print_stat(NeuralNetwork* nn, double error) {
    fprintf(nn->csv, "%f,%f\n", error, nn->updates.current_learning_rate);
    fflush(nn->csv);;
}

void forward_pass(NeuralNetwork* nn) {
    for (szt i = 1; i < nn->layer_count; i++) {
        Layer prev = nn->layers[i - 1];
        Layer cur = nn->layers[i];
        multiply(cur.W, prev.A, cur.Z);
        add(cur.Z, cur.B, cur.Z);
        switch (cur.activation) {
            case ReLU:
                relu(cur.Z, cur.A);
                break;
            case SoftMax:
                softmax(cur.Z, cur.A);
                break;
            default:
                break;
        }
    }
}

double backward_pass(NeuralNetwork* nn, Matrix expected) {
    nn->updates.current_learning_rate =
        nn->learning_rate * (1 / (1 + nn->decay * nn->updates.iterations));

    sub(nn->layers[2].A, expected, nn->updates.dZ2);
    Matrix A1T = transpose(nn->layers[1].A);
    multiply(nn->updates.dZ2, A1T, nn->updates.dW2);

    Matrix W2T = transpose(nn->layers[2].W);
    multiply(W2T, nn->updates.dZ2, nn->updates.dZ1);
    Matrix dRelu = relu_deriv(nn->layers[1].Z);
    multiply_ew(nn->updates.dZ1, dRelu, nn->updates.dZ1);
    Matrix A0T = transpose(nn->layers[0].A);
    multiply(nn->updates.dZ1, A0T, nn->updates.dW1);

    multiply_scalar(nn->updates.dW2, nn->updates.current_learning_rate, nn->updates.dW2);
    multiply_scalar(nn->updates.dW1, nn->updates.current_learning_rate, nn->updates.dW1);
    multiply_scalar(nn->updates.dZ2, nn->updates.current_learning_rate, nn->updates.dB2);
    multiply_scalar(nn->updates.dZ1, nn->updates.current_learning_rate, nn->updates.dB1);

    multiply_scalar(nn->updates.mW2, nn->momentum, nn->updates.mW2);
    multiply_scalar(nn->updates.mW1, nn->momentum, nn->updates.mW1);
    multiply_scalar(nn->updates.mB2, nn->momentum, nn->updates.mB2);
    multiply_scalar(nn->updates.mB1, nn->momentum, nn->updates.mB1);
    sub(nn->updates.mW2, nn->updates.dW2, nn->updates.mW2);
    sub(nn->updates.mW1, nn->updates.dW1, nn->updates.mW1);
    sub(nn->updates.mB2, nn->updates.dB2, nn->updates.mB2);
    sub(nn->updates.mB1, nn->updates.dB1, nn->updates.mB1);

    // add(nn->layers[2].W, nn->updates.mW2, nn->layers[2].W);
    // add(nn->layers[1].W, nn->updates.mW1, nn->layers[1].W);
    // add(nn->layers[2].B, nn->updates.mB2, nn->layers[2].B);
    // add(nn->layers[1].B, nn->updates.mB1, nn->layers[1].B);
    sub(nn->layers[2].W, nn->updates.dW2, nn->layers[2].W);
    sub(nn->layers[1].W, nn->updates.dW1, nn->layers[1].W);
    sub(nn->layers[2].B, nn->updates.dB2, nn->layers[2].B);
    sub(nn->layers[1].B, nn->updates.dB1, nn->layers[1].B);

    free_matrix(&W2T);
    free_matrix(&A1T);
    free_matrix(&A0T);
    free_matrix(&dRelu);

    nn->updates.iterations++;
    return sum_abs(nn->updates.dZ2);
}

int main(int argc, char **argv) {
    // Initialize randomizer
    srand((unsigned int)time(NULL));

    if (argc != 2)
        errx(1, "./neural-net <path>");

    NeuralNetwork nn = new_network(argc, argv);

    szt len_dataset;
    LabeledImage* dataset = load_dataset(argv[1], &len_dataset);

    Matrix expected = new_matrix(10, 1);
    szt prev = 0;
    for (szt i = 0; i < 10000; i++) {
        // Randomize array
        for (szt i = 0; i < len_dataset - 1; i++) {
            szt j = i + rand() / (RAND_MAX / (len_dataset - i) + 1);
            LabeledImage t = dataset[j];
            dataset[j] = dataset[i];
            dataset[i] = t;
        }

        double error = 0;
        for (szt j = 0; j < len_dataset; j++) {
            LabeledImage img = dataset[j];
            expected.v[prev] = 0;
            expected.v[img.label] = 1;
            prev = img.label;
            nn.layers[0].A.v = img.data;
            forward_pass(&nn);
            error += backward_pass(&nn, expected);
        }
        print_stat(&nn, error);
        if (i % 10 == 0) {
            printf("[%zu] Error: %.4f    LR: %f\n", i, error, nn.updates.current_learning_rate);
            fflush(stdout);
        }
        if (i % 100 == 0) {
            print_mat(nn.updates.dW2);
            fflush(stdout);
        }
    }
    free_matrix(&expected);
    free_network(&nn);
    free(dataset);
    return 0;
}

void save_network(NeuralNetwork *nn) {
    FILE *file;
    file = fopen("save.neural", "w");

    fprintf(file, "%zu\n", nn->layer_count);
    fprintf(file, "%f\n", nn->decay);
    fprintf(file, "%f\n", nn->momentum);
    fprintf(file, "%f\n", nn->learning_rate);
    fprintf(file, "%f\n", nn->updates.current_learning_rate);
    fprintf(file, "%zu\n", nn->updates.iterations);
    fprintf(file, "--l\n");
    for (szt i = 0; i < nn->layer_count; i++)
        fprintf(file, "%zu,%d\n", nn->layers[i].size, nn->layers[i].activation);
    fprintf(file, "--w\n");
    for (szt i = 1; i < nn->layer_count; i++) {

    }
    fclose(file);
}

/*
enum Mode {
    HYPERPARAMS,
    LAYERS,
    WEIGHTS,
    BIAS,
};

void load_network(NeuralNetwork *nn, char *path) {
    FILE *file;
    file = fopen(path, "r");

    if (!file) {
        errx(1, "Could not load network from: %s", path);
    }

    char line[100000];
    int n = 0;
    enum Mode mode = HYPERPARAMS;
    while (fgets(line, 100000, file) != NULL) {
        if (strncmp(line, "--", 2) == 0) {
            char *modestr = line + 2;
            if (strncmp(modestr, "l", 1) == 0) {
                mode = LAYERS;
                nn->layers = calloc(nn->layer_count, sizeof(Layer));
                nn->weights = malloc((nn->layer_count - 1) * sizeof(float *));
                nn->bias = malloc((nn->layer_count - 1) * sizeof(float *));
                nn->layers_node_count = malloc(nn->layer_count * sizeof(int));
            } else if (strncmp(modestr, "w", 1) == 0) {
                mode = WEIGHTS;
                for (int i = 1; i < nn->layer_count; i++) {
                    nn->weights_sizes[i - 1][0] = nn->layers_node_count[i];
                    nn->weights_sizes[i - 1][1] = nn->layers_node_count[i - 1];
                    nn->weights[i - 1] =
                        malloc(nn->weights_sizes[i - 1][0] *
                               nn->weights_sizes[i - 1][1] * sizeof(float));
                    nn->bias[i - 1] =
                        calloc(nn->layers_node_count[i], sizeof(float));
                }
            } else if (strncmp(modestr, "b", 1) == 0) {
                mode = BIAS;
            }
            n = -1;
        } else {
            char *num;
            int i = 0;
            switch (mode) {
            case HYPERPARAMS:
                if (n == 0) {
                    nn->layer_count = atoi(line);
                } else if (n == 1) {
                    nn->batch_size = atoi(line);
                } else if (n == 2) {
                    nn->learning_rate = atof(line);
                }
                break;

            case LAYERS:
                nn->layers_node_count[n] = atoi(line);
                break;

            case WEIGHTS:
                num = strtok(line, ",");
                while (num != NULL) {
                    nn->weights[n][i] = atof(num);
                    i++;
                    num = strtok(NULL, ",");
                }
                break;

            case BIAS:
                num = strtok(line, ",");
                while (num != NULL) {
                    nn->bias[n][i] = atof(num);
                    i++;
                    num = strtok(NULL, ",");
                }
                break;
            }
        }
        n++;
    }
}

void setup_network(NeuralNetwork *nn, int *layers_node_count, int batch_size,
                   float learning_rate, int layer_count) {
    nn->weights = malloc((layer_count - 1) * sizeof(float *));
    nn->bias = malloc((layer_count - 1) * sizeof(float *));
    nn->layers = calloc(layer_count, sizeof(Layer));
    nn->batch_size = batch_size;

    for (int i = 1; i < layer_count; i++) {
        nn->weights_sizes[i - 1][0] = layers_node_count[i];
        nn->weights_sizes[i - 1][1] = layers_node_count[i - 1];
        nn->weights[i - 1] =
            malloc(nn->weights_sizes[i - 1][0] * nn->weights_sizes[i - 1][1] *
                   sizeof(float));
        nn->bias[i - 1] = calloc(layers_node_count[i], sizeof(float));
        nn->layers[i].A =
            malloc(layers_node_count[i] * nn->batch_size * sizeof(float));
        nn->layers[i].Z =
            malloc(layers_node_count[i] * nn->batch_size * sizeof(float));
        nn->layers[i].DZ =
            malloc(layers_node_count[i] * nn->batch_size * sizeof(float));
        nn->layers[i].size = layers_node_count[i];
        nn->layers[i].disabled = calloc(layers_node_count[i], sizeof(char));
        mat_randomize(nn->weights[i - 1], nn->weights_sizes[i - 1][0] *
                                              nn->weights_sizes[i - 1][1]);
    }

    nn->layer_count = layer_count;
    nn->layers_node_count = layers_node_count;
    nn->learning_rate = learning_rate;
    nn->loss = 0;
}
*/

LabeledImage* load_dataset(char *path, szt *len_d) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    szt len = 0;

    if (!d)
        errx(1, "Could not load dataset at '%s'", path);

    while ((dir = readdir(d)) != NULL) {
        if (!strcmp(strrchr(dir->d_name, '\0') - 4, ".bmp")) {
            len++;
        }
    }
    closedir(d);

    LabeledImage *dataset = malloc(len * sizeof(LabeledImage));
    d = opendir(path);

    szt i = 0;
    while ((dir = readdir(d)) != NULL) {
        if (!strcmp(strrchr(dir->d_name, '\0') - 4, ".bmp")) {
            char *tmppath =
                malloc((strlen(path) + strlen(dir->d_name) + 5) * sizeof(char));
            load_image(dataset[i].data,
                      strcat(strcpy(tmppath, path), dir->d_name));
            dataset[i].label = dir->d_name[0] - 48;
            free(tmppath);
            i++;
        }
    }
    closedir(d);

    for (szt i = 0; i < len - 1; i++) {
        szt j = i + rand() / (RAND_MAX / (len - i) + 1);
        LabeledImage t = dataset[j];
        dataset[j] = dataset[i];
        dataset[i] = t;
    }

    printf("Loaded %zu images\n", len);
    (*len_d) = len;
    return dataset;
}

void print_pixel(double *img, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            double a = img[x * 28 + y] * 255;
            char c;
            if (a > 240)
                c = ' ';
            else if (a > 200)
                c = '.';
            else if (a > 150)
                c = ':';
            else if (a > 120)
                c = '=';
            else if (a > 100)
                c = '+';
            else if (a > 90)
                c = '*';
            else if (a > 70)
                c = '#';
            else if (a > 50)
                c = '%';
            else
                c = '@';
            printf("%c", c);
        }
        printf("\n");
    }
}


void load_image(double *res, char *image_path) {
    SDL_Surface* surface;
    surface = IMG_Load(image_path);
    if (!surface) {
        errx(1, "Could not load image (%s)", image_path);
    }
    int w, h;
    w = surface->w;
    h = surface->h;

    Uint8 r, g, b;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            res[x * 28 + y] = (double)average / 255;
        }
    }

    SDL_FreeSurface(surface);
}
