#include <err.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "matrix.h"
#include "neural-net.h"

int main(int argc, char **argv) {
    // Initialize randomizer
    srand((unsigned int)time(NULL));
    
    test_matrix();

    if (argc < 2) {
        errx(1, "Usage: ./neural-net --xor|digit|guess");
    }

    if (strcmp(argv[1], "--digit") == 0) {
        errx(1, "Not yet implemented!");
    } else if(strcmp(argv[1], "--guess") == 0) {
        if (argc != 4) {
            errx(1, "\nUsage: ./neural-net --guess 0|1 0|1\nExample: ./neural-net --guess 0 1");
        }
        printf("%d", guess_xor(argv+2));
    } else {
        if (argc == 3) {
            main_xor(argv[2]);
        } else {
            main_xor("");
        }
    }
}

int guess_xor(char **input) {
    NeuralNetwork nn;
    load_network(&nn, "save.neural");

    nn.batch_size = 1;

    float *W1 = nn.weights[0];
    float *W2 = nn.weights[1];
    float *b1 = nn.bias[0];
    float *b2 = nn.bias[1];

    float *Z1 = malloc(nn.layers_node_count[1] * nn.batch_size * sizeof(float));
    float *A1 = malloc(nn.layers_node_count[1] * nn.batch_size * sizeof(float));
    float *Z2 = malloc(nn.layers_node_count[2] * nn.batch_size * sizeof(float));
    float *A2 = malloc(nn.layers_node_count[2] * nn.batch_size * sizeof(float));
    
    float X[2] = {atof(input[0]), atof(input[1])};
    // FORWARD PROPAGATION
    mat_multiply(Z1, W1, X, nn.weights_sizes[0][0], nn.weights_sizes[0][1], nn.batch_size);
    mat_add_repeat(Z1, Z1, b1, nn.layers_node_count[1], nn.batch_size, 1);
    mat_copy(Z1, A1, nn.layers_node_count[1] * nn.batch_size);

    //mat_apply_relu(A1, network.layers_node_count[1] * network.batch_size);
    mat_apply_sigmoid(A1, nn.layers_node_count[1] * nn.batch_size);

    mat_multiply(Z2, W2, A1, nn.weights_sizes[1][0], nn.weights_sizes[1][1], nn.batch_size);
    mat_add_repeat(Z2, Z2, b2, nn.layers_node_count[2], nn.batch_size, 1);
    mat_copy(Z2, A2, nn.layers_node_count[2] * nn.batch_size);

    //mat_apply_softmax(A2, network.layers_node_count[2], network.batch_size);
    mat_apply_sigmoid(A2, nn.layers_node_count[2] * nn.batch_size);

    return A2[0] > 0.5;
}

int main_xor(char *path) {
    printf("XOR Neural Network\n");


    FILE *outputFile;
    outputFile = fopen("out.csv", "a");

    NeuralNetwork nn;

    int layers_node_count[3];
    layers_node_count[0] = 2;
    layers_node_count[1] = 8;
    layers_node_count[2] = 1;

    if (strlen(path) != 0) {
        load_network(&nn, path);
        printf("Network loaded from save\n");
    } else {
        int layer_count = 3;
        float learning_rate = 0.05;
        int batch_size = 4;
        setup_network(&nn, layers_node_count, batch_size, learning_rate, layer_count);
        printf("New network created\n");
    }


    float X[2*4] = {
        0, 0, 1, 1,
        0, 1, 0, 1
    };
    float Y[4] = {
        0, 1, 1, 0
    };

    float *W1 = nn.weights[0];
    float *W2 = nn.weights[1];
    float *b1 = nn.bias[0];
    float *b2 = nn.bias[1];

    float *Z1 = malloc(nn.layers_node_count[1] * nn.batch_size * sizeof(float));
    float *A1 = malloc(nn.layers_node_count[1] * nn.batch_size * sizeof(float));
    float *Z2 = malloc(nn.layers_node_count[2] * nn.batch_size * sizeof(float));
    float *A2 = malloc(nn.layers_node_count[2] * nn.batch_size * sizeof(float));

    float *dZ1 = malloc(nn.layers_node_count[1] * nn.batch_size * sizeof(float));
    float *dW1 = malloc(nn.weights_sizes[0][0] * nn.weights_sizes[0][1] * sizeof(float));
    float db1;

    float *dZ2 = malloc(nn.layers_node_count[2] * nn.batch_size * sizeof(float));
    float *dW2 = malloc(nn.weights_sizes[1][0] * nn.weights_sizes[1][1] * sizeof(float));
    float db2;

    float *A1_T = malloc(nn.batch_size * nn.layers_node_count[1] * sizeof(float));
    float *dReluZ1 = malloc(nn.layers_node_count[1] * nn.batch_size * sizeof(float));
    float *W2_T = malloc(nn.weights_sizes[1][1] * nn.weights_sizes[1][0] * sizeof(float));
    float *X_T = malloc(nn.batch_size * 2 * sizeof(float));

    fprintf(outputFile, "epoch,training\n");
    for (int k = 0; k < 1000001; k++) {
        // FORWARD PROPAGATION
        mat_multiply(Z1, W1, X, nn.weights_sizes[0][0], nn.weights_sizes[0][1], nn.batch_size);
        mat_add_repeat(Z1, Z1, b1, nn.layers_node_count[1], nn.batch_size, 1);
        mat_copy(Z1, A1, nn.layers_node_count[1] * nn.batch_size);

        //mat_apply_relu(A1, network.layers_node_count[1] * network.batch_size);
        mat_apply_sigmoid(A1, nn.layers_node_count[1] * nn.batch_size);

        mat_multiply(Z2, W2, A1, nn.weights_sizes[1][0], nn.weights_sizes[1][1], nn.batch_size);
        mat_add_repeat(Z2, Z2, b2, nn.layers_node_count[2], nn.batch_size, 1);
        mat_copy(Z2, A2, nn.layers_node_count[2] * nn.batch_size);

        //mat_apply_softmax(A2, network.layers_node_count[2], network.batch_size);
        mat_apply_sigmoid(A2, nn.layers_node_count[2] * nn.batch_size);

        // CALCULATE DEVIATION

        mat_substract(dZ2, A2, Y, 1, nn.batch_size);
        mat_transpose(A1_T, A1, nn.layers_node_count[1], nn.batch_size);
        mat_multiply(dW2, dZ2, A1_T, nn.layers_node_count[2], nn.batch_size, nn.layers_node_count[1]);
        mat_multiply_scalar(dW2, dW2, (float)1/nn.batch_size, nn.layers_node_count[2], nn.layers_node_count[1]);
        db2 = mat_sum(dZ2, nn.layers_node_count[2] * nn.batch_size) / nn.batch_size;

        mat_copy(A1, dReluZ1, nn.layers_node_count[1] * nn.batch_size);
        //mat_apply_drelu(dReluZ1, network.layers_node_count[1] * network.batch_size);
        mat_apply_dsigmoid(dReluZ1, nn.layers_node_count[1] * nn.batch_size);

        mat_transpose(W2_T, W2, nn.weights_sizes[1][0], nn.weights_sizes[1][1]);
        
        mat_multiply(dZ1, W2_T, dZ2, nn.weights_sizes[1][1], nn.weights_sizes[1][0], nn.batch_size);
        mat_multiply_hadamard(dZ1, dZ1, dReluZ1, nn.layers_node_count[1], nn.batch_size);

        mat_transpose(X_T, X, 2, 4);
        mat_multiply(dW1, dZ1, X_T, nn.layers_node_count[1], nn.batch_size, 2);
        mat_multiply_scalar(dW1, dW1, (float)1/nn.batch_size, nn.layers_node_count[1], nn.layers_node_count[0]);
        db1 = mat_sum(dZ1, nn.layers_node_count[1] * nn.batch_size) / nn.batch_size;
        
        // UPDATE INTERNAL PARAMETERS
        mat_multiply_scalar(dW1, dW1, nn.learning_rate, nn.layers_node_count[1], nn.layers_node_count[0]);
        mat_multiply_scalar(dW2, dW2, nn.learning_rate, nn.layers_node_count[2], nn.layers_node_count[1]);
        db1 *= nn.learning_rate;
        db2 *= nn.learning_rate;

        mat_substract(W1, W1, dW1, nn.layers_node_count[1], nn.layers_node_count[0]);
        mat_substract(W2, W2, dW2, nn.layers_node_count[2], nn.layers_node_count[1]);
        mat_substract_ew(b1, b1, db1, nn.layers_node_count[1], 1);
        mat_substract_ew(b2, b2, db2, nn.layers_node_count[2], 1);

        float deviation = 0;
        for (int i = 0; i < nn.batch_size; i++) {
            deviation += fabsf(Y[i] - A2[i]);
        }
        deviation /= nn.batch_size;
        if (k % 100 == 0) {
            printf("\rEPOCH: %7d ~> %.5f", k, deviation);
            fflush(stdout);
        }
        fprintf(outputFile, "%d,%.5f\n", k, deviation);
        fflush(outputFile);
    }
    printf("\n");

    save_network(&nn);

    free(Z1);
    free(A1);
    free(Z2);
    free(A2);
    free(dZ1);
    free(dW1);
    free(dZ2);
    free(dW2);
    free(A1_T);
    free(dReluZ1);
    free(W2_T);
    free(X_T);

    for (int i = 0; i < nn.layer_count - 1; i++) {
        free(nn.weights[i]);
        free(nn.bias[i]);
    }

    free(nn.weights);
    free(nn.bias);
    free(nn.layers);
    fclose(outputFile);

    return 0;
}

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
    for (int k = 0; k < nn->layer_count-1; k++) {
        size_t len = nn->weights_sizes[k][0] * nn->weights_sizes[k][1];
        for (size_t c = 0; c < len; c++) {
            fprintf(file,
                    c == len - 1 ? "%f" : "%f,",
                    nn->weights[k][c]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "--b\n");
    for (int k = 0; k < nn->layer_count-1; k++) {
        size_t len = nn->layers_node_count[k + 1];
        for (size_t c = 0; c < len; c++) {
            fprintf(file,
                    c == len - 1 ? "%f" : "%f,",
                    nn->bias[k][c]);
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

    char line[500];
    int n = 0;
    enum Mode mode = HYPERPARAMS;
    while (fgets(line, 500, file) != NULL) {
        if (strncmp(line, "--", 2) == 0) {
            char *modestr = line + 2;
            if (strncmp(modestr, "l", 1) == 0) {
                mode = LAYERS;
                nn->layers = calloc(nn->layer_count, sizeof(Layer));
                nn->weights = malloc((nn->layer_count - 1) * sizeof(float *));
                nn->bias = malloc((nn->layer_count - 1) * sizeof(float *));
                nn->layers_node_count = malloc(nn->layer_count * sizeof(int));
            } else if (strncmp(modestr, "w", 1)== 0) {
                mode = WEIGHTS;
                for (int i = 1; i < nn->layer_count; i++) {
                    nn->weights_sizes[i-1][0] = nn->layers_node_count[i];
                    nn->weights_sizes[i-1][1] = nn->layers_node_count[i-1];
                    nn->weights[i-1] = malloc(nn->weights_sizes[i-1][0] *
                                        nn->weights_sizes[i-1][1] * sizeof(float));
                    nn->bias[i-1] = calloc(nn->layers_node_count[i], sizeof(float));
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

void setup_network(NeuralNetwork *nn, int *layers_node_count, int batch_size, float learning_rate, int layer_count) {
    nn->weights = malloc((layer_count - 1) * sizeof(float *));
    nn->bias = malloc((layer_count - 1) * sizeof(float *));
    nn->layers = calloc(layer_count, sizeof(Layer));
    nn->batch_size = batch_size;

    for (int i = 1; i < layer_count; i++) {
        nn->weights_sizes[i-1][0] = layers_node_count[i];
        nn->weights_sizes[i-1][1] = layers_node_count[i-1];
        nn->weights[i-1] = malloc(nn->weights_sizes[i-1][0] *
                                  nn->weights_sizes[i-1][1] * sizeof(float));
        nn->bias[i-1] = calloc(layers_node_count[i], sizeof(float));
        nn->layers[i].data = malloc(layers_node_count[i] * nn->batch_size * sizeof(float));
        nn->layers[i].z = malloc(layers_node_count[i] * nn->batch_size * sizeof(float));
        nn->layers[i].dz = malloc(layers_node_count[i] * nn->batch_size * sizeof(float));
        nn->layers[i].size = layers_node_count[i];
        nn->layers[i].disabled = calloc(layers_node_count[i], sizeof(char));
        mat_randomize(nn->weights[i-1], nn->weights_sizes[i-1][0] *
                                      nn->weights_sizes[i-1][1]);
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

void print_pixel(float* img, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float a = img[x * 28 + y] * 255;
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

void get_input(float *res, char *image_path) {
    SDL_Surface *surface;
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
            res[x * 28 + y] = (float)average / 255;
        }
    }

    SDL_FreeSurface(surface);
}
