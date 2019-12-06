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

#define DEFAULT 10

int flag = 0;

char late[10][1000];

void display_message(int s);

int FillServAdress(char *const *argv, struct sockaddr_in *servaddr);

void *mythread(void *sockfd);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }

    for (int j = 0; j < 10; ++j) {
        late[j][0] = '\0';
    }

    signal(SIGALRM, display_message);

    int latemes = 0;

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

    pthread_t thid;
    int result = pthread_create(&thid, (pthread_attr_t *) NULL, mythread, &sockfd);
    if (result != 0) {
        printf("Error on thread create, return value = %d\n", result);
        exit(-1);
    }

    pid_t chpid = fork();

    if (chpid == -1) {//    printf("copyit: Still working...\n");
        if (write(sockfd, sendline, strlen(sendline) + 1) < 0) {
            perror("Can\'t write\n");
            close(sockfd);
            exit(1);
        }
        printf("Can't fork a child process\n");
        exit(-1);
    } else if (chpid > 0) {

        while (1) {
            fgets(sendline, 1000, stdin);
            sendline[strlen(sendline) - 1] = '\0';
            if (sendline[0] == '\0') {
                continue;
            } else if (strncmp(sendline, "'ALARM' ", 8) == 0) {
                int cur = 8;
                int time = 0;
                while (sendline[cur] != ' ' && sendline[cur] != '\n') {
                    if (isdigit(sendline[cur])) {
                        time = time * 10 + (sendline[cur] - '0');
                    } else {
                        break;
                    }
                    cur++;
                }
                strcpy(late[latemes], sendline + cur + 1);
                latemes++;
                if (time == 0) {
                    alarm(DEFAULT);
                } else {
                    alarm(time);
                }
                if (latemes == 10) {
                    latemes = 0;
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
        kill(chpid, SIGKILL);
        close(sockfd);
    } else {
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

void display_message(int s) {
    flag = s;
}

void *mythread(void *sockfd) {

    int fd = *(int *) sockfd;
    int num = 0;
    while (1) {
        if (flag == 0) {
            sleep(1);
        } else {
            if (late[num][0] != '\0') {
                if (write(fd, late[num], strlen(late[num]) + 1) < 0) {
                    perror("Can\'t write\n");
                    close(fd);
                    exit(1);
                }
            }
            num++;
            flag = 0;
            if (num == 10) {
                num = 0;
            }
        }
    }
}
