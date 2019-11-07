#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <memory.h>

typedef struct {
    long mtype;
    pid_t id;
} Mybuf;

void mute (pid_t own, int msqid, int msqid1);

void umute (pid_t own, int msqid);

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
    int lol = 0;
    while (lol != 100) {
        mute(getpid(), msqid, msqid1);
//        sleep(1);
        memory[0] ++;
        umute(getpid(), msqid);
        lol++;
    }
    return 0;
}

void mute (pid_t own, int msqid, int msqid1) {
    Mybuf mybuf;
    mybuf.mtype = 1;
    mybuf.id = own;
    if (msgsnd(msqid, (Mybuf *) &mybuf, sizeof(pid_t), 0) < 0) {
        printf("Can't send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    if (msgrcv(msqid1, (Mybuf *) &mybuf, sizeof(pid_t), own, 0) < 0) {
        printf("Can't receive message from queue\n");
        exit(-1);
    }
}

void umute (pid_t own, int msqid) {
    Mybuf mybuf;
    mybuf.mtype = own;
    if (msgsnd(msqid, (Mybuf *) &mybuf, sizeof(pid_t), 0) < 0) {
        printf("Can't send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }
}
