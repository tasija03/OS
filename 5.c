#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <math.h>

#define check_error(cond, msg)\
do{\
    if(!(cond)){\
        perror(msg);\
        fprintf(stderr, "File: %s\nFunction: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
        exit(EXIT_FAILURE);\
    }\
}while(0)

#define SEC_IN_DAY (86400)

void printDir(char* filepath){

    struct stat file_info = {0};
    check_error(-1 != stat(filepath, &file_info), "stat");

    if(!S_ISDIR(file_info.st_mode)) return;

    DIR* dir = opendir(filepath);
    check_error(NULL != dir, "opendir");

    struct dirent* dirEntry = NULL;
    while ((dirEntry = readdir(dir)) != NULL)
    {
        char* file_path = malloc(strlen(filepath) + 1 + strlen(dirEntry->d_name) + 1);
        strcpy(file_path, filepath);
        strcat(file_path, "/");
        strcat(file_path, dirEntry->d_name);

        if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0){
            free(file_path);
            continue;
        }

        check_error(-1 != stat(file_path, &file_info), "stat");
        
        if(S_ISREG(file_info.st_mode)){
            time_t now = time(NULL);
            time_t vreme_modifikacije = now - file_info.st_mtime;

            if((int)ceil(vreme_modifikacije/SEC_IN_DAY) <= 5) printf("%s\n", dirEntry->d_name);
        }

        printDir(file_path);
        free(file_path);

    }

closedir(dir);
return;

}

int main(int argc, char** argv){

    check_error(2 == argc, "usage");

    struct stat dir_info = {0};

    check_error(lstat(argv[1], &dir_info) != -1 && S_ISDIR(dir_info.st_mode), "Provided path is not a directory");

    printDir(argv[1]);

    return 0;

}