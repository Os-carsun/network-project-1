/*
 * =====================================================================================
 *
 *       Filename:  pipe.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/2016 07:55:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
typedef struct CmdPipeList {
    char** cmds;
    int wait;
    struct CmdPipeList* next;
} CPL;

void loopCPLpipe(CPL**);
void appendCmd(CPL** , CPL*);
CPL* createCPL(char**, int);
void ageingWait(CPL**);
CPL* removeCPL(CPL**, CPL*);
CPL* arrange(CPL **);
void appendAndageing(CPL **, CPL*);
void dumpCPL(CPL*);
