#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

/* define derived values from the variables */
int num_threads, length;
int NUMPER_THREAD;
int OFFSET;
int *arr;

/* function definitions */
void open_input(char *argv[], int length);
void check_args(int argc);
int rand_generate(unsigned int lower_bound, unsigned int upper_bound);
void merge_sort(int arr[], int left, int right);
void merge(int arr[], int left, int middle, int right);
void* merge_threads(void* arg);
void merge_sections(int arr[], int number, int ag);

int main(int argc, char *argv[]) {
    struct timeval  start, end;
    double time_spent;
    FILE *output;

    check_args(argc);

    num_threads = atoi(argv[1]);
    length = atoi(argv[2]);
    NUMPER_THREAD = length / num_threads;
    OFFSET = length % num_threads;
    arr = malloc(length*sizeof(int));
    int *reg_arr = malloc(length*sizeof(int));

    /* open input file to allocate unsorted array */
    open_input(argv, length);

    /* make copy of original array for seq sort */
    memcpy(reg_arr, arr, length*sizeof(int));

    /* begin timing 1 */
    pthread_t threads[num_threads];
    gettimeofday(&start, NULL);
    
    /* create threads */
    for (long i = 0; i < num_threads; i ++) {
        pthread_create(&threads[i], NULL, merge_threads, (void *)i);
    }
    
    /* wait for threads to terminate */
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    /* merge sections of sorted array */
    merge_sections(arr, num_threads, 1);
    
    /* end timing 1 */
    gettimeofday(&end, NULL);
    time_spent = ((double) ((double) (end.tv_usec - start.tv_usec) / 1000000 +
                            (double) (end.tv_sec - start.tv_sec)));
    printf("Execution time (multithreaded): %f seconds\n", time_spent);
    
    /* begin timing 2 */
    gettimeofday(&start, NULL);
    merge_sort(reg_arr, 0, length - 1);

    /* end timing 2 */
    gettimeofday(&end, NULL);
    time_spent = ((double) ((double) (end.tv_usec - start.tv_usec) / 1000000 +
                            (double) (end.tv_sec - start.tv_sec)));
    printf("Execution time (sequential): %f seconds\n", time_spent);

    /* open output file for writing */
    output = fopen("sorted.txt", "w");
    for (int i = 0; i < length; i++){
        fprintf(output, "%d\n", reg_arr[i]);
    }

    return 0;
}

void open_input(char *argv[], int length){
    /* read input file */
    FILE *input = fopen(argv[3], "r");
    for(int j = 0; j < length; j++){
        fscanf(input, "%d", &arr[j]);
    }
    fclose(input);
}

/* error check for arguments */
void check_args(int argc){
	if (argc < 4){
		printf("Invalid number of arguments: multithreadsort [# threads] [array length] [input text file]\n");
		exit(1);
	}
}

/* generate random numbers within the specified limit */
int rand_generate(unsigned int lower_bound, unsigned int upper_bound) {
    return lower_bound + (upper_bound - lower_bound) * ((double)rand() / RAND_MAX);
}

/* merge locally sorted sections */
void merge_sections(int arr[], int number, int ag) {
    for(int i = 0; i < number; i = i + 2) {
        int left = i * (NUMPER_THREAD * ag);
        int right = ((i + 2) * NUMPER_THREAD * ag) - 1;
        int middle = left + (NUMPER_THREAD * ag) - 1;
        if (right >= length) {
            right = length - 1;
        }
        merge(arr, left, middle, right);
    }
    if (number / 2 >= 1) {
        merge_sections(arr, number / 2, ag * 2);
    }
}

/** assigns work to each thread to perform merge sort */
void *merge_threads(void* arg)
{
    int thread_id = (long)arg;
    int l = thread_id * (NUMPER_THREAD);
    int r = (thread_id + 1) * (NUMPER_THREAD) - 1;
    if (thread_id == num_threads - 1) {
        r += OFFSET;
    }
    int m = l + (r - l) / 2;
    if (l < r) {
        merge_sort(arr, l, r);
        merge_sort(arr, l + 1, r);
        merge(arr, l, m, r);
    }
    return NULL;
}

/* perform merge sort */
void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        merge_sort(arr, left, middle);
        merge_sort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

/* merge function */
void merge(int arr[], int left, int middle, int right) {
    int i = 0;
    int j = 0;
    int k = 0;
    int left_length = middle - left + 1;
    int right_length = right - middle;
    int left_array[left_length];
    int right_array[right_length];
    
    /* copy values to left array */
    for (int i = 0; i < left_length; i ++) {
        left_array[i] = arr[left + i];
    }
    
    /* copy values to right array */
    for (int j = 0; j < right_length; j ++) {
        right_array[j] = arr[middle + 1 + j];
    }
    
    i = 0;
    j = 0;
    /** chose from right and left arrays and copy */
    while (i < left_length && j < right_length) {
        if (left_array[i] <= right_array[j]) {
            arr[left + k] = left_array[i];
            i ++;
        } else {
            arr[left + k] = right_array[j];
            j ++;
        }
        k ++;
    }
    
    /* copy the remaining values to the array */
    while (i < left_length) {
        arr[left + k] = left_array[i];
        k ++;
        i ++;
    }
    while (j < right_length) {
        arr[left + k] = right_array[j];
        k ++;
        j ++;
    }
}