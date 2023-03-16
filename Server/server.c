
//============================================================================
// Name         : Chat Server using Select()
// Description  : This Program will receive messages from several clients using
//                select() system class
//============================================================================
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/select.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
    //socket
    int server_sd = socket(AF_INET,SOCK_STREAM,0);
    printf("Server fd = %d \n",server_sd);
    if(server_sd<0)
    {
        perror("socket:");
        exit(-1);
    }
    //setsock
    int value  = 1;
    setsockopt(server_sd,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value)); //&(int){1},sizeof(int)
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9021);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY, INADDR_LOOP
    
    //bind
    if(bind(server_sd, (struct sockaddr*)&server_addr,sizeof(server_addr))<0)
    {
        perror("bind failed");
        exit(-1);
    }
    //listen
    if(listen(server_sd,5)<0)
    {
        perror("listen failed");
        close(server_sd);
        exit(-1);
    }
    
    fd_set full_fdset;
    fd_set read_fdset;
    FD_ZERO(&full_fdset);
    
    int max_fd = server_sd;
    
    FD_SET(server_sd,&full_fdset);
    
    printf("Server is listening...\n");
    char buffer[256];
    char bufferCopy[256];
    char pass[256];
    int client_sd;
    int client_sock;
    char space[1] = " ";
    int userCheck = 0;
    int isAuth = 0;
    char newPort[256];
    
    while(1)
    {
        printf("max fd = %d \n",max_fd);
        read_fdset = full_fdset;
        
        if(select(max_fd+1,&read_fdset,NULL,NULL,NULL)<0)
        {
            perror("select");
            exit (-1);
        }
        
        for(int fd = 3 ; fd<=max_fd; fd++)
        {
            if(FD_ISSET(fd,&read_fdset))
            {
                if(fd==server_sd)
                {
                    client_sd = accept(server_sd,0,0);
                    printf("Client Connected fd = %d \n",client_sd);
                    FD_SET(client_sd,&full_fdset);
                    if(client_sd>max_fd)
                        max_fd = client_sd;
                    
                }
                else
                {
                    bzero(buffer,sizeof(buffer));
                    int bytes = recv(fd,buffer,sizeof(buffer),0);
                    if(bytes==0)   //client has closed the connection
                    {
                        printf("connection closed from client side \n");
                        close(fd);
                        FD_CLR(fd,&full_fdset);
                        if(fd==max_fd)
                        {
                            for(int i=max_fd; i>=3; i--)
                                if(FD_ISSET(i,&full_fdset))
                                {
                                    max_fd =  i;
                                    break;
                                }
                        }
                    }
                    
                    else
                    {
                        
                        printf("buffer from client : %s \n", buffer);
                        char *token;
                        strcpy(bufferCopy,buffer);
                        token = strtok(bufferCopy, " ");
                        
                        if(strcmp(token, "USER") == 0 || strcmp(token, "PASS") == 0)
                        {
                            //getting strings from buffer
                            char *token1;
                            char input[20];
                            token1 = strtok(buffer, " "); // separate the first string
                            
                            while (token1 != NULL) {
                                strcpy(input, token1);
                                token1 = strtok(NULL, " "); // separate the next string
                            }
                            
                            if(strcmp(token, "USER") == 0)
                            {
                                FILE *fp;
                                char line[256], *username, *password;
                                int found = 0;
                                
                                fp = fopen("users.txt", "r");
                                if (fp == NULL) {
                                    printf("Error opening file.");
                                    return 1;
                                }
                                
                                while (fgets(line, 256, fp) != NULL) {
                                    username = strtok(line, ",");
                                    password = strtok(NULL, ",");
                                    
                                    if (strcmp(username, input) == 0) {
                                        strcpy(pass,password);
                                        pass[strcspn(pass, "\n")] = 0;
                                        found = 1;
                                        break;}
                                }
                                
                                if (!found)
                                {
                                    char *message = "530 Not logged in";
                                    if (send(fd, message, strlen(message), 0) < 0){
                                        perror("Error: send failed");
                                        exit(EXIT_FAILURE);}
                                }
                                
                                if(found)
                                {
                                    userCheck = 1;
                                    char *message = "Username OK, need password";
                                    if (send(fd, message, strlen(message), 0) < 0){
                                        perror("Error: send failed");
                                        exit(EXIT_FAILURE);}
                                }
                                fclose(fp);
                            }
                            
                            else if(strcmp(token, "PASS") == 0 )
                            {
                                //printf("input: %s \n",input);
                                //printf("pass: %s \n",input);
                                
                                if(strcmp(input,pass) == 0 && userCheck == 1)
                                {
                                    char *message = "230, User logged in, proceed";
                                    if (send(fd, message, strlen(message), 0) < 0){
                                        perror("Error: send failed");
                                        exit(EXIT_FAILURE);}
                                    isAuth = 1;
                                    bzero(pass, sizeof(pass));
                                }
                                else
                                {
                                    char *message = "530 Not logged in";
                                    if (send(fd, message, strlen(message), 0) < 0){
                                        perror("Error: send failed");
                                        exit(EXIT_FAILURE);}
                                    isAuth = 0;
                                }
                            }
                            
                        }
                        
                        else if(strcmp(token,"PORT") == 0 && isAuth == 1)
                        {
                            printf(bufferCopy);
                            //FIRST HANDLE PORT COMMAND
                            
                            //getting strings from original buffer
                            char *token1;
                            char input[20]; //contains the file name
                            token1 = strtok(buffer, " "); // separate the first string
                            while (token1 != NULL) {
                                strcpy(input, token1);
                                token1 = strtok(NULL, " "); // separate the next string
                            }
                            //printf("input : %s \n",input); //contains the new port
                            
                            strcpy(newPort,input);
                            
                            char *message = "200 PORT command successful.";
                            if (send(fd, message, strlen(message), 0) < 0){
                                perror("Error: send failed");
                                exit(EXIT_FAILURE);}
                        }
                        else if(strcmp(token,"RETR") == 0 && isAuth == 1)
                        {
                            //getting strings from original buffer
                            char *token1;
                            char input[20]; //contains the file name
                            token1 = strtok(buffer, " "); // separate the first string
                            while (token1 != NULL) {
                                strcpy(input, token1);
                                token1 = strtok(NULL, " "); // separate the next string
                            }
                            input[strcspn(input, "\n")] = 0;
                            //printf("input : %s \n",input); //contains the file name
                            
                            int port = atoi(newPort);
                            
                            int pid = fork(); //fork a child process
                            if(pid == 0)   //if it is the child process
                            {
                                close(server_sd);//SHOULD BE FD?
                                
                                if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
                                {   perror("socket failed");
                                    exit(EXIT_FAILURE);}
                                
                                int value = 1;
                                setsockopt(client_sock,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value));
                                
                                // Connect to the client's listening port
                                struct sockaddr_in client_address;
                                client_address.sin_family = AF_INET;
                                client_address.sin_addr.s_addr = inet_addr("127.0.0.1");
                                client_address.sin_port = htons(port);
                                
                                struct sockaddr_in data_server_addr;
                                bzero(&data_server_addr,sizeof(data_server_addr));
                                data_server_addr.sin_family = AF_INET;
                                data_server_addr.sin_port = htons(9020);
                                data_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                                
                                //NEED THE SERVER TO BIND TO PORT 9020
                                if(bind(client_sock, (struct sockaddr *)&data_server_addr, sizeof(data_server_addr)) < 0){
                                    perror("bind error:");
                                    exit(-1);
                                }
                                
                                // connect to the client address and port
                                if (connect(client_sock, (struct sockaddr *)&client_address, sizeof(client_address)) < 0){
                                    perror("Error: connect failed");
                                    exit(EXIT_FAILURE);}
                                
                                char* success = "150 File status okay; about to open data connection.";
                                if (send(client_sock, success, strlen(success), 0) < 0){
                                    perror("Error: send failed");
                                    exit(EXIT_FAILURE);}
                                
                                FILE* fptr = fopen(input, "r");
                                char fmsg[1024];
                                
                                while (fgets(fmsg, sizeof(fmsg), fptr))
                                {
                                    send(client_sock, fmsg, sizeof(fmsg), 0);
                                    bzero(fmsg, sizeof(fmsg));
                                }
                                fclose(fptr);
                                send(client_sock, fmsg, sizeof(fmsg), 0);
                                
                                success = "226 Transfer completed.";
                                if (send(client_sock, success, strlen(success), 0) < 0){
                                    perror("Error: send failed");
                                    exit(EXIT_FAILURE);}
                                
                                // close client socket and exit child process
                                close(client_sock);
                                exit(EXIT_SUCCESS);
                            }
                            
                        }
                        else if(strcmp(token,"STOR") == 0 && isAuth == 1)
                        {
                            
                            //getting strings from original buffer
                            char *token1;
                            char input[20]; //contains the file name
                            token1 = strtok(buffer, " "); // separate the first string
                            while (token1 != NULL) {
                                strcpy(input, token1);
                                token1 = strtok(NULL, " "); // separate the next string
                            }
                            
                            input[strcspn(input, "\n")] = 0;
                            
                            int port = atoi(newPort);
                            
                            int pid = fork(); //fork a child process
                            if(pid == 0)   //if it is the child process
                            {
                                close(server_sd);
                                
                                if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
                                {   perror("socket failed");
                                    exit(EXIT_FAILURE);}
                                
                                int value = 1;
                                setsockopt(client_sock,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value));
                                
                                // Connect to the client's listening port
                                struct sockaddr_in client_address;
                                client_address.sin_family = AF_INET;
                                client_address.sin_addr.s_addr = inet_addr("127.0.0.1");
                                client_address.sin_port = htons(port);
                                
                                struct sockaddr_in data_server_addr;
                                bzero(&data_server_addr,sizeof(data_server_addr));
                                data_server_addr.sin_family = AF_INET;
                                data_server_addr.sin_port = htons(9020);
                                data_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                                
                                //NEED THE SERVER TO BIND TO PORT 9020
                                if(bind(client_sock, (struct sockaddr *)&data_server_addr, sizeof(data_server_addr)) < 0){
                                    perror("bind error:");
                                    exit(-1);
                                }
                                
                                // connect to the client address and port
                                if (connect(client_sock, (struct sockaddr *)&client_address, sizeof(client_address)) < 0){
                                    perror("Error: connect failed");
                                    exit(EXIT_FAILURE);}
                                
                                char* success = "150 File status okay; about to open data connection.";
                                if (send(client_sock, success, strlen(success), 0) < 0){
                                    perror("Error: send failed");
                                    exit(EXIT_FAILURE);}
                                
                                FILE *fp = fopen(input, "w");
                                fclose(fp);
                                
                                FILE *fptr = fopen(input, "a");
                                
                                char fmsg[1024];
                                while(1)
                                {
                                    int b = recv(client_sock,fmsg, sizeof(fmsg), 0);
                                    if(strcmp(fmsg,"") == 0){
                                        break;}
                                    
                                    //printf("message : %s \n", fmsg);
                                    fprintf(fptr, "%s", fmsg);
                                    bzero(fmsg, sizeof(fmsg));
                                }
                                
                                fclose(fptr);
                                success = "226 Transfer completed.";
                                
                                if(send(client_sock, success, strlen(success), 0) < 0){
                                    perror("Error: send failed");
                                    exit(EXIT_FAILURE);}
                                
                                // close client socket and exit child process
                                close(client_sock);
                                exit(EXIT_SUCCESS);
                            }
                            
                        }//else if stor
                        else if (isAuth == 1)
                        {
                            printf(": %d \n",isAuth);
                            char* message = "202 Command not implemented.";
                            if(send(fd, message, strlen(message), 0) < 0){
                                perror("Error: send failed");
                                exit(EXIT_FAILURE);}
                        }
                        else
                        {
                            printf(": %d \n",isAuth);
                            char* message = "530 Not logged in.";
                            if(send(fd, message, strlen(message), 0) < 0){
                                perror("Error: send failed");
                                exit(EXIT_FAILURE);}
                        }
                    } //else
                }//another else
            }//if
            
        }//for
        
    }//while
    
    //close
    close(server_sd);
    return 0;
}//int main

