#include "neural-net.h"
#include <time.h>

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
