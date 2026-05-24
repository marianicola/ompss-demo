#define N 1024
#define BLOCK_SIZE 256
#define NUM_BLOCKS (N / BLOCK_SIZE)

void partial_dot_product(double *x, double *y, int start, int size, double *partial) {
    double sum = 0.0;

    for (int i = start; i < start + size; i++) {
        sum += x[i] * y[i];
    }

    *partial = sum;
}

void reduce_partials(double *partial, int num_blocks, double *result) {
    double sum = 0.0;

    for (int i = 0; i < num_blocks; i++) {
        sum += partial[i];
    }

    *result = sum;
}

int main() {
    double x[N], y[N];
    double partial[NUM_BLOCKS];
    double result = 0.0;

    // Initialization of x and y is omitted for simplicity.

    for (int k = 0; k < NUM_BLOCKS; k++) {
        int start = k * BLOCK_SIZE;

        #pragma oss task in(x[start;BLOCK_SIZE], y[start;BLOCK_SIZE]) out(partial[k])
        partial_dot_product(x, y, start, BLOCK_SIZE, &partial[k]);
    }

    #pragma oss task in(partial[0;NUM_BLOCKS]) out(result)
    reduce_partials(partial, NUM_BLOCKS, &result);

    #pragma oss taskwait

    return 0;
}