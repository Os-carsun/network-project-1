/*
 ** server.c -- a stream socket server demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "pipe.h"
#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
#define BUFFER_SIZE 15000
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void renderStrToSocket(int socket, char* str)
{
    send(socket, str, sizeof(str), 0);
}

void remove_crlf(char* source)
{
    int length = strlen(source);
    if(length >= 2 ) {
        if((int)(source[length-2]) == 13 && (int)(source[length-1]) == 10) source[length-2]='\0';
        else if((int)(source[length-1]) == 10) source[length-1] = '\0';
    }
}
int command_switch(int argc, char** argv, int socket, CPL** cmdList)
{
    if(argc <= 1) {// one command no arg
        if(strcmp(argv[0],"exit") == 0) {
            close(socket);
            return 0;
        }
        //execute_cmd(socket, argv[0], argv);
        appendAndageing(socket, cmdList, createCPL(argv,0));
    }else if(argc >= 3) {
        int havePipe = 0, i, b;
        for(i=1,b=0; i<argc; i++) {
            if(argv[i][0] == '|') {
                char** subcmd = (char**) malloc (sizeof(char**)*(i-1-b)+1);
                int wait = 0;
                for(int k = b; k<i; k++) {
                    subcmd[k-b] = argv[k];
                }
                subcmd[i-b] = NULL;
                if(argv[i][1] != '\0') {
                    wait = atoi(&(argv[i][1]));
                }
                appendAndageing(socket, cmdList, createCPL(subcmd,wait));
                if(i < argc) {//ignore | command
                    i++;
                    b=i;
                }
                havePipe = 1;
            }
        }
        if(havePipe == 0) {
            appendAndageing(socket, cmdList, createCPL(argv, 0));
        } else if(b != i){// have last cmd

                fprintf(stderr, "%s ,%s ,%s, i=%d,b=%d\n ",argv[0], argv[1], argv[2],i,b);
            appendAndageing(socket, cmdList, createCPL(&(argv[i-b+1]), 0));
        }
    }else {// one command one arg
        int wait = 0;
        if(argv[1][0] == '|') {
            if(argv[1][0] != '\0') {
                wait = atoi(&argv[1][1]);
            }
            argv[1] = NULL;
            appendAndageing(socket, cmdList, createCPL(argv,wait));
            return;
        }
        appendAndageing(socket, cmdList, createCPL(argv,wait));
        //execute_cmd(socket, argv[0], argv);
        return 0;
    }
    return 0;
}
void free2d(char** source, int count)
{
    for(int k=0; k < count; k++){
        char* tmp = source[k];
        free(tmp);
    }
    free(source);
}
void parse_recv_data(int socket)
{
    char* saveptr = NULL;
    char* substr = NULL;
    int i = 0;
    const char const *delim = " ";
    char* inputData = NULL;
    char** command = NULL;
    int count = 0;
    CPL* header = NULL;
    setenv("PATH","bin:.",1);
    while(1){
        // init part
        i = 0;
        send(socket,"%",1,0);
        inputData = (char*)malloc(sizeof(char*)*(BUFFER_SIZE+1));
        command = (char**)malloc(sizeof(char**) * i + 1);
        memset(inputData, 0, sizeof(char*) * BUFFER_SIZE);
        //
        if( count = recv(socket, inputData, BUFFER_SIZE, 0) < 0 )
        {
            return;
        }else {
            substr = strtok_r(inputData, delim, &saveptr);
            do{
                remove_crlf(substr);
                command[i] = (char*)malloc(sizeof(char*) * strlen(substr) + 1);
                strcpy(command[i], substr);
                substr = strtok_r(NULL, delim, &saveptr);
                if(substr != NULL){
                    i++;
                    command = (char**)realloc(command, sizeof(char**) * i + 1);
                }
            }while(substr != NULL);
            i++;
            command = (char**)realloc(command, sizeof(char**) * i + 1);
            command[i] = NULL;
            command_switch(i, command, socket, &header);
            for(int k=0; k<=i; k++){
                free(command[k]);
                command[k] = NULL;
            }
            free(command);
            free(inputData);
            inputData = NULL;
            command = NULL;
            saveptr = NULL;
        }
    }
}

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                    sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            const char* message[]={"****************************************\n",
"** Welcome to the information server. **\n",
"****************************************\n",
"** You are in the directory, /home/studentA/ras/.\n",
"** This directory will be under \"/\", in this system.\n",
"** This directory includes the following executable programs.\n",
"**\n",
"**  bin/\n",
"**  test.html   (test file)\n",
"**\n",
"** The directory bin/ includes:\n",
"**  cat\n",
"**  ls\n",
"**  removetag       (Remove HTML tags.)\n",
"**  removetag0      (Remove HTML tags.)\n",
"**  number          (Add a number in each line.)\n",
"**\n",
"** In addition, the following two commands are supported by ras.\n",
"**  setenv\n",
"**  printenv\n",
"**\n"};
            //if (send(new_fd, "Hello, world!", 13, 0) == -1)
                //perror("send");
            for(int i=0; i<21; i++) {
                send(new_fd, message[i], strlen(message[i]), 0);
            }
            parse_recv_data(new_fd);
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}

