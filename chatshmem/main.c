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


    // Предполагаем, что размер исходного файла < SIZE
#define SIZE 512

int main(int argc, char *argv[]) {

    //  Объявление того, чтобы требуется открыть в дургом терминале ту же программу, но с 2-мя входными параметрами
    if (argc == 1) {
        printf("\e[;35mOpen second terminal and Enter two or more arguments for chatting.\e[m\n");
    }

    //  Имя файла, использующееся для генерации ключа.
    //  Файл с таким именем должен существовать в текущей директории
    char pathname1[] = "key";

    //  IPC ключ
    key_t key1;

    //  Генерируем IPC ключ из имени файла  в текущей директории
    //  и номера экземпляра области разделяемой памяти 0
    if ((key1 = ftok(pathname1, 0)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

    //  IPC дескриптор для области разделяемой памяти
    int shmid;

    //  Пытаемся найти разделяемую память по сгенерированному ключу
    if ((shmid = shmget(key1, SIZE, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create shared memory\n");
        exit(-1);
    }

    //  Указатель на разделяемую память
    char *memoryw;
    char *memoryr;
    char *memory;

    //  Пытаемся отобразить разделяемую память в адресное пространство текущего процесса
    if ((memory = (char *) shmat(shmid, NULL, 0)) == (char *) (-1)) {
        printf("Can't attach shared memory\n");
        exit(-1);
    }

    int arr[4] = {0, 1, 2, 3};

    if (argc == 1) {
        memoryw = memory;
        memoryr = memoryw + 256;
    } else {
        memoryr = memory;
        memoryw = memoryr + 256;
        arr[0] = 2, arr[1] = 3, arr[2] = 0, arr[3] = 1;
    }

    //  Объявление семофоров
    int semid;
    char pathname2[] = "main.c";
    key_t key2;
    key2 = ftok(pathname2, 0);
    if ((semid = semget(key2, 4, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create semaphore set\n");
        exit(-1);
    }
    struct sembuf mybuf;



    // Ввод имени не более 20 символов
    char name[21];
    printf("\e[;35mEnter your Name for chatting\e[m\n");
    printf("\e[;35mThe name must be less than 20 characters long: \e[m");

    fgets(name, 20, stdin);
    name[strlen(name) - 1] = '\0';

    while (strlen(name) > 20) {
        printf("\e[5;31mWrong name\e[m\n");
        printf("\e[;35mTry other: \e[m");
        fgets(name, 21, stdin);
        name[strlen(name) - 1] = '\0';
    }

    int i = 0;
    char messageout[256];

    pid_t chpid = fork();

    if (chpid == -1) {
        // Ошибка
        printf("\e[;31mCan't fork a child process\e[m\n");
        exit(-1);
    } else if (chpid > 0) {
        // Время
        struct tm *u1;
        time_t timer1;
        while (1) {
            //Написание
            if (i == 0) {
                strcpy(messageout, name);
            } else {
                fgets(messageout, 40, stdin);
                messageout[strlen(messageout) - 1] = '\0';
            }

            strcpy(memoryw, messageout);

            mybuf.sem_num = arr[0];
            mybuf.sem_op = 1;

            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            mybuf.sem_num = arr[1];
            mybuf.sem_op = -1;

            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
            if (i == 0) {
                i++;
            } else {
                // Проверка на выход из системы
                if (strcmp(messageout, "Exit") == 0) {
                    /* Отсоединяем разделяемую память и завершаем работу */
                    if (shmdt(memory) < 0) {
                        printf("Can't detach shared memory\n");
                        exit(-1);
                    }
                    kill(chpid, SIGKILL);
                    exit(0);
                } else {
                    // Вывод на экран и запись в Фифо + время
                    timer1 = time(NULL);
                    u1 = localtime(&timer1);
                    printf("\e[;32m%d:%d:%d_____You: %s\e[m\n", u1->tm_hour, u1->tm_min, u1->tm_sec, messageout);
                }
            }
        }
    } else {
        // Порожденный процесс
        // Время
        struct tm *u2;
        time_t timer2;
        // Запуск дескрипторов относительно порядка запуска программ
        while (1) {

            mybuf.sem_num = arr[2];
            mybuf.sem_op = -1;
            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            if (i == 0) {
                strcpy(name, memoryr);
            } else {
                strcpy(messageout, memoryr);
            }
            mybuf.sem_num = arr[3];
            mybuf.sem_op = 1;
            if (semop(semid, &mybuf, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
            if (i == 0) {
                i++;
            } else {
                // Проверка на выход из системы
                if (strcmp(messageout, "Exit") == 0) {
                    printf("\e[;36m%s is disconnected\e[m\n", name);
                    if (shmdt(memory) < 0) {
                        printf("Can't detach shared memory\n");
                        exit(-1);
                    }
                    kill(getppid(), SIGKILL);
                    exit(0);
                } else {
                    timer2 = time(NULL);
                    u2 = localtime(&timer2);
                    printf("\e[;34m%d:%d:%d_____%s : %s\e[m\n", u2->tm_hour, u2->tm_min, u2->tm_sec, name, messageout);
                }
            }
        }
    }
}