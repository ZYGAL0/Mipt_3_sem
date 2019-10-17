#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

int cmp(const void *a, const void *b) {
    return *(char*)a - *(char*)b;
}

typedef struct {
    long mtype;
    char mtext[81];
} mybuf;

int main() {
    int msqid1 = 0;
    int msqid2 = 0;

    char pathname1[] = "server.c";
    char pathname2[] = "client.c";

    key_t key1 = 0;
    key_t key2 = 0;

    unsigned int len = 0;
    unsigned int number = 0;

    if ((key1 = ftok(pathname1, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((key2 = ftok(pathname2, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid1 = msgget(key1, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    if ((msqid2 = msgget(key2, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    while (1) {
        mybuf res;
        int maxlen = 81;
        if ((msgrcv(msqid1, (struct msgbuf *) &res, maxlen, 0, 0) < 0)) {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }

        if (res.mtype == LAST_MESSAGE) {
            msgctl(msqid1, IPC_RMID, (struct msqid_ds *) NULL);
            msgctl(msqid2, IPC_RMID, (struct msqid_ds *) NULL);
            exit(0);
        }
        qsort(res.mtext, strlen(res.mtext), sizeof(char), cmp);
        len = strlen(res.mtext) + 1;

        if (msgsnd(msqid2, (struct msgbuf *) &res, len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid1, IPC_RMID, (struct msqid_ds *) NULL);
            msgctl(msqid2, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
        number++;
        printf("Number of clients processed: %d\n", number);
    }
}
