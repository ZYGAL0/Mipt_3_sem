#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

int main (int argc, char * argv[]) {
    // Объявление переменных
    pid_t chpid;
    int fdwr = 0;
    int fdrd = 0;
    char messageout[1025];
    char messagein[1025];
    ssize_t sizeout;
    ssize_t sizein;
    char key1[] = "key1";
    char key2[] = "key2";
    char nameper[21];
    // Маска для создания Фифо
    (void) umask(0);
    // Распараллеливание на процесс чтения и написания.
    // Предупреждение для входа.
    if (argc == 1) {
           printf("\e[;35mOpen second terminal and Enter two or more arguments for chatting.\e[m\n");
    }
    chpid = fork();



    if (chpid == -1) {
        // Ошибка
        printf("\e[;31mCan't fork a child process\e[m\n");
        exit(-1);
    } else if (chpid > 0) {
        // Родительский процесс (написание)
        // Время
        struct tm *u1;
        time_t timer1;
        // Запуск дескрипторов относительно порядка запуска программ
        if (argc >= 2) {
            if ((fdwr = open(key2, O_WRONLY | O_CREAT, 0666)) < 0) {
                printf("\e[;31mCan't open FIFO for writing...\e[m\n");
                exit(-1);
            }
        } else {
            if ((fdwr = open(key1, O_WRONLY | O_CREAT, 0666)) < 0) {
                printf("\e[;31mCan't open FIFO for writing...\e[m\n");
                exit(-1);
            }
        }
        // Ввод имени не более 20 символов
        printf ("\e[;35mEnter your Name for chatting\e[m\n");
        printf ("\e[;35mThe name must be less than 20 characters long: \e[m");
        fgets (messageout, 1025, stdin);
            messageout[strlen(messageout) - 1] = '\0';

        while (strlen(messageout) > 20) {
            printf("\e[5;31mWrong name\e[m\n");
            printf("\e[;35mTry other: \e[m");
            fgets (messageout, 1025, stdin);
                messageout[strlen(messageout) - 1] = '\0';
        }
        sizeout = write(fdwr, messageout, strlen(messageout) + 1);
        if (sizeout != strlen(messageout) + 1) {
            printf("\e[;31mCan't write into file\e[m\n");
            exit(-1);
        }
        // Написание сообщений
        while (1) {
            //Написание
            fgets (messageout, 1025, stdin);
            messageout[strlen(messageout) - 1] = '\0';
            // Проверка на выход из системы
            if (strcmp(messageout, "Exit") == 0) {
                sizeout = write(fdwr, messageout, strlen(messageout) + 1);
                if (sizeout != strlen(messageout) + 1) {
                    printf("\e[;31mCan't write into file\e[m\n");
                    exit(-1);
                }
                close(fdwr);
                exit(1);
            } else {
                // Вывод на экран и запись в Фифо + время
                timer1 = time(NULL);
                u1 = localtime(&timer1);
                printf("\e[;32m%d:%d:%d_____You: %s\e[m\n", u1->tm_hour, u1->tm_min, u1->tm_sec, messageout);
                sizeout = write(fdwr, messageout, strlen(messageout) + 1);
                if (sizeout != strlen(messageout) + 1) {
                    printf("\e[;31mCan't write into file\e[m\n");
                    exit(-1);
                }
            }
        }

    } else {
        // Порожденный процесс
        // Время
        struct tm *u2;
        time_t timer2;
        // Запуск дескрипторов относительно порядка запуска программ
            if (argc >= 2) {
                if ((fdrd = open(key1, O_RDONLY | O_CREAT, 0666)) < 0) {
                    printf("\e[;31mCan't open FIFO for reading...\e[m\n");
                    exit(-1);
                }
            } else {
                if ((fdrd = open(key2, O_RDONLY | O_CREAT, 0666)) < 0) {
                    printf("\e[;31mCan't open FIFO for reading...\e[m\n");
                    exit(-1);
                }
            }
        // Получение имени
        sizein = read(fdrd, messagein, 21);
        if (sizein <= 0) {
            printf("\e[;31mCan't read from file\e[m\n");
            exit(-1);
        }
        strcpy(nameper, messagein);
        // Получение сообщений
        while (1) {
            sizein = read(fdrd, messagein, 1025);
            if (sizein <= 0) {
                printf("\e[;31mCan't read from file\e[m\n");
                exit(-1);
            }
            // Проверка на выход из системы
            if (strcmp(messagein, "Exit") == 0) {
                printf("\e[;36m%s is disconnected\e[m\n", nameper);
                close(fdrd);
                kill(getppid(), SIGKILL);
                exit(1);
            } else {
                // Печать сообщения
                timer2 = time(NULL);
                u2 = localtime(&timer2);
                printf("\e[;34m%d:%d:%d_____%s : %s\e[m\n",u2->tm_hour, u2->tm_min, u2->tm_sec, nameper,  messagein);
            }
        }
    }
}
