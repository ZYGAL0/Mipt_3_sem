// Семинар по нитям + mutex

#include <pthread.h>
#include <stdio.h>

pthread_mutex_t lock;
int counter = 0;
const long long int ITERATIONS = 10000000;

void* routine(void* arg);

int main() {
    pthread_mutex_init(&lock, NULL);
    pthread_t tid = 0;
    pthread_create(&tid, NULL, routine, NULL);

    for (int i = 0; i < ITERATIONS; ++i) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }

    pthread_join(tid, NULL);
    pthread_mutex_destroy(&lock);

    printf("Results. Actual: %d, expected: %lld\n", counter, ITERATIONS * 2);
    return 0;
}

void* routine(void* arg) {
    for (int i = 0; i < ITERATIONS; ++i) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}