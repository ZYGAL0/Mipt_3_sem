#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>

// Дочерний процесс и супрвайзер (Pipe)
// Ввод: имя программы до какого числа следует считать

int main(int argc, char *argv[], char **envp) {
    size_t size = 0;
    int fd[2];
    if (pipe(fd) < 0) {
        printf("Can't create pipe\n");
        exit(1);
    }
    int child_stat = 0;
    int kol =  atoi(argv[1]);
    int i = 0;
    while (i < kol) {
        int res = fork();
        if (res == -1) {
            printf ("Can't fork child\n");
            exit(-1);
        } else if (res == 0) {
            close(fd[1]);
            int mes = 0;
            size = read(fd[0], &mes, sizeof(int));
            if (size != sizeof(int)) {
                printf ("Can't read from pipe\n");
                exit(-1);
            }
            printf ("Message from supervisor: %d\n", mes);
            close(fd[0]);
            srand((unsigned int) time(NULL) + getpid());
            int chto = rand() % 2;
            exit (chto);
        } else {
            size = write(fd[1], &i, sizeof(int));
            if (size != sizeof(int)) {
                printf ("Can't write into pipe\n");
                exit(-1);
            }
            waitpid(res, &child_stat, 0);
            if (WEXITSTATUS(child_stat) == 0) {
                i++;
            }
            printf ("WEXITSTATUS: %d\n", WEXITSTATUS(child_stat));
        }
    }
    close(fd[0]);
    close(fd[1]);
    printf("Result: %d\n", i);
    return 0;
}