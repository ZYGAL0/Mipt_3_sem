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

int sockfd = 0;

struct Info {
    char mes[1000];
    int time;
};

int FillServAdress(char *const *argv, struct sockaddr_in *servaddr);

void *mythread(void *late);

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: client.out <IP address>\n");
        exit(1);
    }

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
        close(sockfd);
        exit(-1);
    } else if (chpid > 0) {

        while (1) {
            fgets(sendline, 1000, stdin);
            sendline[strlen(sendline) - 1] = '\0';
            if (sendline[0] == '\0') {
                continue;
            } else if (strncmp(sendline, "-l ", 3) == 0) {
                int cur = 3;
                int time = 0;
                while (sendline[cur] != ' ' && sendline[cur] != '\n') {
                    if (isdigit(sendline[cur])) {
                        time = time * 10 + (sendline[cur] - '0');
                    } else {
                        cur--;
                        break;
                    }
                    cur++;
                }
                struct Info late;
                strcpy(late.mes, sendline + cur + 1);

                if (time == 0) {
                    late.time = DEFAULT;
                } else {
                    late.time = time;
                }

                pthread_t thid;
                int result = pthread_create(&thid, (pthread_attr_t *) NULL, mythread, &late);
                if (result != 0) {
                    printf("Error on thread create, return value = %d\n", result);
                    exit(-1);
                }

            } else {
                if (write(sockfd, sendline, strlen(sendline) + 1) < 0) {
                    perror("Can\'t write\n");
                    close(sockfd);
                    exit(1);
                }
                if (strcmp(sendline, "EXIT") == 0) {
                    break;
                }
            }
        }
        close(sockfd);
        kill(chpid, SIGKILL);
    } else {
        while (1) {
            if (read(sockfd, recvline, 999) < 0) {
                perror("Can\'t read\n");
                close(sockfd);
                exit(1);
            }
            if (strcmp(recvline, "EXIT") == 0) {
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
    servaddr->sin_family = AF_INET;
    servaddr->sin_port = htons(51000);
    if (inet_aton(argv[1], &servaddr->sin_addr) == 0) {
        printf("Invalid IP address\n");
        return 1;
//        exit(1);
    }
    return 0;
}

void *mythread(void *late) {

    struct Info info = *(struct Info *) late;

    sleep(info.time);

    if (write(sockfd, info.mes, strlen(info.mes) + 1) < 0) {
        perror("Can\'t write\n");
        close(sockfd);
        kill(getppid(), SIGKILL);
        exit(1);
    }
    return NULL;
}