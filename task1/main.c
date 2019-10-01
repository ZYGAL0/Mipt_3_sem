#include <stdio.h>
#include <string.h>

//Задача 1: Выводит все переменные окружения, которые в значениях сожержат в значениях имя пользователя.
// Ввод: Имя программы `whoami`

int main(int argc, char *argv[], char *envp[]) {
   for (int i = 0; i < argc; i++) {
       printf("%s ", argv[i]);
   }
   printf("\n");
    for (int i = 0; envp[i] != NULL; i++) {
        if (strstr(envp[i], argv[1]) != NULL) {
            printf("envp[%d]: %s\n", i, envp[i]);
       }
    }

  return 0;
}

