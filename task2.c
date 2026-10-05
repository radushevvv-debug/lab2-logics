#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE_COUNT 3
#define DATA_TYPE_COUNT 4
#define ALGORITHM_COUNT 3

typedef enum {
    DATA_RANDOM,
    DATA_ASCENDING,
    DATA_DESCENDING,
    DATA_ASCENDING_DESCENDING
} DataType;

typedef void (*SortFunction)(int *array, int size);

typedef struct {
    const char *name;
    SortFunction sort;
} SortAlgorithm;

double seconds_between(clock_t start, clock_t end)
{
    return (double)(end - start) / CLOCKS_PER_SEC;
}

int *allocate_array(int size)
{
    int *array = (int *)malloc((size_t)size * sizeof(int));

    if (array == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    return array;
}

void fill_array(int *array, int size, DataType type)
{
    int i;
    int middle = size / 2;

    for (i = 0; i < size; i++) {
        switch (type) {
        case DATA_RANDOM:
            array[i] = rand() % 100000;
            break;

        case DATA_ASCENDING:
            array[i] = i;
            break;

        case DATA_DESCENDING:
            array[i] = size - i;
            break;

        case DATA_ASCENDING_DESCENDING:
            if (i < middle) {
                array[i] = i;
            } else {
                array[i] = size - i;
            }
            break;
        }
    }
}

const char *data_type_name(DataType type)
{
    switch (type) {
    case DATA_RANDOM:
        return "Random";
    case DATA_ASCENDING:
        return "Ascending";
    case DATA_DESCENDING:
        return "Descending";
    case DATA_ASCENDING_DESCENDING:
        return "Ascending-descending";
    default:
        return "Unknown";
    }
}

void shell_sort(int *array, int size)
{
    int gap;
    int i;

    for (gap = size / 2; gap > 0; gap /= 2) {
        for (i = gap; i < size; i++) {
            int current = array[i];
            int j = i;

            while (j >= gap && array[j - gap] > current) {
                array[j] = array[j - gap];
                j -= gap;
            }

            array[j] = current;
        }
    }
}

void quick_sort_recursive(int *array, int left, int right)
{
    int i = left;
    int j = right;
    int pivot = array[(left + right) / 2];

    while (i <= j) {
        while (array[i] < pivot) {
            i++;
        }

        while (array[j] > pivot) {
            j--;
        }

        if (i <= j) {
            int temporary = array[i];
            array[i] = array[j];
            array[j] = temporary;

            i++;
            j--;
        }
    }

    if (left < j) {
        quick_sort_recursive(array, left, j);
    }

    if (i < right) {
        quick_sort_recursive(array, i, right);
    }
}

void quick_sort(int *array, int size)
{
    if (size > 1) {
        quick_sort_recursive(array, 0, size - 1);
    }
}

int compare_integers(const void *left, const void *right)
{
    int first = *(const int *)left;
    int second = *(const int *)right;

    return (first > second) - (first < second);
}

void standard_qsort(int *array, int size)
{
    qsort(array, (size_t)size, sizeof(int), compare_integers);
}

int is_sorted(const int *array, int size)
{
    int i;

    for (i = 1; i < size; i++) {
        if (array[i - 1] > array[i]) {
            return 0;
        }
    }

    return 1;
}

double measure_sort_time(
    SortFunction sort,
    const int *source,
    int size
)
{
    int *copy = allocate_array(size);
    int repeats = 1;
    const int max_repeats = 32768;
    double total_time;
    int i;

    do {
        total_time = 0.0;

        for (i = 0; i < repeats; i++) {
            clock_t start;
            clock_t end;

            memcpy(copy, source, (size_t)size * sizeof(int));

            start = clock();
            sort(copy, size);
            end = clock();

            total_time += seconds_between(start, end);
        }

        if (total_time < 0.05 && repeats < max_repeats) {
            repeats *= 2;
        } else {
            break;
        }
    } while (1);

    if (!is_sorted(copy, size)) {
        printf("Sorting error\n");
    }

    free(copy);

    return total_time / repeats;
}

int main(void)
{
    int sizes[SIZE_COUNT] = {100, 1000, 10000};

    DataType data_types[DATA_TYPE_COUNT] = {
        DATA_RANDOM,
        DATA_ASCENDING,
        DATA_DESCENDING,
        DATA_ASCENDING_DESCENDING
    };

    SortAlgorithm algorithms[ALGORITHM_COUNT] = {
        {"Shell sort", shell_sort},
        {"Quick sort", quick_sort},
        {"qsort", standard_qsort}
    };

    int size_index;
    int type_index;
    int algorithm_index;

    srand(1);

    printf("Task 2. Sorting algorithms\n\n");

    printf("+--------+----------------------+--------------+------------------+\n");
    printf("| n      | Data                 | Algorithm    | Time, sec        |\n");
    printf("+--------+----------------------+--------------+------------------+\n");

    for (size_index = 0; size_index < SIZE_COUNT; size_index++) {
        int size = sizes[size_index];

        for (type_index = 0;
             type_index < DATA_TYPE_COUNT;
             type_index++) {
            int *source = allocate_array(size);

            fill_array(source, size, data_types[type_index]);

            for (algorithm_index = 0;
                 algorithm_index < ALGORITHM_COUNT;
                 algorithm_index++) {
                double time = measure_sort_time(
                    algorithms[algorithm_index].sort,
                    source,
                    size
                );

                printf(
                    "| %6d | %-20s | %-12s | %16.9f |\n",
                    size,
                    data_type_name(data_types[type_index]),
                    algorithms[algorithm_index].name,
                    time
                );
            }

            free(source);
        }
    }

    printf("+--------+----------------------+--------------+------------------+\n");

    return 0;
}