#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <time.h>
#include <sys/sem.h>


/* Предполагаем, что размер исходного файла < SIZE */
#define SIZE 512

int main(int argc, char *argv[]) {
    // struct time for message
    struct tm *u1;
    time_t timer1;
/* Указатель на разделяемую память */
    char *memoryw;
    char *memoryr;
    char *memory;
/* IPC дескриптор для области разделяемой памяти */
    int shmid;
/* Имя файла, использующееся для генерации ключа.
   Файл с таким именем должен существовать в текущей директории */
    char pathname1[] = "key";
/* IPC ключ */
    key_t key1;

/* Генерируем IPC ключ из имени файла  в текущей директории
   и номера экземпляра области разделяемой памяти 0 */
    if ((key1 = ftok(pathname1, 0)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

/* Пытаемся найти разделяемую память по сгенерированному ключу */
    if ((shmid = shmget(key1, SIZE, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create shared memory\n");
        exit(-1);
    }

/* Пытаемся отобразить разделяемую память в адресное пространство текущего процесса */
    if ((memory = (char *) shmat(shmid, NULL, 0)) == (char *) (-1)) {
        printf("Can't attach shared memory\n");
        exit(-1);
    }
    if (argc == 1) {
        memoryw = memory;
        memoryr = memoryw + 256;
    } else {
        memoryr = memory;
        memoryw = memoryr + 256;
    }

    // semaphores
    int semid;
    char pathname2[] = "main.c";
    key_t key2;
    key2 = ftok(pathname2, 0);
    if ((semid = semget(key2, 2, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create semaphore set\n");
        exit(-1);
    }
    struct sembuf mybuf;

    char messageout[256];

    pid_t chpid = fork();

    if (chpid == -1) {
        // Ошибка
        printf("\e[;31mCan't fork a child process\e[m\n");
        exit(-1);
    } else if (chpid > 0) {
        while (1) {
            //Написание
            fgets(messageout, 40, stdin);
            messageout[strlen(messageout) - 1] = '\0';
            // Проверка на выход из системы
            if (strcmp(messageout, "Exit") == 0) {
                strcpy(memoryw, messageout);
                /* Отсоединяем разделяемую память и завершаем работу */
                if (shmdt(memoryw) < 0) {
                    printf("Can't detach shared memory\n");
                    exit(-1);
                }
                exit(0);
            } else {
                // Вывод на экран и запись в Фифо + время
                timer1 = time(NULL);
                u1 = localtime(&timer1);
                printf("\e[;32m%d:%d:%d_____You: %s\e[m\n", u1->tm_hour, u1->tm_min, u1->tm_sec, messageout);
                strcpy(memoryw, messageout);

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
            }
            return 0;
        }
    } else {
        // Порожденный процесс
        // Время
        struct tm *u2;
        time_t timer2;
        // Запуск дескрипторов относительно порядка запуска программ
        while (1) {
            if (sig_status == 1) {
                // Проверка на выход из системы
                if (strcmp(memoryr, "Exit") == 0) {
                    printf("\e[;36m is disconnected\e[m\n");
                    if (shmdt(memoryw) < 0) {
                        printf("Can't detach shared memory\n");
                        exit(-1);
                    }
                    exit(0);
                } else {
                    timer2 = time(NULL);
                    u2 = localtime(&timer2);
                    printf("\e[;34m%d:%d:%d_____ : %s\e[m\n", u2->tm_hour, u2->tm_min, u2->tm_sec, memoryr);
                }
            }
        }
    }
}