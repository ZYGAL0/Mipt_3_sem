#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define DEFAULT 10

int fd[2];

int semid = 0;

void display_message();

int FillServAdress(char *const *argv, struct sockaddr_in *servaddr);

void *mythread(void *lol);

int main(int argc, char **argv) {

    if((semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT)) < 0){
        printf("Can\'t get semid\n");
        exit(-1);
    }

    if (argc != 2) {
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }

    if (pipe(fd) < 0) {
        printf("Can't create pipe\n");
        exit(1);
    }

    signal(SIGALRM, display_message);

    int sockfd = 0;
    char sendline[1000], recvline[1000];
    bzero(sendline, 1000);
    bzero(recvline, 1000);

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror(NULL);
        exit(1);
    }

    struct sockaddr_in servaddr;
    int res = FillServAdress(argv, &servaddr);
    if (res != 0) {
        close(sockfd);
        exit(res);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    if (read(sockfd, recvline, 999) < 0) {
        perror("Can\'t read\n");
        close(sockfd);
        exit(1);
    }
    if (strcmp(recvline, "OK") == 0) {
        printf("Connection established\n");
    }

    pid_t chpid = fork();

    if (chpid == -1) {
        printf("Can't fork a child process\n");
        exit(-1);
    } else if (chpid > 0) {
        int size = 0;

        pthread_t thid;
        int result = pthread_create(&thid, (pthread_attr_t *) NULL, mythread, &sockfd);
        if (result != 0) {
            printf("Error on thread create, return value = %d\n", result);
            exit(-1);
        }

        while (1) {
            fgets(sendline, 1000, stdin);
            sendline[strlen(sendline) - 1] = '\0';
            if (sendline[0] == '\0') {
                continue;
            } else if (strncmp(sendline, "'LATE' ", 7) == 0) {
                int cur = 7;
                int time = 0;
                while (sendline[cur] != ' ' && sendline[cur] != '\n') {
                    if (isdigit(sendline[cur])) {
                        time = time * 10 + (sendline[cur] - '0');
                    } else {
                        break;
                    }
                    cur++;
                }
                char *p = sendline + cur + 1;
                size = write(fd[1], p, strlen(p) + 1);
                if (size != strlen(p) + 1) {
                    printf("Can't write into pipe\n");
                    exit(-1);
                }
                if (time == 0) {
                    alarm(DEFAULT);
                } else {
                    alarm(time);
                }

            } else {
                if (write(sockfd, sendline, strlen(sendline) + 1) < 0) {
                    perror("Can\'t write\n");
                    close(sockfd);
                    exit(1);
                }
                if (strcmp(sendline, "Exit") == 0) {
                    break;
                }
            }
        }

        if (semctl(semid, 0, IPC_RMID, 0) < 0) {
            printf("Can't delete sem\n");
            exit(1);
        }
        kill(chpid, SIGKILL);
        close(sockfd);
        close(fd[0]);
        close(fd[1]);
    } else {
        close(fd[0]);
        close(fd[1]);
        while (1) {
            if (read(sockfd, recvline, 999) < 0) {
                perror("Can\'t read\n");
                close(sockfd);
                exit(1);
            }
            if (strcmp(recvline, "Exit") == 0) {
                printf("Server is down\n");
                kill(getppid(), SIGKILL);
                close(sockfd);
                exit(0);
            }
            printf("%s\n", recvline);
        }
    }
}

int FillServAdress(char *const *argv, struct sockaddr_in *servaddr) {
    bzero(servaddr, sizeof((*servaddr)));
    (*servaddr).sin_family = AF_INET;
    (*servaddr).sin_port = htons(51000);
    if (inet_aton(argv[1], &(*servaddr).sin_addr) == 0) {
        printf("Invalid IP address\n");
        return 1;
//        exit(1);
    }
    return 0;
}

void display_message() {
    struct sembuf mybuf;
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if(semop(semid, &mybuf, 1) < 0){
        printf("Can\'t wait for condition\n");
        exit(-1);
    }
}

void *mythread(void *lol) {

    int sockfd = *(int *) lol;

    struct sembuf mybuf;
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    while (1) {

        size_t size = 0;

        char recvline[1000];
        bzero(recvline, 1000);

        while (1) {

            size = read(fd[0], recvline, 1000);
            if (size <= 0) {
                printf("Can't read from pipe\n");
                close(fd[0]);
                close(fd[1]);
                close(sockfd);
                kill(getppid(), SIGKILL);
                exit(-1);
            }

            if(semop(semid, &mybuf, 1) < 0){
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            if (write(sockfd, recvline, strlen(recvline) + 1) < 0) {
                perror("Can\'t write\n");
                close(fd[0]);
                close(fd[1]);
                close(sockfd);
                kill(getppid(), SIGKILL);
                exit(1);
            }
        }
    }
}
