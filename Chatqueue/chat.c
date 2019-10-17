#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

typedef struct {
    long mtype;
    char mtext[256];
} Mybuf;


int main(int argc, char *argv[]) {

    if (argc == 1) {
        printf("\e[;35mOpen second terminal and Enter two or more arguments for chatting.\e[m\n");
    }

    long typew = 0;
    long typer = 0;
    if (argc == 1) {
        typew = 1;
        typer = 255;
    } else {
        typew = 255;
        typer = 1;
    }

    pid_t chpid;
    int msqid;
    char pathname[] = "key";
    key_t key = 0;

    // Создание ключа для очереди
    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

    // Создание очереди
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can't get msgid\n");
        exit(-1);
    }

    // Распараллеливание на процесс чтения и написания
    chpid = fork();
    if (chpid == -1) {
        // Ошибка
        printf("\e[;31mCan't fork a child process\e[m\n");
        exit(-1);
    } else if (chpid > 0) {
        // Родительский процесс (написание)
        char messageout[1025];

        Mybuf mybuf1;
        // Время
        struct tm *u1;
        time_t timer1;
        size_t len1;

        // Ввод имени не более 20 символов

        printf("\e[;35mEnter your Name for chatting\e[m\n");
        printf("\e[;35mThe name must be less than 20 characters long: \e[m");

        fgets(messageout, 256, stdin);
        messageout[strlen(messageout) - 1] = '\0';

        while (strlen(messageout) > 20) {
            printf("\e[5;31mWrong name\e[m\n");
            printf("\e[;35mTry other: \e[m");
            fgets(messageout, 256, stdin);
            messageout[strlen(messageout) - 1] = '\0';
        }
        mybuf1.mtype = typew;
        strcpy(mybuf1.mtext, messageout);
        len1 = strlen(mybuf1.mtext) + 1;
        // Отправка имя второму пользователю
        if (msgsnd(msqid, (Mybuf *) &mybuf1, len1, 0) < 0) {
            printf("Can't send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }

        // Написание сообщений
        while (1) {
            fgets(messageout, 256, stdin);
            messageout[strlen(messageout) - 1] = '\0';
            if (strlen(messageout) != 0) {
                mybuf1.mtype = typew;
                strcpy(mybuf1.mtext, messageout);
                len1 = strlen(mybuf1.mtext) + 1;
                // Проверка на выход из системы
                if (strcmp(messageout, "Exit") == 0) {
                    if (msgsnd(msqid, (Mybuf *) &mybuf1, len1, 0) < 0) {
                        printf("Can't send message to queue\n");
                        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                        exit(-1);
                    }
                    kill(chpid, SIGKILL);
                    exit(0);
                } else {
                    // Вывод на экран и запись в queue + время
                    timer1 = time(NULL);
                    u1 = localtime(&timer1);
                    printf("\e[;32m%d:%d:%d_____You: %s\e[m\n", u1->tm_hour, u1->tm_min, u1->tm_sec, mybuf1.mtext);
                    if (msgsnd(msqid, (Mybuf *) &mybuf1, len1, 0) < 0) {
                        printf("Can't send message to queue\n");
                        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                        exit(-1);
                    }
                }
            }
        }

    } else {
        // Порожденный процесс
        // Время
        struct tm *u2;
        time_t timer2;
        char nameper[21];
        Mybuf mybuf2;
        // Получение имени
        if (msgrcv(msqid, (Mybuf *) &mybuf2, 256, typer, 0) < 0) {
            printf("Can't receive message from queue\n");
            exit(-1);
        }
        strcpy(nameper, mybuf2.mtext);
        // Прием сообщений
        while (1) {
            if (msgrcv(msqid, (Mybuf *) &mybuf2, 256, typer, 0) < 0) {
                printf("Can't receive message from queue\n");
                exit(-1);
            }
            // Проверка на выход из системы
            if (strcmp(mybuf2.mtext, "Exit") == 0) {
                printf("\e[;36m%s is disconnected\e[m\n", nameper);
                msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
                kill(getppid(), SIGKILL);
                exit(0);
            } else {
                // Печать сообщения
                timer2 = time(NULL);
                u2 = localtime(&timer2);
                printf("\e[;34m%d:%d:%d_____%s : %s\e[m\n", u2->tm_hour, u2->tm_min, u2->tm_sec, nameper, mybuf2.mtext);
            }
        }
    }
}