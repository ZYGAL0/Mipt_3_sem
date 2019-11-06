#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Child and Parents communicate by pipe with sem

int cmp(const void *a, const void *b) {
    return *(char*)a - *(char*)b;
}

int main() {
    int semid;
    char pathname[] = "sem1";
    key_t key;
    struct sembuf mybuf;

    key = ftok(pathname, 0);

    if ((semid = semget(key, 2, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create semaphore set\n");
        exit(-1);
    }

    mybuf.sem_num = 0;
    mybuf.sem_flg = 0;

    printf("Enter number of strings: ");
    int num;
    scanf("%d\n", &num);

    size_t size = 0;
    int fd[2];
    if (pipe(fd) < 0) {
        printf("Can't create pipe\n");
        exit(1);
    }

    int res = fork();
    if (res == -1) {
        printf("Can't fork child\n");
        exit(-1);
    } else if (res == 0) {
        char mes[101];
        for (int i = 0; i < num; i++) {

            mybuf.sem_num = 0;
            mybuf.sem_op = -1;

            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            size = read(fd[0], mes, 101);
            if (size <= 0) {
                printf("Can't read from pipe\n");
                exit(-1);
            }

            qsort(mes, strlen(mes), sizeof(char), cmp);

            mybuf.sem_num = 1;
            mybuf.sem_op = 1;

            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            size = write(fd[1], mes, strlen(mes) + 1);
            if (size != (strlen(mes) + 1)) {
                printf("Can't write into pipe\n");
                exit(-1);
            }
        }
        close(fd[0]);
        close(fd[1]);
    } else {
        char mes[101];
        for (int i = 0; i < num; i++) {

            fgets(mes, 100, stdin);
            mes[strlen(mes) - 1] = '\0';

            size = write(fd[1], mes, (strlen(mes) + 1));
            if (size != (strlen(mes) + 1)) {
                printf("Can't write into pipe\n");
                exit(-1);
            }

            mybuf.sem_num = 0;
            mybuf.sem_op = 1;

            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            mybuf.sem_num = 1;
            mybuf.sem_op = -1;

            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            size = read(fd[0], mes, 100);
            if (size <= 0) {
                printf("Can't read from pipe\n");
                exit(-1);
            }
            printf("%s\n", mes);
        }
        close(fd[0]);
        close(fd[1]);
    }
    return 0;
}