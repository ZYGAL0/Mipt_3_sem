// Робот с двумя ногами, которые передвигаются поочередно

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *mythread(void *dummy) {
    printf("Left\n");
    return NULL;
}

int main() {
    pthread_t thid;

    int result;

    while (1) {
        printf("Right\n");
        sleep(1);
        result = pthread_create( &thid, (pthread_attr_t *)NULL, mythread, NULL);

        if(result != 0){
            printf ("Error on thread create, return value = %d\n", result);
            exit(-1);
        }

        pthread_join(thid, (void **)NULL);
        sleep(1);
    }

    return 0;
}
