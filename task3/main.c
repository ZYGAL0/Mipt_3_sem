#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Задача 3: Программа, которая из строки вырезает ссылку (url) и печатать в файл и скачивает страницы.
// Ввод: имя программы < лог файл > результирующий файл

int main(int argc, char *argv[], char **envp) {
    char cur[10000];
    char *ref;
    char *end;
    int j = 0;
    char url[105];
    while (fgets(cur, 10000, stdin) != NULL) {
        char res[100] = "wget ";
        j = 0;
        while (cur[j] != '\n') {
            j++;
        }
        cur[j] = '\0';
        if (strstr(cur, "ERROR") != NULL) {
            ref = strstr(cur, "http://");
            if (ref != NULL) {
                end = strstr(ref + 7, "/");
                memcpy(url, ref, end - ref);
                url[end - ref] = '\0';
                printf("%s\n", url);
                strcat(res, url);
                system(res);
            }
        }
    }
    return 0;
}