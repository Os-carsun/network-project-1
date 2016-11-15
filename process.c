/*
   pipe(p_fd[tar]);
 * =====================================================================================
 *
 *       Filename:  process.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/09/2016 10:01:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "inputparser.h"
#define PMAX 1000
typedef struct process {
    int id;
    int needtrue;
    int numberPipe;
    char* fileOutput;
    struct process* pipeTo;
    char** cmdsPtr;
} process;
static int p_fd[PMAX][2] ;

char** getPATHenv(void) {
    char *path = getenv("PATH");
    char  pathenv[strlen(path) + sizeof("PATH=")];
    sprintf(pathenv, "PATH=%s", path);
    char *envp[] = {pathenv, NULL};
    return envp;
}

void loop_pipe(process *cmd, int id) {
    process* ptr = cmd;
    int fd_in=p_fd[id][0] ;
    int pid;
    char buff[1000];
    int nb = 0;
    close(p_fd[id][1]);
    while (ptr != NULL) {
        if(p_fd[id][0]!=-1 && p_fd[id][1]!=-1){
        }else{
            pipe(p_fd[id]);
        }
        if( (pid = fork()) == -1){
            fprintf(stderr, "%s\n", "fork error");
            exit(1);
        }else if(pid == 0) {// child Process
            dup2(fd_in, 0);
            if(ptr-> pipeTo != NULL)
                dup2(p_fd[id][1], 1);
            close(p_fd[id][0]);
            p_fd[id][0]=-1;
            if(execvpe(ptr->cmdsPtr[0], ptr->cmdsPtr, getPATHenv())<0) {
            }
            exit(1);
        }else {//farther Process
            wait(NULL);
            close(p_fd[id][1]);
            p_fd[id][1]=-1;
            fd_in = p_fd[id][0];
            ptr = ptr->pipeTo;
        }
    }
    close(p_fd[id][0]);
    close(p_fd[id][1]);
    p_fd[id][0]=-1;
    p_fd[id][1]=-1;
}
void loop_pipe_number(process *cmd, int id, int tar) {
    process* ptr = cmd;
    int fd_in=p_fd[id][0] ;
    int pid;
    int save_out = dup(fileno(stdout));
    close(p_fd[id][1]);
    while (ptr != NULL) {

        if(p_fd[id][0]!=-1 && p_fd[id][1]!=-1){
        }else{
            pipe(p_fd[id]);
        }
        if( (pid = fork()) == -1){
            fprintf(stderr, "%s\n", "fork error");
            exit(1);
        }else if(pid == 0) {// child Process
            dup2(fd_in, 0);
            if(ptr-> pipeTo != NULL)
                dup2(p_fd[id][1], 1);
            else
                dup2(p_fd[tar][1],fileno(stdout));
            close(p_fd[id][0]);
            p_fd[id][0]=-1;
            execvpe(ptr->cmdsPtr[0], ptr->cmdsPtr, getPATHenv());
            exit(1);
        }else {//farther Process
            wait(NULL);
            close(p_fd[id][1]);
            p_fd[id][1] = -1;
            fd_in = p_fd[id][0];
            ptr = ptr->pipeTo;
        }
    }
    dup2(save_out,fileno(stdout));
    p_fd[id][0]=-1;
    p_fd[id][1]=-1;
}
void initalProcessPool(process** p,int size) {
    for(int i=0; i<size; i++){
        (*p)[i].id = -1;
        (*p)[i].needtrue = 0;
        (*p)[i].numberPipe = -1;
        (*p)[i].fileOutput = NULL;
        (*p)[i].pipeTo = NULL;
        (*p)[i].cmdsPtr = NULL;
    }
}
void doCommand(char*** cmds, int *processID){
    int commands = 0;
    int begin = *processID;
    process* processPool = NULL;
    setenv("PATH",".:./bin",1);
    while(cmds[commands] != NULL) commands++;
    processPool = (process*)malloc(sizeof(process) * (commands+1));
    initalProcessPool(&processPool, commands);
    for (int i = 0; i < commands; i++) {
        switch (isOP(cmds[i][0])) {
            case 0://|
                if(strlen(cmds[i][0]) > 1) {//number pipe
                    if(i >=1 ){
                        processPool[i-1].numberPipe = (*processID + atoi(&cmds[i][0][1]) -1);
                        fprintf(stderr, "%d \n",processPool[i-1].numberPipe);
                    }else {
                        fprintf(stderr, "wrong pipe\n");
                        return;
                    }
                }else {//pipe
                    if( i >= 1 && i + 1 <= commands) {
                        processPool[i-1].pipeTo = &(processPool[i+1]);
                        processPool[i+1].id = -2;
                    }else {
                        fprintf(stderr, "wrong pipe\n");
                        return;
                    }
                }
                break;
            case 1://&&
                if(i >= 1) {
                    processPool[i-1].needtrue = 1;
                }else {
                    fprintf(stderr, "wrong && \n");
                }
                break;
            case 2://>
                if( i >= 1 && i + 1 <= commands) {
                    processPool[i-1].fileOutput = cmds[i + 1][0];
                    i++;
                }else {
                    fprintf(stderr, "wrong fileOutput\n");
                }
                break;
            default:
                processPool[i].cmdsPtr = cmds[i];
                //processPool[i].id = processPool[i].id == -1 ?(*processID): -2;
                //(*processID)+=1;
                if(processPool[i].id == -1){
                    processPool[i].id = (*processID);
                    (*processID)+=1;
                }
        }
    }
    for(int i = 0; i < commands; i++) {
        if(processPool[i].id < 0) continue;


        if(isOP(processPool[i].cmdsPtr[0]) == 3 )exit(1);

        if(isOP(processPool[i].cmdsPtr[0]) == 4 ) {
            setenv(processPool[i].cmdsPtr[1], processPool[i].cmdsPtr[2], 1);
        }

        if(isOP(processPool[i].cmdsPtr[0]) == 5 ) {
            fprintf(stderr, "%s=%s\n",processPool[i].cmdsPtr[1], getenv(processPool[i].cmdsPtr[1]));
        }

        int save_out,out;
        if(processPool[i].fileOutput != NULL){
            out = open(processPool[i].fileOutput, O_RDWR|O_CREAT|O_APPEND, 0600);
            save_out = dup(fileno(stdout));
            if (-1 == dup2(out, fileno(stdout))) { perror("cannot redirect stdout"); }
        }
        if(processPool[i].pipeTo != NULL) {
            process* ptr = processPool[i].pipeTo;
            while(ptr->pipeTo != NULL)ptr = ptr->pipeTo;
            if(ptr->numberPipe != -1) {
                processPool[i].numberPipe = ptr->numberPipe;
            }
            //to check num pipe after pipe
            if(processPool[i].numberPipe != -1) {
                int tar = (processPool[i].numberPipe) % PMAX;
                if(p_fd[tar][0]==-1) pipe(p_fd[tar]);
                loop_pipe_number(&processPool[i], (processPool[i].id) % PMAX, tar);
            }
            else{
                loop_pipe(&processPool[i], (processPool[i].id) % PMAX);
            }
        }
        else if(processPool[i].numberPipe != -1){
            //do sth num pipe
            int tar = ( processPool[i].numberPipe) % PMAX;
            if(p_fd[tar][0]==-1) pipe(p_fd[tar]);
            loop_pipe_number(&processPool[i], (processPool[i].id) % PMAX, tar);
        }else {
            loop_pipe(&processPool[i], (processPool[i].id) % PMAX);
        }
        if(processPool[i].fileOutput != NULL){
            fflush(stdout); close(out);
            dup2(save_out, fileno(stdout));
            close(save_out);
        }
    }
    initalProcessPool(&processPool, commands);
}
void pipeInit(){

    for (int i = 0; i < PMAX; ++i) {
        p_fd[i][0] = -1;
        p_fd[i][1] = -1;
    }

}
