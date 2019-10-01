#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char **envp) {
    int child_status = 0;
    int j = 0;
    while (1) {
            int kol =  atoi(argv[1]);
        while (j < kol) {
            j++;
            int result = fork();
            if (result == -1) {
                printf ("Can't fork child\n");
                exit (-1);
            } else if (result == 0) {
                srand((unsigned int) time(NULL) + getpid());
                int ti = (unsigned int) rand() % 5 + 1;
                printf ("Start PID: %d. Sleep : %d\n",getpid(), ti);
                sleep(ti);
                printf ("Finish PID: %d\n", getpid());
                return 0;
            }
        }
        wait(&child_status);
        j--;
    }
}
