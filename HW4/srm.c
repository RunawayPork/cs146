#include <stdio.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

void getSubstring(char* path, int target, char direction, char* result){
    if (direction == 'L'){
        int i = strlen(path);
        for (i; path-i != path; i--){
            if (*(path+i) == target){
                strcpy(result, path+i+1);
                if ( *(result+strlen(result)-1) == '\n'){
                    *(result+strlen(result)-1) = 0;
                }
                return;
            }
        }
        strcpy(result, path);
        return;
    }
    else{
        int i = 0;
        for (i; *(path+i) != 0 ; i++){
            if (*(path+i) == target){
                strcpy(result, path);
                result[i] = 0;
                return;
            }
        }
        strcpy(result, path);
        return;
    }
}


int isDir(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}


char* getFileName(char* path){
    int i = strlen(path);
    
    for (i; path+i != path; i--){
        if (*(path+i) == '/'){
            return path+i;
        }
    }
    return path;
}


void moveFile(char* src, char* dest){
    
    char log[PATH_MAX];
    char buffer[1024];

    int srcFd = open(src, O_RDONLY, S_IRWXU);
    int dstFd = open(dest, O_CREAT | O_WRONLY, S_IRWXU);

    if (srcFd == -1 || dstFd == -1){
        printf("File already deleted\n");
        return;
    }

    ssize_t bytesRead;
    while (1) {
        bytesRead = read(srcFd, buffer, 1024);
        if (bytesRead == 0) break;
        int x = write(dstFd, buffer, bytesRead);
    }

    close(srcFd);
    close(dstFd);

    unlink(src);

    int logFd = open("/tmp/Trash/trashMap.txt", O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
    strcat(log, src);
    strcat(log, " ");
    strcat(log, dest);
    strcat(log, "\n");
    write(logFd, log, strlen(log));
    close(logFd);
}


int main(int argc, char* argv[]){

    char src[PATH_MAX];
    char dest[PATH_MAX];
    char parentDir[PATH_MAX];
    
    

    int i = 1;
    for (i; i<argc; i++){
        strcpy(src, argv[i]);
        strcpy(dest, getenv("TRASH"));
        char* fileName = getFileName(src);
        if (*fileName != '/'){
            strcat(dest, "/");
        }
        strcat(dest, fileName);

        int file = open(src, O_RDONLY, S_IRWXU);

        if (file == -1){
            perror(src);
        }
        else{
            if (isDir(src)){
                //Getting the parent and changing dirs
                getSubstring(src, '/', 'L', parentDir);
                int fileNameLength = strlen(parentDir);
                strcpy(parentDir, src);
                *(parentDir + strlen(parentDir) - fileNameLength) = 0;
                int dir = chdir(parentDir);

                strcpy(dest, parentDir);
                strcat(dest, "._");
                strcat(dest, getFileName(src)+1);
                int retVal = rename(src, dest);

                if (retVal == -1){
                    printf("Error safe removing directory %s\n", src);
                }
            }
            else{
                moveFile(src, dest);
            }
        }
        close(file);
    }
    return 0;
}




