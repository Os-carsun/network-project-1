/*
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
typedef struct process {
    int id;
    int needtrue;
    char*** cmdsPtr;
} process;
void doCommand(char*** cmds){
       
       for (int i = 0; cmds[i] != NULL; ++i) {
           for (int k = 0; cmds[i][k] != NULL; k++) {
               fprintf(stderr, "%s\n", cmds[i][k]);
           }
       }


}
