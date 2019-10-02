#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>

int main() {
    size_t size = 0;
    FILE *file = fopen("lol", "r");
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
    int j = 0;
    while (fscanf(file,"%s", url) != EOF) {
        j = 0;
        while (url[j] != '\n') {
            j++;
        }
        url[j] = '\0';
        printf("Check: %s\n", url);
        size = write(fd[1], &url, strlen(url));
        if (size != strlen(url)) {
            printf("Can't write into pipe\n");
            exit(-1);
        }
        printf("Loh %d\n", getpid());
        int res = fork();
        if (res == -1) {
            printf("Can't fork child\n");
            exit(-1);
        } else if (res == 0) {
            printf("%d\n", getpid());
            close(fd[1]);
            char mes[100];
            size = read(fd[0], &mes, 100);
            if (size == 0) {
                printf("Can't read from pipe\n");
                exit(-1);
            }
//            execle ("/usr/bin/wget", "wget", mes, NULL, envp);
            printf("%s\n", mes);
            sleep(5);
            close(fd[0]);
            return 0;
        } else {
//            execle ("/usr/bin/wget", "wget", url, NULL, envp);
            waitpid(res, &child_stat, 0);
        }
        waitpid(res, &child_stat, 0);
    }
    close(fd[0]);
    close(fd[1]);
    fclose(file);
    return 0;
}