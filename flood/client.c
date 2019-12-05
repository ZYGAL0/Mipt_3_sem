#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int FillServAdress(char *const *argv, struct sockaddr_in *servaddr);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }

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

        while (1) {
            fgets(sendline, 1000, stdin);
            sendline[strlen(sendline) - 1] = '\0';

            if (write(sockfd, sendline, strlen(sendline) + 1) < 0) {
                perror("Can\'t write\n");
                close(sockfd);
                exit(1);
            }
            if (strcmp(sendline, "Exit") == 0) {
                break;
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
                printf ("Server is down\n");
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
