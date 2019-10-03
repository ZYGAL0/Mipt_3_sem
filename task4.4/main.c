#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>

// Основной процесс считывает из input url-ссылки, а дочерние процессы скачивают  html с помощью exec

int main(int argc, char *argv[], char *envp[]) {
    size_t size = 0;
    FILE *file = fopen("input", "r");
    if (file == NULL) {
        printf("Can't open file\n");
        exit(-1);
    }
    int fd[2] = {0};
    if (pipe(fd) < 0) {
        printf("Can't create pipe\n");
        exit(1);
    }
    int child_stat = 0;
    char url[100];
    while (fgets(url, 100, file) != NULL) {
        url[strlen(url) - 1] = '\0';
        size = write(fd[1], &url, strlen(url) + 1);
        if (size != (strlen(url) + 1)) {
            printf("Can't write into pipe\n");
            exit(-1);
        }
        int res = fork();
        if (res == -1) {
            printf("Can't fork child\n");
            exit(-1);
        } else if (res == 0) {
            fclose(file);
            close(fd[1]);
            printf("Child process%d\n", getpid());
            char mes[101];
            size = read(fd[0], &mes, 101);
            if (size <= 0) {
                printf("Can't read from pipe\n");
                exit(-1);
            }
            close(fd[0]);
            (void) execle ("/usr/bin/wget", "wget", mes, NULL, envp);
            printf ("Error in execle\n");
//            printf("%s\n", mes);
            exit(0);
        }
        waitpid(res, &child_stat, 0);
    }
    close(fd[0]);
    close(fd[1]);
    fclose(file);
    return 0;
}