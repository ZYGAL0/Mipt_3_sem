// Робот с n-м количеством ног, которые передвигаются поочередно
// Ввод: имя программы и количество ног n

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *mythread(void *dummy) {
    int *num = (int *) dummy;
    printf("the leg %d was moved\n", *num);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thid;

    int n = atoi(argv[1]);

    printf("Number of legs: %d\n", n );

    int result;

    while (1) {
        for (int i = 1; i <= n; i ++) {
            result = pthread_create( &thid, (pthread_attr_t *)NULL, mythread, &i);

            if(result != 0){
                printf ("Error on thread create, return value = %d\n", result);
                exit(-1);
            }
            pthread_join(thid, (void **)NULL);
            sleep(1);
        }
    }

}
