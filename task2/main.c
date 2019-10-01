#include <stdio.h>
#include <string.h>

// Задача 2: Программа читатает со стандартного входа файл и возвращает количество слов и строк в этом файле.
// Ввод: имя программы < Исходный файл

int main(int argc, char *argv[], char **envp) {
    char cur[1000];
    int words = 0, lines = 0;
    while (fgets(cur, 1000, stdin) != NULL) {
        lines++;
        cur[strlen(cur)] = '\0';
        for (int i = 0; i < strlen(cur); i++) {
            if (cur[i] != ' ' && (cur[i+1] == ' ' || cur[i+1] == '\0'))
            words++;
        }
    }
    printf("Words: %d Lines: %d\n", words, lines);
    return 0;
}