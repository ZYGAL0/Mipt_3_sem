// bakery algorithm для поочередного доступа к разделяемой памяти

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 100

int Entering[N] = {0};
int Number[N] = {0};

int buns = 0;

void lock(long thread) {
    Entering[thread] = 1;

    int max_number = 0;

    for (int i = 0; i < N; ++i) {
        if (max_number < Number[i]) {
            max_number = Number[i];
        }
    }

    Number[thread] = max_number + 1;

    Entering[thread] = 0;

    for (int i = 0; i < N; ++i) {

        while (Entering[i]);

        while (Number[i] != 0 &&
               (Number[i] < Number[thread] ||
                (Number[i] == Number[thread] &&
                 i < thread)));
    }
}

void unlock(long thread) {
    Number[thread] = 0;
}

void CriticalSection(long thread) {

    printf("Process %ld with number %d in critical section\n", thread, Number[thread]);
    buns += 1;

}

void *mythread(void *arg) {

    long thread = (long) arg;
    lock(thread);
    CriticalSection(thread);
    unlock(thread);
    return NULL;

}

int main() {
    pthread_t thid[N];

    int result;

    for (int i = 0; i < N; i++) {
        result = pthread_create(&thid[i], (pthread_attr_t *) NULL, mythread, (void *) (long) i);

        if (result != 0) {
            printf("Error on thread create, return value = %d\n", result);
            exit(-1);
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(thid[i], (void **) NULL);
    }

    printf("%d\n", buns);

    return 0;
}