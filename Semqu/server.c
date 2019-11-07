#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <memory.h>

typedef struct {
    long mtype;
    pid_t id;
} Mybuf;

int main() {
    int shmid;

    char pathname[] = "key";

    key_t key = 0;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

    //  Пытаемся найти разделяемую память по сгенерированному ключу
    if ((shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT)) < 0) {
        printf("Can't create shared memory\n");
        exit(-1);
    }

    int *memory;

    //  Пытаемся отобразить разделяемую память в адресное пространство текущего процесса
    if ((memory = (int *) shmat(shmid, NULL, 0)) == (int *) (-1)) {
        printf("Can't attach shared memory\n");
        exit(-1);
    }

    int msqid;

    if ((key = ftok(pathname, 1)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can't get msgid\n");
        exit(-1);
    }

    int msqid1;

    if ((key = ftok(pathname, 2)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

    if ((msqid1 = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can't get msgid\n");
        exit(-1);
    }

    pid_t cur = 0;

    while (*memory != 200) {
        Mybuf mybuf;
        if (msgrcv(msqid, (Mybuf *) &mybuf, sizeof(pid_t), 1, 0) < 0) {
            printf("Can't receive message from queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            msgctl(msqid1, IPC_RMID, (struct msqid_ds *) NULL);
            shmdt(memory);
            exit(-1);
        }
        cur = mybuf.id;
        printf("%s %d %ld\n", "FIRST", mybuf.id, mybuf.mtype);
        mybuf.mtype = cur;

        if (msgsnd(msqid1, (Mybuf *) &mybuf, sizeof(pid_t), 0) < 0) {
            printf("Can't send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            msgctl(msqid1, IPC_RMID, (struct msqid_ds *) NULL);
            shmdt(memory);
            exit(-1);
        }

        if (msgrcv(msqid, (Mybuf *) &mybuf, sizeof(pid_t), cur, 0) < 0) {
            printf("Can't receive message from queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            msgctl(msqid1, IPC_RMID, (struct msqid_ds *) NULL);
            shmdt(memory);
            exit(-1);
        }
        cur = mybuf.id;
        printf("%s %d %ld\n", "SECOND", mybuf.id, mybuf.mtype);
        printf("%d\n", *memory);
        mybuf.mtype = cur;
    }
    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
    msgctl(msqid1, IPC_RMID, (struct msqid_ds *) NULL);
    if(shmdt(memory) < 0){
        printf("Can't detach shared memory\n");
        exit(-1);
    }

    return 0;
}