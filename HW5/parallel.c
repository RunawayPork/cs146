#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <linux/limits.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

int ARRAYSIZE = 50;
int n;
pthread_mutex_t lock;
char shell[PATH_MAX];
int ret=0;
char*** threadArgs;
int commandCount=0;


void stripWhiteSpace(char* line, char delim){
    int counter = 0;
    while(1){
        if (*(line+counter) == '\0'){
            return;
        }
        if  (*(line+counter) == delim){
            *(line+counter) = '\0';
            return;
        }
        counter++;
    }
}

void printArray(char** array){
    int x = 0;
    printf("[");
    for (x;  array[x]!= NULL; x++){
        printf("%s\n", array[x]);
    }
    printf("NULL]\n"); 
}

int countArguments(char* line){
    int whiteSpace = 0;
    int counter = 0;
    while (1){
        if (*(line+counter) == '\0'){
            return whiteSpace;
        }
        if (*(line+counter) == ' '){
            whiteSpace++;
        }
        counter++;
    }
}


char** acquireFromThreadArgs(int i, char*** threadArgs,  pthread_mutex_t lock){
    //printf("Locked\n");
    pthread_mutex_lock(&lock);
    char** args = threadArgs[i];
    threadArgs[i] = NULL;
    pthread_mutex_unlock(&lock);
    //printf("Unlocked\n");
    return args;
}

int checkIfFinished(){
    //printf("Locked\n");
    pthread_mutex_lock(&lock);
    int i = 0;
    for(i; i< commandCount; i++){
        if (threadArgs[i] != NULL){
            pthread_mutex_unlock(&lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&lock);
    return 1; 
}

void reapChildren(){
    int status;
    wait(&status);
    if (status != 0){
        ret++;
    }
}


int main (int argc, char* argv[]){

    if (argc > 4){ printf("Wrong number of arguments\n"); exit(1);}

    strcpy(shell, "/bin/bash");
    int i = 1;
    n = get_nprocs_conf();
    int optionalShell = 0;
    for (i; i < argc; i++){

        //Checking for shell argument
        if (strcmp(argv[i], "-s") == 0){
            if (i != 1){ 
                printf("Wrong placement of shell argument\n");
                exit(1);
            }
            optionalShell = 1;
        }
        else if (optionalShell){
            strcpy(shell, argv[i]);
            optionalShell = 0;
        }
        else if (isdigit(*(argv[i]))){
            n = atoi(argv[i]);
        }
        else{
            printf("Invalid input\n");
            exit(1);
        }
    }

    if (optionalShell){
        printf("Didn't provide a shell\n");
        exit(1);
    }

    //Initializing the array
    char* commandList[ARRAYSIZE];
    i = 0;
    for (i; i < ARRAYSIZE; i++){
        char* command = malloc(sizeof(char) * PATH_MAX);
        strcpy(command, "");
        commandList[i] = command;
    }

    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1){   
        if (strlen(line) == 1){ continue; }
        strcpy(commandList[commandCount], "");
        strcat(commandList[commandCount], line);
        stripWhiteSpace(commandList[commandCount], '\n');
        commandCount++;
    }

    
    //printf("Shell = %s\n", shell);
    //printf("Cores = %d\n", n);

    threadArgs = malloc(sizeof(char**) * commandCount);
    i = 0;
    for (i; i < commandCount; i++){
        char* s = commandList[i];
        char** array = malloc(sizeof(char*) * 4);
        array[0] = shell;
        array[1] = "-c";
        array[2] = commandList[i];
        array[3] = NULL;
        threadArgs[i]= array;
    }
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    int nThreads = 0;
    i = 0;
    for (i; i < commandCount; i++){
        while (nThreads >= n) { reapChildren(); nThreads--; };
        char ** args = acquireFromThreadArgs(i,threadArgs,lock);
        nThreads++;
        int pid = fork();
        if (pid == 0){
            execvp(shell, args);
        }
    }

    while(nThreads!=0){
        reapChildren();
        nThreads--;
    }
    
    while(1){
        if (checkIfFinished() == 1){
            printf("%d\n", ret);
            return ret;
        }
        sleep(0.5);
    }
}