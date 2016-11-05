#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    CPL* prevLast = *header;
    CPL* tmp = *header;
    CPL* prev = NULL;
    CPL* removed = NULL;
    CPL* exeCPL = NULL;
    while(last->next) {
        prevLast = last;
        last=last->next;
    }
    if(last->wait != 0) {//last cmd still need wait
        while(tmp) {
            if(tmp->wait == 0  && prevLast != tmp && tmp->next != NULL && tmp->next->wait != 0){
                last->next = tmp->next;
                tmp->next = last;
                prevLast->next = NULL;
                break;
            }
            tmp = tmp->next;
        }
        tmp = *header;
        while(tmp) {
            if(tmp->wait == 0) {
                tmp->wait = last->wait;
            }
            tmp = tmp->next;
        }
    }else {// last cmd can exec
        while(tmp) {
            if(tmp-> wait == 0) {
                if(tmp == *header) {
                    removed = removeCPL(header, tmp);
                }
                else {
                    removed = removeCPL(&(prev->next), tmp);
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
    if(tmp != NULL){
        loopCPLpipe(&tmp);
    }
    /* if(tmp != NULL) {
       printf("/////\n");
       dumpCPL(tmp);
       printf("/////\n");
       }*/
}
void dumpCPL(CPL* header) {
    CPL* tmp = header;
    printf("%s\n","----" );
    if(tmp == NULL) printf("%s\n","NULL" );
    while( tmp != NULL){
        printf("%s %d ->",(tmp->cmds)[0],tmp->wait);
        tmp = tmp->next;
    }
    printf("\n%s\n","=====" );
}
void loopCPLpipe(CPL** header) {
    int p[2];
    pid_t pid;
    int fd_in = 0;
    CPL* tmp = *header;
    while(tmp->next) {
        pipe(p);
        if ( (pid = fork()) == -1) {
            exit(EXIT_FAILURE);
        }else if(pid == 0) {
            dup2(fd_in, 0);
            if(tmp->cmds != NULL)
                dup2(p[1], 1);
            close(p[0]);
            execvp(tmp->cmds[0], tmp->cmds);
            exit(EXIT_FAILURE);
        }else {
            wait(NULL);
            close(p[1]);
            fd_in = p[0];
            tmp = tmp->next;
        }
    }
}
//int main()
//{
//    int fd[2], commands=0;
//    char *cmds1[3] = {"A",NULL,NULL};
//    char *cmds2[3] = {"B",NULL,NULL};
//    char *cmds3[3] = {"C",NULL,NULL};
//    char *cmds4[3] = {"D",NULL,NULL};
//    char *cmds5[3] = {"E",NULL,NULL};
//    char *cmds6[3] = {"F",NULL,NULL};
//    //cmds = (char***)malloc(commands*sizeof(char***)+1);
//    CPL* header = NULL;
//    appendAndageing(&header,createCPL(cmds1,3));
//    appendAndageing(&header,createCPL(cmds2,1));
//    appendAndageing(&header,createCPL(cmds3,2));
//    appendAndageing(&header,createCPL(cmds4,1));
//    appendAndageing(&header,createCPL(cmds5,0));
//    appendAndageing(&header,createCPL(cmds5,0));
//    appendAndageing(&header,createCPL(cmds5,0));
//
//    //dumpCPL(header);
//    return 0;
//}
