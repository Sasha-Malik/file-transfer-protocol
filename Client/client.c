#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <netinet/in.h>
#include<unistd.h>
#include<stdlib.h>


static int port(int h1, int h2, int h3, int h4, int p1, int p2, int server_sd){
    char msg[256];
    sprintf(msg, "PORT %d,%d,%d,%d,%d,%d", h1, h2, h3, h4, p1, p2);
    if(send(server_sd, msg, strlen(msg), 0) < 0){
        perror("send");
        exit(-1);
    }
    return 0;
}

int main()
{
    //socket
    int server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sd < 0){
        perror("socket:");
        exit(-1);
    }
    int i = 0;
    setsockopt(server_sd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9021);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(connect(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect:");
        exit(-1);
    }
    
    printf("Connected to server \n");
    //char myIP[16];
    unsigned int myPort;
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);
    getsockname(server_sd, (struct sockaddr *)&my_addr, &len);
    //inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    myPort = ntohs(my_addr.sin_port);
    
    unsigned char* myIP = (unsigned char*)&my_addr.sin_addr;
    //printf("%hhu,%hhu,%hhu,%hhu \n",h[0],h[1],h[2],h[3] );
    
    char buffer[256];
    char bufferCopy[256];
    
    char bufferCopy3[256];
    int isAuth = -1;
    int pid, transfersocket, status;
    unsigned int new_Port;
    while(1){
        char bufferTemp[256];
        
        bzero(buffer,sizeof(buffer));
        
        fgets(buffer,sizeof(buffer),stdin);
        
        if(strcmp(buffer,"\n") == 0){
        }
        else
            buffer[strcspn(buffer, "\n")] = 0;
        
        strcpy(bufferTemp,buffer);
        char *token;
        
        token = strtok(bufferTemp, " ");
        
        char tokenTemp[256];
        strcpy(tokenTemp,token);
        tokenTemp[strcspn(tokenTemp, "\n")] = 0;
        token[strcspn(token, "\n")] = 0;
        if(strcmp(token, "USER") == 0 || strcmp(token, "PASS") == 0 || strcmp(token, "QUIT") == 0)
        {
            
            if(send(server_sd, buffer, strlen(buffer),0)<0){
                perror("send");
                exit(-1);}
            bzero(buffer,sizeof(buffer));
            recv(server_sd,buffer,sizeof(buffer),0);
            if(strcmp(buffer, "230, User logged in, proceed") == 0){
                isAuth = 1;
            }
            printf("%s \n", buffer);
        }
        
        else if(isAuth == 1)
        {
            if(strcmp(token, "!CWD") == 0){
                char input[256];
                while (token != NULL) {
                    strcpy(input, token);
                    token = strtok(NULL, " "); // separate the next string
                }
                input[strcspn(input, "\n")] = 0;
                if(chdir(input) < 0){
                    //perror("Invalid directory:");
                    printf("550 No such file or directory. \n");
                    //exit(-1);
                }
            }
            else if(strcmp(tokenTemp, "!PWD") == 0){
                char input[256];
                getcwd(input, 256); // removed &
                printf("%s \n", input);
            }
            else if(strcmp(token, "!LIST") == 0){
                FILE* fptr = popen("ls -l", "r");
                char l[256];
                while(fgets(l, 255, fptr)!= NULL){
                    printf("%s\n", l);
                }
            }
            else if(strcmp(token, "CWD") == 0){
                if(send(server_sd, buffer, strlen(buffer),0)<0){
                    perror("send");
                    exit(-1);}
                bzero(buffer, sizeof(buffer));
                recv(server_sd, buffer, sizeof(buffer), 0);
                printf("%s \n", buffer);
                bzero(buffer, sizeof(buffer));
            }
            else if(strcmp(token, "PWD") == 0){
                if(send(server_sd, buffer, strlen(buffer),0)<0){
                    perror("send");
                    exit(-1);}
                bzero(buffer, sizeof(buffer));
                recv(server_sd, buffer, sizeof(buffer), 0);
                printf("%s \n", buffer);
                bzero(buffer, sizeof(buffer));
            }
            else if(strcmp(token, "LIST") == 0){
                if(send(server_sd, buffer, strlen(buffer),0)<0){
                    perror("send");
                    exit(-1);}
                
                char l[1024];
                while(1)
                {
                    int b = recv(server_sd, l, sizeof(l), 0);
                    if(strcmp(l,"") == 0){
                        break;}
                    if(b<= 0)
                        break;
                    
                    printf("%s", l);
                    bzero(l, sizeof(l));
                }
            }
            else if (strcmp(token, "STOR") == 0 ||strcmp(token, "RETR") == 0 )
            {
                i++;
                new_Port = myPort + i;
                printf("port:%d \n",new_Port);
                /*int ipInt = atoi("127.0.0.1");
                 
                 int d = (ipInt << 0) & 0xff;
                 int c = (ipInt << 8) & 0xff;
                 int b = (ipInt << 16) & 0xff;
                 int a = (ipInt << 24) & 0xff;*/
                int h1 = myIP[0], h2 = myIP[1], h3 = myIP[2], h4 = myIP[3];
                int p1 = new_Port/256;
                int p2 = new_Port%256;
                
                char msg[256];
                sprintf(msg, "PORT %d,%d,%d,%d,%d,%d", h1, h2, h3, h4, p1, p2);
                if(send(server_sd, msg, strlen(msg), 0) < 0){
                    perror("send");
                    exit(-1);
                }
                
                //port(a, b, c, d, e, f, server_sd);
                
                bzero(bufferCopy, sizeof(bufferCopy));
                recv(server_sd, &bufferCopy, sizeof(bufferCopy), 0);
                if(strcmp(bufferCopy, "200 PORT command successful"))
                {
                    printf("%s \n",bufferCopy);
                    send(server_sd, buffer, sizeof(buffer), 0);
                    
                    bzero(bufferCopy, sizeof(bufferCopy));
                    recv(server_sd, bufferCopy, sizeof(bufferCopy), 0);
                    //printf("buffer copy: %s \n", bufferCopy);
                    
                    pid = fork();
                    if(pid == 0){
                        close(server_sd);
                        
                        int transfersocketfd = socket(AF_INET, SOCK_STREAM, 0);
                        struct sockaddr_in data_server_addr;
                        bzero(&data_server_addr,sizeof(data_server_addr));
                        data_server_addr.sin_family = AF_INET;
                        data_server_addr.sin_port = htons(new_Port);
                        data_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                        if(bind(transfersocketfd, (struct sockaddr *)&data_server_addr, sizeof(data_server_addr)) < 0){
                            perror("bind error:");
                            exit(-1);
                        }
                        status = listen(transfersocketfd, 5);
                        if(status < 0){
                            perror("listen error:");
                            exit(-1);
                        }
                        int new_dedicated_data_sd = accept(transfersocketfd, 0, 0);
                        char bufferSend[256];
                        char toSend2[256];
                        strcpy(bufferSend,buffer);
                        char* tokenSend;
                        
                        tokenSend = strtok(bufferSend, " ");
                        while (tokenSend != NULL) {
                            strcpy(toSend2, tokenSend);
                            tokenSend = strtok(NULL, " "); // separate the next string
                        }
                        //printf("token : .%s. \n",token);
                        if(strcmp(token, "STOR") == 0){
                            
                            toSend2[strcspn(toSend2, "\n")] = 0;
                            printf(".%s.\n",toSend2);
                            FILE* fptr = fopen(toSend2, "r");
                            if(fptr == NULL){
                                printf("550 No such file or directory. \n");
                            }
                            else{
                                
                                /*bzero(buffer, sizeof(buffer));
                                 recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);*/
                                printf("%s \n", bufferCopy);
                                
                                
                                char fmsg[1024];
                                while (fgets(fmsg, 1024, fptr) != NULL) {
                                    //printf("%s \n", fmsg);
                                    send(new_dedicated_data_sd, fmsg, sizeof(fmsg), 0);
                                    bzero(fmsg, sizeof(fmsg));
                                }
                                
                                fclose(fptr);
                                
                                send(new_dedicated_data_sd, fmsg, sizeof(fmsg), 0);
                                bzero(buffer, sizeof(buffer));
                                recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
                                printf("%s \n", buffer);
                            }
                            
                        }
                        else if(strcmp(token, "RETR") == 0){
                            /*bzero(buffer, sizeof(buffer));
                             recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);*/
                            printf("%s \n", bufferCopy);
                            
                            if(strcmp(bufferCopy,"550 No such File or Directory.") != 0)
                            {
                                FILE *fp = fopen(toSend2, "w");
                                fclose(fp);
                                
                                FILE* fptr = fopen(toSend2, "a");
                                char fmsg[1024];
                                while(1)
                                {
                                    int b = recv(new_dedicated_data_sd,fmsg, sizeof(fmsg), 0);
                                    if(strcmp(fmsg,"") == 0){break;}
                                    
                                    if(b <= 0){
                                        break;}
                                    
                                    fprintf(fptr, "%s", fmsg);
                                    bzero(fmsg, sizeof(fmsg));
                                }
                                fclose(fptr);
                                
                                bzero(buffer, sizeof(buffer));
                                recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
                                printf("%s \n", buffer);
                            }
                        }
                        
                        else if(strcmp(token, "LIST") == 0){
                            recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
                            printf("%s \n", buffer);
                        }
                        
                        close(new_dedicated_data_sd);
                    }
                    
                }
                else{
                    perror("not successful:");
                    exit(-1);
                }
                
            }
            
            else //Wrong command
            {
                if(send(server_sd, buffer, strlen(buffer),0)<0){
                    perror("send");
                    exit(-1);}
                bzero(buffer,sizeof(buffer));
                recv(server_sd,buffer,sizeof(buffer),0);
                printf("%s \n", buffer);
            }
            
        }
        
        else
        {
            if(send(server_sd, buffer, strlen(buffer),0)<0){
                perror("send");
                exit(-1);}
            bzero(buffer,sizeof(buffer));
            recv(server_sd,buffer,sizeof(buffer),0);
            printf("%s \n", buffer);
        }
        
    }
    
    // Close the socket
    close(server_sd);
    
    return 0;
}
