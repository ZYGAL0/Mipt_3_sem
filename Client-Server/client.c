#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LAST_MESSAGE 255

typedef struct {
    long mtype;
    char mtext[81];
} mybuf;

int cmp(const void *a, const void *b);

int main() {
    int msqid1 = 0;
    int msqid2 = 0;

    char pathname1[] = "server.c";
    char pathname2[] = "client.c";

    key_t key1 = 0;
    key_t key2 = 0;

    unsigned int len = 0;

    if ((key1 = ftok(pathname1, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((key2 = ftok(pathname2, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid1 = msgget(key1, 0666)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }
    if ((msqid2 = msgget(key2, 0666)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    mybuf res;
    res.mtype = getpid();
    printf("Enter string for sort: ");
    scanf("%s", res.mtext);
    res.mtext[strlen(res.mtext)] = '\0';

    if (res.mtext[0] == 'E' && strlen(res.mtext) == 1) {
        res.mtype = LAST_MESSAGE;
        len = 0;

        if (msgsnd(msqid1, (struct msgbuf *) &res,
                   len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid1, IPC_RMID,(struct msqid_ds *) NULL);
            exit(-1);
        }
    }
    len = strlen(res.mtext) + 1;

    if (msgsnd(msqid1, (struct msgbuf *) &res, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid1, IPC_RMID,(struct msqid_ds *) NULL);
        exit(-1);
    }

    int maxlen = 84;

    if ((msgrcv(msqid2, (struct msgbuf *) &res, maxlen, getpid(), 0) < 0)) {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }

    printf("Result: %s\n", res.mtext);

    return 0;
}

int cmp(const void *a, const void *b) {
    return *(char*)a - *(char*)b;
}

