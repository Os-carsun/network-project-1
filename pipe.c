#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct CmdPipeList {
    char** cmds;
    int wait;
    struct CmdPipeList* next;
} CPL;

void appendCmd(CPL** header, CPL* newCPL) {
    CPL* tmp = *header;
    if(!tmp) {
        *header = newCPL;
        return;
    }
    while(tmp->next != NULL)tmp = tmp->next;
    tmp->next = newCPL;
}
CPL* createCPL(char** cmds,int wait) {
    CPL* newCPL = (CPL*)malloc(sizeof(CPL*));
    newCPL->cmds = cmds;
    newCPL->wait = wait;
    newCPL->next = NULL;
    return newCPL;
}
void dumpCPL(CPL* header){
    CPL* tmp = header;
    printf("%s\n","in" );
    while( tmp != NULL){
        printf("%d\n",tmp->wait);
        tmp = tmp->next;
    }
    printf("%s\n","out" );
}
void pushCommand(char**** cmds, char** cmd, int size){
}
int main()
{
    int fd[2], commands=0;
    char *cmds[3] = {"ls",NULL,NULL};
    //cmds = (char***)malloc(commands*sizeof(char***)+1);
    CPL* header = NULL;
    CPL* tmp = createCPL(cmds,2);
    appendCmd(&header,createCPL(cmds,2));
    appendCmd(&header,createCPL(cmds,2));
    appendCmd(&header,createCPL(cmds,2));
    appendCmd(&header,tmp);
    dumpCPL(header);
    return 0;
}
