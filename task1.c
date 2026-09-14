#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE_COUNT 10

double seconds_between(clock_t start, clock_t end)
{
    return (double)(end - start) / CLOCKS_PER_SEC;
}

int *allocate_matrix(int n)
{
    int *matrix = (int *)malloc((size_t)n * n * sizeof(int));

    if (matrix == NULL) {
        printf("Memory allocation error for matrix %dx%d\n", n, n);
        exit(1);
    }

    return matrix;
}

void fill_matrix(int *matrix, int n)
{
    int i;

    for (i = 0; i < n * n; i++) {
        matrix[i] = rand() % 9 + 1;
    }
}

long long multiply_matrices(const int *a, const int *b, int *c, int n)
{
    int i;
    int j;
    int k;
    long long checksum = 0;

    for (i = 0; i < n * n; i++) {
        c[i] = 0;
    }

    for (i = 0; i < n; i++) {
        for (k = 0; k < n; k++) {
            int value = a[i * n + k];

            for (j = 0; j < n; j++) {
                c[i * n + j] += value * b[k * n + j];
            }
        }
    }

    for (i = 0; i < n * n; i++) {
        checksum += c[i];
    }

    return checksum;
}

double theoretical_function(int n)
{
    return (double)n * n * n;
}

double select_coefficient(const double *experiment, const double *function_values, int count)
{
    double numerator = 0.0;
    double denominator = 0.0;
    int i;

    for (i = 0; i < count; i++) {
        numerator += experiment[i] * function_values[i];
        denominator += function_values[i] * function_values[i];
    }

    return numerator / denominator;
}

double measure_time(int n)
{
    int *a = allocate_matrix(n);
    int *b = allocate_matrix(n);
    int *c = allocate_matrix(n);

    int repeats = 1;
    int max_repeats = 64;
    int r;

    clock_t start;
    clock_t end;

    double elapsed;
    long long checksum_sum = 0;

    fill_matrix(a, n);
    fill_matrix(b, n);

    do {
        start = clock();

        for (r = 0; r < repeats; r++) {
            checksum_sum += multiply_matrices(a, b, c, n);
        }

        end = clock();

        elapsed = seconds_between(start, end);

        if (elapsed < 0.05 && repeats < max_repeats) {
            repeats *= 2;
        } else {
            break;
        }
    } while (1);

    if (checksum_sum == -1) {
        printf("Impossible value\n");
    }

    free(a);
    free(b);
    free(c);

    return elapsed / repeats;
}

int main(void)
{
    int sizes[SIZE_COUNT] = {
        100, 200, 300, 400, 500,
        600, 700, 800, 900, 1000
    };

    double experiment[SIZE_COUNT];
    double function_values[SIZE_COUNT];

    double k;
    int i;

    srand(1);

    printf("Task 1. Matrix multiplication\n");
    printf("Analytical complexity: O(n^3)\n\n");

    for (i = 0; i < SIZE_COUNT; i++) {
        function_values[i] = theoretical_function(sizes[i]);
        experiment[i] = measure_time(sizes[i]);
    }

    k = select_coefficient(experiment, function_values, SIZE_COUNT);

    printf("Selected coefficient k = %.12e\n\n", k);

    printf("+--------+------------------+---------------------+---------------------+\n");
    printf("| n      | f(n) = n^3       | theoretical, sec    | experiment, sec     |\n");
    printf("+--------+------------------+---------------------+---------------------+\n");

    for (i = 0; i < SIZE_COUNT; i++) {
        double theoretical_time = k * function_values[i];

        printf("| %6d | %16.0f | %19.9f | %19.9f |\n",
               sizes[i],
               function_values[i],
               theoretical_time,
               experiment[i]);
    }

    printf("+--------+------------------+---------------------+---------------------+\n");

    return 0;
}