#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>

int main() {
    int child_stat = 0;
   int res = fork();
   if (res == -1) {
       printf ("Can't fork child\n");
       exit(-1);
   } else if (res == 0) {
       srand((unsigned int) time(NULL) + getpid());
       int chto = rand() % 2;
       exit (chto);
   } else {
       waitpid(res, &child_stat, 0);
       printf ("WEXITSTATUS: %d\n", WEXITSTATUS(child_stat));
       return 0;
   }
}