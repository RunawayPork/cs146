#include <stdio.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>

char* getFileName(char* path){
    int i = strlen(path);
    
    for (i; path+i != path; i--){
        if (*(path+i) == '/'){
            return path+i+1;
        }
    }
    return path;
}


int isDir(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

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


void moveFile(char* src, char* dest){
    char log[PATH_MAX];
    char buffer[1024];

    int srcFd = open(src, O_RDONLY, S_IRWXU);
    int dstFd = open(dest, O_CREAT | O_WRONLY, S_IRWXU);



    if (srcFd == -1 ){
        printf("File: %s is not in trash.\n", src);
        return;
    }
    if (dstFd == -1){
        printf("Error restoring file %s\n", src);
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
}


int main(int argc, char* argv[]){
    
    FILE* mapping = fopen("/tmp/Trash/trashMap.txt", "r+");
    char line[PATH_MAX];
    char src[PATH_MAX];
    char dest[PATH_MAX];
    char temp1[PATH_MAX];
    char temp2[PATH_MAX];
    
    int i = 1;
    for (i; i < argc; i++){
        
        strcpy(line, "");
        strcpy(src, argv[i]);
        strcpy(dest, "");
        strcpy(temp1, "");
        strcpy(temp2, "");

        if (isDir(src)){
                getSubstring(src, '/', 'L', temp1);
                int fileNameLength = strlen(temp1);
                strcpy(temp1, src);
                *(temp1 + strlen(temp1) - fileNameLength) = 0;
                int dir = chdir(temp1);

                strcpy(dest, temp1);
                strcat(dest, getFileName(src)+2); //Ignore the ""._"
                int retVal = rename(src, dest);
                if ( retVal == -1){
                   printf("Error safe removing directory %s\n", src);
                }
        }

        else{
            int found = 0;
            while (fgets(line, PATH_MAX, mapping) != NULL){
                getSubstring(line, ' ', 'L', src);
                getSubstring(line, ' ', 'R', dest);
                getSubstring(argv[i], '/', 'L', temp1);
                getSubstring(src, '/', 'L', temp2);
                if (strcmp(temp1, temp2) == 0){
                    found=1;
                    break;
                }
            }

            if (!found){
                printf("The file does not exist\n");
            }
            else{
                moveFile(src, dest);
            }
        }
        
    }
    return 0;
}

    




