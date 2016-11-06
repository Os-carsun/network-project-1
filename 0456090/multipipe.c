/*
 * =====================================================================================
 *
 *       Filename:  multipipe.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/2016 04:27:10 AM
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
#include "pipe.h"
void loop_pipe(char ***cmd)
{
    int   p[2];
    pid_t pid;
    int   fd_in = 0;

    while (*cmd != NULL)
    {
        pipe(p);
        if ((pid = fork()) == -1)
        {
           exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            dup2(fd_in, 0); //change the input according to the old one
            if (*(cmd + 1) != NULL)
                dup2(p[1], 1);
            close(p[0]);
            execvp((*cmd)[0], *cmd);
            exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL);
            close(p[1]);
            fd_in = p[0]; //save the input for the next command
            cmd++;
        }
    }
}
//void loopCPLpipe(CPL** header) {
//    int p[2];
//    pid_t = pid;
//    int fd_in = 0;
//    CPL* tmp = *header;
//    while(tmp->next) {
//        pipe(p);
//        if ( (pid = fork()) == -1) {
//           exit(EXIT_FAILURE);
//        }else if(pid == 0) {
//            dup2(fd_in, 0);
//            if(tmp->cmds != NULL)
//                dup2(p[1], 1);
//            close(p[0]);
//            execvp(tmp->cmds[0], tmp->cmds);
//            exit(EXIT_FAILURE);
//        }
//    }else {
//        wait(NULL);
//        close(p[1]);
//        fd_in = p[0];
//        tmp = tmp->next;
//    }
//}
int main()
{
    char *ls[] = {"ls", NULL};
    char *ls2[] = {"ls", NULL};
    char *ls3[] = {"ls", NULL};
    char *grep[] = {"grep", "pipe", NULL};
    char *wc[] = {"wc", NULL};
    char **cmd[] = {ls, grep, wc, NULL};

    //loop_pipe(cmd);
    //loop_pipe(cmd);
    CPL* header = NULL;
    appendAndageing(&header, createCPL(ls,2));
    appendAndageing(&header, createCPL(grep,1));
    appendAndageing(&header, createCPL(wc,0));
    appendAndageing(&header, createCPL(ls2,0));
    appendAndageing(&header, createCPL(ls3,0));
   return (0);
}
