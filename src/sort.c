#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "benchmark.h"


void insertion_sort(struct benchmark_input *binput) {

    printf("INSIDE");
    fflush(stdout);

    int64_t *data = (int64_t *)binput->data;
    uint32_t size = binput->size;

    int32_t i;
    uint32_t j;
    int64_t key;

    for (j = 1; j < size; j++) {
        i = j-1;
        key = data[j];

        while(data[i] > key) {
            data[i+1] = data[i];
            i--;
        }

        data[i+1] = key;
    }
}


void generate_array(
    struct benchmark_input *binput, 
    uint32_t size
) {
    binput->size = size;
    binput->data = malloc(size * sizeof(int64_t));

    int64_t *_data = (int64_t *)binput->data;

    for (uint64_t i=0; i<size; i++) {
        _data[i] = rand();
    }
}


algorithm_ptr select_sorting_algorithm(char *algo_name) {
    if (strcmp(algo_name, "INSERTION") == 0) {
        return &insertion_sort;
    }
    else {
        printf("Algoritmo %s non selezionato.", algo_name);
        exit(-1);
    }
}


int test_sort(struct benchmark_input *binput) {
    int64_t *_data = (int64_t *)binput->data;

    for (uint64_t i=1; i<binput->size; i++) {
        if (_data[i] < _data[i-1]) {
            return 0;
        }
    }

    return 1;
}



