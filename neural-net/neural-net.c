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

Layer new_layer(
        enum ActivationFunction activation,
        size_t num_nodes,
        size_t prev_num_nodes
) {
    Layer l;
    l.size = num_nodes;
    l.activation = activation;
    l.A = new_matrix(num_nodes, 1);
    l.Z = new_matrix(num_nodes, 1);
    if (prev_num_nodes != 0) {
        printf("Layer %zu\n", num_nodes);
        l.B = new_random_matrix(num_nodes, 1);
        l.W = new_random_matrix(prev_num_nodes, num_nodes);
    }
    return l;
}

NeuralNetwork new_network(int argc, char** argv) {
    NeuralNetwork nn;
    nn.layer_count = 3;
    nn.learning_rate = 0.01;
    nn.layers = calloc(nn.layer_count, sizeof(Layer));
    nn.layers[0] = new_layer(ReLU, 784, 0);
    nn.layers[1] = new_layer(ReLU, 10, 784);
    nn.layers[2] = new_layer(SoftMax, 10, 10);
    return nn;
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

void backward_pass(NeuralNetwork* nn, Matrix expected) {
    Matrix dZ2 = new_matrix(nn->layers[2].Z.n,nn->layers[2].Z.p);
    Matrix dW2 = new_matrix(nn->layers[2].W.n,nn->layers[2].W.p);
    Matrix dB2 = new_matrix(nn->layers[2].B.n,nn->layers[2].B.p);
    Matrix dZ1 = new_matrix(nn->layers[1].Z.n,nn->layers[1].Z.p);
    Matrix dW1 = new_matrix(nn->layers[1].W.n,nn->layers[1].W.p);
    Matrix dB1 = new_matrix(nn->layers[1].B.n,nn->layers[1].B.p);

    sub(nn->layers[2].A, expected, dZ2);
    Matrix A1T = transpose(nn->layers[1].A);
    multiply(dZ2, A1T, dW2);

    Matrix W2T = transpose(nn->layers[2].W);
    multiply(W2T, dZ2, dZ1);
    Matrix dRelu = relu_deriv(nn->layers[1].Z);
    multiply_ew(dZ1, dRelu, dZ1);
    Matrix A0T = transpose(nn->layers[0].A);
    multiply(dZ1, A0T, dW1);

    multiply_scalar(dW2, nn->learning_rate, dW2);
    multiply_scalar(dW1, nn->learning_rate, dW1);

    add(nn->layers[2].W, dW2, nn->layers[2].W);
    add(nn->layers[1].W, dW1, nn->layers[1].W);
    double sumDZ2 = sum(dZ2) / (dZ2.n * dZ2.p);
    double sumDZ1 = sum(dZ1) / (dZ1.n * dZ1.p);
    add_scalar(nn->layers[2].B, sumDZ2, nn->layers[2].B);
    add_scalar(nn->layers[1].B, sumDZ1, nn->layers[1].B);

    free_matrix(&W2T);
    free_matrix(&A1T);
    free_matrix(&A0T);
    free_matrix(&dRelu);
    free_matrix(&dZ2);
    free_matrix(&dW2);
    free_matrix(&dB2);
    free_matrix(&dZ1);
    free_matrix(&dW1);
    free_matrix(&dB1);
}

int main(int argc, char **argv) {
    // Initialize randomizer
    srand((unsigned int)time(NULL));

    if (argc != 2)
        errx(1, "./neural-net <path>");

    NeuralNetwork nn = new_network(argc, argv);
    SDL_Surface* surface;
    surface = IMG_Load(argv[1]);
    if (!surface) {
        errx(1, "Could not load image (%s)", argv[1]);
    }
    int w, h;
    w = surface->w;
    h = surface->h;

    Uint8 r, g, b;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            nn.layers[0].A.v[x * 28 + y] = (double)average / 255;
        }
    }

    SDL_FreeSurface(surface);
    print_pixel(nn.layers[0].A.v, 28, 28);
    //load_image(nn.layers[0].A.v, argv[1]);
    forward_pass(&nn);
    printf("gang1\n");
    fflush(stdout);
    print_mat(nn.layers[2].W);
    //Matrix expected = new_matrix(10, 1);
    //expected.v[5] = 1;
    //backward_pass(&nn, expected);
    //print(nn.layers[2].W);
    printf("gang2\n");
    fflush(stdout);
    printf("done");
    fflush(stdout);
    return 0;
}

/*
void save_network(NeuralNetwork *nn) {
    FILE *file;
    file = fopen("save.neural", "w");

    fprintf(file, "%d\n", nn->layer_count);
    fprintf(file, "%d\n", nn->batch_size);
    fprintf(file, "%f\n", nn->learning_rate);
    fprintf(file, "--l\n");
    for (int k = 0; k < nn->layer_count; k++) {
        fprintf(file, "%d\n", nn->layers_node_count[k]);
    }
    fprintf(file, "--w\n");
    for (int k = 0; k < nn->layer_count - 1; k++) {
        size_t len = nn->weights_sizes[k][0] * nn->weights_sizes[k][1];
        for (size_t c = 0; c < len; c++) {
            fprintf(file, c == len - 1 ? "%f" : "%f,", nn->weights[k][c]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "--b\n");
    for (int k = 0; k < nn->layer_count - 1; k++) {
        size_t len = nn->layers_node_count[k + 1];
        for (size_t c = 0; c < len; c++) {
            fprintf(file, c == len - 1 ? "%f" : "%f,", nn->bias[k][c]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

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

LabeledImage *load_dataset(char *path, size_t *len_d) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    size_t len = 0;

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

    size_t i = 0;
    while ((dir = readdir(d)) != NULL) {
        if (!strcmp(strrchr(dir->d_name, '\0') - 4, ".bmp")) {
            char *tmppath =
                malloc((strlen(path) + strlen(dir->d_name) + 5) * sizeof(char));
            get_input(dataset[i].data,
                      strcat(strcpy(tmppath, path), dir->d_name));
            dataset[i].label = dir->d_name[0] - 48;
            free(tmppath);
            i++;
        }
    }
    closedir(d);

    for (size_t i = 0; i < len - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (len - i) + 1);
        LabeledImage t = dataset[j];
        dataset[j] = dataset[i];
        dataset[i] = t;
    }

    printf("Loaded %zu images\n", len);
    (*len_d) = len;
    return dataset;
}
*/

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
