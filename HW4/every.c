
#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int BUFFER_SIZE = 100;

void debugInput(int argc, char *argv[]){
    int i = 0;
    for (i; i<argc; i++){
        printf("%d: %s\n", i, argv[i]);
    }
}

char* findMatch(regex_t regex, int argc, char *argv[]){
    int i = 1;
    for (i; i<argc; i++){
        if (regexec(&regex, argv[i], 0, NULL, 0) == 0){
            return argv[i];
        }
    }
    return NULL;
}

void parseCommandLineIntervals(int argc, char *argv[], char** intervals){
    regex_t regex;
    int retVal;
    
    int m_specified = 0;
    char * match = NULL;
    retVal = regcomp(&regex, "-[0-9]+,[0-9]+", REG_EXTENDED);
    match = findMatch(regex, argc, argv);
    
    
    if (match == NULL){
        retVal = regcomp(&regex, "-[0-9]+", REG_EXTENDED);
        match = findMatch(regex, argc, argv);
        if (match == NULL){
            return;
        }
    }
    else{
        m_specified = 1;
    }

    char* begin;
    char* end;
    if (match != NULL){
        match++;
        begin = match;

        int i = 0;
        for (i; *(match+i) != 0; i++){
            if (*(match+i) == ','){
                if (m_specified){
                    *(match+i) = 0;
                    end = (match+i+1);
                }
                break;
            }
        }
    }

    intervals[0] = begin;
    if (m_specified){ intervals[1] = end; }
    
}

char ** getIntervalsFromEnv(char** intervals){
    char* temp[2]; //Index 2 to match the format of the commandLine argv
    temp[0] = ""; //Placeholder aka junk
    temp[1] = getenv("EVERY");
    if (temp[1] != NULL){
        parseCommandLineIntervals(2, temp, intervals);
    }

    
}

void printFile(int n, int m, FILE* file){
    int nCounter = 0;
    int mCounter = 0;
    int run = 1;
    char line[BUFFER_SIZE]; // Max Line Length = 100

    while (1){
        fgets(line, BUFFER_SIZE, file);
     
        if (feof(file)){ break; }
        
        if (nCounter % n == 0){ run = 1; }
        else{ run = 0; }

        if (run== 1 || mCounter != 0){
            printf("%s", line);
            mCounter++;
            if (mCounter == m){
                mCounter = 0;
            }
        }
        nCounter +=1;
    }
}

int main( int argc, char *argv[] )  {
    //debugInput(argc, argv);
    char* intervals[2];
    intervals[0] = NULL;
    intervals[1] = NULL;
    int NMspecified = 1;

    parseCommandLineIntervals(argc, argv, intervals);
    if (intervals[0] == NULL && intervals[1] == NULL) { 
        getIntervalsFromEnv(intervals); 
        NMspecified = 0;
    }
    if (intervals[0] == NULL) { intervals[0] = "-1"; }
    if (intervals[1] == NULL) { intervals[1] = "1";}
    
    int n = atoi(intervals[0]);
    int m = atoi(intervals[1]);


    FILE* file;
    if ((argc-NMspecified) == 1){ //stdin as input
        char buffer[BUFFER_SIZE];
        file = fopen("stdin.txt", "w+");
        int c=0;
        while ((c = getchar()) != EOF){ fputc(c, file); }
        fclose(file);
        
        file = fopen("stdin.txt", "r");
        printFile(n,m,file);
    }
    else{ //there are files provided
        int i=1;
        for (i; i<argc; i++){
            if (*(argv[i]) != '-'){
                file = fopen(argv[i], "r");
                if ( file == NULL){
                    perror(argv[i]);
                    continue;
                };
                printFile(n,m,file);
                fclose(file);
            }
        }
    }
    
    return 0;
}


