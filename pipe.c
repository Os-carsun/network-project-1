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
    newCPL->next = NULL;
}
CPL* createCPL(char** cmds,int wait) {
    CPL* newCPL = (CPL*)malloc(sizeof(CPL*));
    newCPL->cmds = cmds;
    newCPL->wait = wait;
    newCPL->next = NULL;
    return newCPL;
}
void ageingWait(CPL** header) {
    CPL* tmp = *header;
    while(tmp) {
        tmp->wait--;
        tmp = tmp->next;
    }
}
CPL* removeCPL(CPL** prev, CPL* target) {
    *prev = target->next;
    return target;
}
CPL* arrange(CPL **header){
    CPL* last = *header;
    CPL* tmp = *header;
    CPL* prev = NULL;
    CPL* removed = NULL;
    CPL* exeCPL = NULL;
    while(last->next) last=last->next;
    if(last->wait != 0){//last cmd still need wait
        while(tmp) {
            tmp->wait =  tmp->wait == 0? last->wait : tmp->wait;
            tmp = tmp->next;
        }
    }else {// last cmd can exec
        while(tmp) {
            if(tmp-> wait == 0) {
                if(tmp == *header) {
                    printf("in\n");
                   removed = removeCPL(header, tmp);
                }
                else {
                    printf("??\n");
                   removed = removeCPL(prev->next, tmp);
                }
            }
            prev = tmp;
            tmp = tmp->next;
            if(removed != NULL){
                appendCmd(&exeCPL, removed);
                removed-> next = NULL;
                removed = NULL;
            }
        }
    }
    return exeCPL;
}
void appendAndageing(CPL **header, CPL* newCPL) {
    ageingWait(header);
    appendCmd(header, newCPL);
    CPL* tmp = arrange(header);
        //dumpCPL(*header);
    /*  if(tmp != NULL) {
        printf("/////\n");
        dumpCPL(tmp);
        printf("/////\n");
        dumpCPL(*header);
    }*/
}
void dumpCPL(CPL* header){
    CPL* tmp = header;
    printf("%s\n","----" );
    if(tmp == NULL) printf("%s\n","NULL" );
    while( tmp != NULL){
        printf("%s %d ->",(tmp->cmds)[0],tmp->wait);
        tmp = tmp->next;
    }
    printf("\n%s\n","=====" );
}
void pushCommand(char**** cmds, char** cmd, int size){
}
int main()
{
    int fd[2], commands=0;
    char *cmds1[3] = {"A",NULL,NULL};
    char *cmds2[3] = {"B",NULL,NULL};
    char *cmds3[3] = {"C",NULL,NULL};
    char *cmds4[3] = {"D",NULL,NULL};
    //cmds = (char***)malloc(commands*sizeof(char***)+1);
    CPL* header = NULL;
    appendAndageing(&header,createCPL(cmds1,3));
    appendAndageing(&header,createCPL(cmds2,2));
    appendAndageing(&header,createCPL(cmds3,1));
    appendAndageing(&header,createCPL(cmds4,0));

    //dumpCPL(header);
    return 0;
}
