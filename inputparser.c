#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#include <unistd.h>
const char* OPERATION_CHARACTER[] = { "|", "&&", ">", "exit", "setenv", "printenv", NULL };

int isOP(char* aString) {
    for(int i = 0; OPERATION_CHARACTER[i] != NULL; i++) {
        if(strcmp(aString, OPERATION_CHARACTER[i]) == 0 || aString[0] == '|')
            return i;
    }
    return -1;
}

void appendStringToArray (char*** array, char* source ){
    char*** tmp = array;
    int arraylen = 0;
    int stringlength = strlen(source);
    if(*array == NULL){
        *array = (char**)malloc(sizeof(char*) * (arraylen + 1));
    }else {
        while((*array)[arraylen] != NULL) arraylen++;
    }
    (*array)[arraylen] = (char*) malloc(sizeof(char) * (stringlength + 1));
    strcpy((*array)[arraylen], source);
    arraylen++;
    *array = (char**)realloc(*array, sizeof(char*) * (arraylen + 1) );
    (*array)[arraylen] = NULL;
}
char*** parseString(char *data) {
    char *** commandPool = (char***)malloc(sizeof(char**) * 2);
    int currentCommand = 0;
    const char* const delim = " ";
    char* substr = NULL;
    char* savePtr = NULL;
    commandPool[currentCommand] = NULL;
    substr = strtok_r(data, delim, &savePtr);

    do{
        switch (isOP(substr)) {
            case 0://| and |n
            case 1://&&
            case 2://>
                currentCommand++;
                commandPool = (char***)realloc(commandPool, sizeof(char**) * ( 2 + currentCommand));
                commandPool[currentCommand] = NULL;
                appendStringToArray(&commandPool[currentCommand], substr);
                currentCommand++;
                commandPool = (char***)realloc(commandPool, sizeof(char**) * ( 2 + currentCommand));
                commandPool[currentCommand] = NULL;
                break;
            case 3://setenv
            case 4://printenv
            default:// normal
                appendStringToArray(&commandPool[currentCommand], substr);
        }
        substr = strtok_r(NULL, delim, &savePtr);
    }while(substr);

    currentCommand++;
    commandPool = (char***)realloc(commandPool, sizeof(char**) * ( 2 + currentCommand));
    commandPool[currentCommand] = NULL;
    return commandPool;
}

