#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>

pthread_mutex_t lock;

#define CLIENTS 3
#define LEN 10000

int clients[CLIENTS];

void FillServAdress(struct sockaddr_in *servaddr);

void *mythread(void *newsockfd);

int CreateHistory ();

int main() {

    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "\n mutex init failed\n");
        perror(NULL);
        exit(1);
    }

    for (int j = 0; j < CLIENTS; ++j) {
        clients[j] = -1;
    }

    printf("SID: %d\n", getpid());

    int sockfd;
    struct sockaddr_in cliaddr, servaddr;
    socklen_t clilen;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(NULL);
        exit(1);
    }

    FillServAdress(&servaddr);

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    int res = CreateHistory();
    if (res != 0) {
        close(sockfd);
        exit(res);
    }

    if (listen(sockfd, 10) < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    while (1) {
        for (int i = 0; i < CLIENTS; ++i) {
            if (clients[i] == -1) {
                clilen = sizeof(cliaddr);
                clients[i] = accept(sockfd, (struct sockaddr *) &cliaddr, &clilen);
                if (clients[i] < 0) {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
                pthread_t thid;
                int result = pthread_create(&thid, (pthread_attr_t *) NULL, mythread, &clients[i]);
                if (result != 0) {
                    printf("Error on thread create, return value = %d\n", result);
                    exit(-1);
                }
                break;
            }
        }
    }
}

void FillServAdress(struct sockaddr_in *servaddr) {
    bzero(servaddr, sizeof((*servaddr)));
    (*servaddr).sin_family = AF_INET;
    (*servaddr).sin_port = htons(51000);
    (*servaddr).sin_addr.s_addr = htonl(INADDR_ANY);
}

int CreateHistory () {
    int fd = open("history.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        printf("File open failed!\n");
        return 1;
//        exit(1);
    }
    size_t length = LEN * sizeof(char);
    ftruncate(fd, length);
    close(fd);
    return 0;
}

void *mythread(void *newsockfd) {

    int sockfd = *(int *) newsockfd;

    int fd = open("history.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        printf("File open failed!\n");
        exit(1);
    }

    char *ptr = (char *) mmap(NULL, LEN * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (ptr == MAP_FAILED) {
        printf("Mapping failed!\n");
        exit(2);
    }

    char *tmpptr = ptr;
    int n = 0;
    char line[1000];
    bzero(&line, 1000);
    while ((n = read(sockfd, line, 999)) > 0) {
        if (strcmp(line, "Exit") == 0) {
            break;
        }

        for (int lol = 0; lol < CLIENTS; ++lol) {
            if (clients[lol] != sockfd && clients[lol] != -1) {
                if (write(clients[lol], line, n) < 0) {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
            }
        }

        pthread_mutex_lock(&lock);
        strcat(tmpptr, line);
        strcat(tmpptr, "\n");
        pthread_mutex_unlock(&lock);
    }

    if (n < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    int end = 0;
    while (clients[end] != sockfd && end < CLIENTS) {
        end++;
    }
    clients[end] = -1;
    close(sockfd);
    printf("Someone left chat\n");
    munmap((void *) ptr, LEN * sizeof(char));
    return NULL;
}