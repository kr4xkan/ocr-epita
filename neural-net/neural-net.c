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

    if (nn->csv != NULL)
        fclose(nn->csv);
}

void free_layer(Layer* layer) {
    free_matrix(&layer->A);
    free_matrix(&layer->Z);
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

// size - activation - W - B
void serialize_layer(Buffer* buf, Layer* layer) {
    size_t required_size = sizeof(enum ActivationFunction) + sizeof(size_t);
    reserve_space(buf, required_size);
    write_buffer(buf, &layer->size, sizeof(size_t));
    write_buffer(buf, &layer->activation, sizeof(enum ActivationFunction));
    serialize_matrix(buf, &layer->W);
    serialize_matrix(buf, &layer->B);
}

Layer deserialize_layer(Buffer* buf) {
    Layer l;
    size_t i;
    read_buffer(buf, &i, sizeof(size_t));
    l.size = i;
    read_buffer(buf, &i, sizeof(enum ActivationFunction));
    l.activation = i;
    l.W = deserialize_matrix(buf);
    l.B = deserialize_matrix(buf);
    l.A = new_matrix(l.size, 1);
    l.Z = new_matrix(l.size, 1);
    return l;
}

NeuralNetwork new_network(double learning_rate) {
    NeuralNetwork nn;
    nn.layer_count = 3;
    nn.learning_rate = learning_rate;
    nn.decay = 0.000001;
    nn.momentum = 0.5;
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

// layer_count - iterations - current_learning_rate - learning_rate - decay - momentum - layers
void serialize_network(Buffer* buf, NeuralNetwork* nn) {
    size_t required_size = sizeof(double) * 4 + sizeof(size_t) * 2;
    reserve_space(buf, required_size);

    write_buffer(buf, &nn->layer_count, sizeof(size_t));
    write_buffer(buf, &nn->updates.iterations, sizeof(size_t));
    write_buffer(buf, &nn->updates.current_learning_rate, sizeof(double));
    write_buffer(buf, &nn->learning_rate, sizeof(double));
    write_buffer(buf, &nn->decay, sizeof(double));
    write_buffer(buf, &nn->momentum, sizeof(double));

    for (size_t i = 1; i < nn->layer_count; i++)
        serialize_layer(buf, nn->layers + i);
}

NeuralNetwork deserialize_network(Buffer* buf) {
    NeuralNetwork nn;
    size_t i;
    read_buffer(buf, &i, sizeof(size_t));
    nn.layer_count = i;
    read_buffer(buf, &i, sizeof(size_t));
    nn.updates.iterations = i;
    read_buffer(buf, &nn.updates.current_learning_rate, sizeof(double));
    read_buffer(buf, &nn.learning_rate, sizeof(double));
    read_buffer(buf, &nn.decay, sizeof(double));
    read_buffer(buf, &nn.momentum, sizeof(double));
    nn.layers = malloc(nn.layer_count * sizeof(Layer));
    for (size_t i = 1; i < nn.layer_count; i++)
        nn.layers[i] = deserialize_layer(buf);

    nn.layers[0] = new_layer(ReLU, 784, 0);
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
    fflush(nn->csv);
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
    //multiply_ew(nn->updates.dZ2, nn->updates.dZ2, nn->updates.dZ2);
    //crossentropy(nn->layers[2].A, expected, nn->updates.dZ2);
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

    add(nn->layers[2].W, nn->updates.mW2, nn->layers[2].W);
    add(nn->layers[1].W, nn->updates.mW1, nn->layers[1].W);
    add(nn->layers[2].B, nn->updates.mB2, nn->layers[2].B);
    add(nn->layers[1].B, nn->updates.mB1, nn->layers[1].B);
    // sub(nn->layers[2].W, nn->updates.dW2, nn->layers[2].W);
    // sub(nn->layers[1].W, nn->updates.dW1, nn->layers[1].W);
    // sub(nn->layers[2].B, nn->updates.dB2, nn->layers[2].B);
    // sub(nn->layers[1].B, nn->updates.dB1, nn->layers[1].B);

    free_matrix(&W2T);
    free_matrix(&A1T);
    free_matrix(&A0T);
    free_matrix(&dRelu);

    nn->updates.iterations++;
    return sum_abs(nn->updates.dZ2);
}

void cmd_train(int argc, char **argv) {
    if (argc != 4)
        errx(1, "./neural-net --train "
                "[load,new] <dataset_path> <iterations> <learning_rate>");
    size_t iterations = atof(argv[2]);
    double learning_rate = atof(argv[3]);

    NeuralNetwork nn;
    if (strcmp(argv[0], "load") == 0) {
        Buffer* buf = load_buffer("save.nrl");
        nn = deserialize_network(buf);
        free_buffer(buf);
        printf("Loaded network from save\n");
    } else {
        nn = new_network(learning_rate);
    }

    train_network(&nn, argv[1], iterations);
    free_network(&nn);
}

void cmd_guess(int argc, char **argv) {
    if (argc != 1)
        errx(1, "./neural-net --guess <image_path>");

    NeuralNetwork nn;
    Buffer* buf = load_buffer("save.nrl");
    nn = deserialize_network(buf);
    free_buffer(buf);

    double* img = malloc(784 * sizeof(double));

    load_image(img, argv[0]);
    nn.layers[0].A.v = img;
    forward_pass(&nn);

    print_mat(nn.layers[2].A);

    free(img);
    free_network(&nn);
}

void cmd_test(int argc, char **argv) {
    if (argc != 1)
        errx(1, "./neural-net --test <image_path>");

    NeuralNetwork nn;
    Buffer* buf = load_buffer("save.nrl");
    nn = deserialize_network(buf);
    free_buffer(buf);

    szt len_dataset;
    LabeledImage* dataset = load_dataset(argv[0], &len_dataset);

    szt correct = 0;
    for (size_t i = 0; i < len_dataset; i++) {
        nn.layers[0].A.v = dataset[i].data;
        forward_pass(&nn);

        double max = nn.layers[2].A.v[0];
        size_t max_index = 0;
        for (size_t j = 1; j < 10; j++) {
            if (nn.layers[2].A.v[j] > max) {
                max_index = j;
                max = nn.layers[2].A.v[j];
            }
        }

        correct += max_index == dataset[i].label;
    }

    printf("%.2f%% correct (%zu/%zu)\n", (float)correct*100/len_dataset, correct, len_dataset);

    free_network(&nn);
    free(dataset);
}

int main(int argc, char **argv) {
    // Initialize randomizer
    srand((unsigned int)time(NULL));

    if (argc == 1)
        errx(1, "./neural-net --[train,guess]");

    if (strcmp(argv[1], "--train") == 0) {
        cmd_train(argc - 2, argv + 2);
    } else if (strcmp(argv[1], "--guess") == 0) {
        cmd_guess(argc - 2, argv + 2);
    } else if (strcmp(argv[1], "--test") == 0) {
        cmd_test(argc - 2, argv + 2);
    }

    return 0;
}

void train_network(NeuralNetwork *neural_net, char* dataset_path, size_t iterations) {
    NeuralNetwork nn = *neural_net;

    szt len_dataset;
    LabeledImage* dataset = load_dataset(dataset_path, &len_dataset);

    Matrix expected = new_matrix(10, 1);
    szt prev = 0;
    for (szt i = 0; i < iterations; i++) {
        // Randomize array
        // for (szt i = 0; i < len_dataset - 1; i++) {
        //     szt j = i + rand() / (RAND_MAX / (len_dataset - i) + 1);
        //     LabeledImage t = dataset[j];
        //     dataset[j] = dataset[i];
        //     dataset[i] = t;
        // }

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
        error /= len_dataset;
        print_stat(&nn, error);
        if (i % 10 == 0) {
            printf("[%zu] Error: %.4f    LR: %f\n", i, error, nn.updates.current_learning_rate);
            fflush(stdout);
        }
    }

    Buffer* buf = new_buffer();
    serialize_network(buf, &nn);
    printf("Serialized to buffer, size = %zu , capacity = %zu \n\n", buf->size, buf->capacity);
    save_buffer(buf, "save.nrl");
    free_buffer(buf);

    free_matrix(&expected);
    free(dataset);
}

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
