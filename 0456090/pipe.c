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
void loopCPLpipe(CPL** header,int fd) {
    int p[2];
    pid_t pid;
    int fd_in = 0;
    char* command[] = {"cat", "removetag", "removetag0", "number", "ls"};
    char* envs[] = {getenv("PATH"),NULL};
    while(*header != NULL) {
        pipe(p);
        if ( (pid = fork()) == -1) {
            //  exit(EXIT_FAILURE);
        }else if(pid == 0) {
            //close(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            //dup2(fd_in, 0);
            if((*header)->next != NULL)
                dup2(p[1], 1);
            close(p[0]);
            if(strcmp("setenv",(*header)->cmds[0])==0){
                setenv((*header)->cmds[1],(*header)->cmds[2],1);
                return;
            }
            if(strcmp("printenv",(*header)->cmds[0])==0){

                fprintf(stderr, "%s=%s\n", (*header)->cmds[1],getenv((*header)->cmds[1]));
                return;
            }
            for(int i=0; i<5; i++){
                if(strcmp(command[i],(*header)->cmds[0])==0){
                    execvpe((*header)->cmds[0], (*header)->cmds, envs);
                    break;
                }else if(i==4){
                    fprintf(stderr, "Unknown commad \"%s\" \n", (*header)->cmds[0]);
                    (*header) = (*header)->next;
                }
            }
            exit(EXIT_FAILURE);
        }else {
            wait(NULL);
            close(p[1]);
            fd_in = p[0];
            (*header) = (*header)->next;
        }
    }
}
static void execute_cmd(int fd, CPL**header) {
    pid_t cid;
    size_t i = 0, c = 0;
    // cid = fork();

    // if(cid == 0) {
    //close(STDOUT_FILENO);
    //dup2(fd, STDOUT_FILENO);
    //dup2(fd, STDERR_FILENO);
    loopCPLpipe(header,fd);
    // }else {
    //waitpid(cid, NULL, 0);
    //}
}
void appendAndageing(int socket, CPL **header, CPL* newCPL) {
    ageingWait(header);
    appendCmd(header, newCPL);
    CPL* tmp = arrange(header);
    if(tmp != NULL){
        //loopCPLpipe(&tmp);
        execute_cmd(socket, &tmp);
    }
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
