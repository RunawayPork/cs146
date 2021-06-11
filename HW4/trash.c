#include <stdio.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>



int main(int argc, char* argv[]){
    FILE* ls;
    char path[PATH_MAX];
    char temp[PATH_MAX];
    char cwd[PATH_MAX];

    ls = popen("ls -A /tmp/Trash", "r");

    strcpy(cwd, getenv("TRASH"));
    

    while (fgets(path, PATH_MAX, ls) != NULL){
        strcpy(temp, cwd);
        strcat(temp, "/");
        strcat(temp, path);
        if (*(temp + strlen(temp)-1) == '\n'){
            *(temp + strlen(temp)-1) = 0;
        }
        unlink(temp);
    }
    pclose(ls);
    unlink("trashMap.txt");
    int logFd = open("/tmp/Trash/trashMap.txt", O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
    close(logFd);

    return 0;
}