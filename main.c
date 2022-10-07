#include "cutter/cutter.h"
#include "preprocessing/preprocessing.h"
#include "neural-net/neural-net.h"
#include "solver/solver.h"
#include "ui/ui.h"

#include <time.h> 

int main() {
    clock_t t = clock();
    /*neural_test();*/
    DetectLines();
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
 
    printf("%f seconds to execute \n", time_taken);
    return 0;
}
