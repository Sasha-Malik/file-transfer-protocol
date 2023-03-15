#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <netinet/in.h>

#include<unistd.h>
#include<stdlib.h>



int main()
{
    //socket
    int server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sd < 0){
        perror("socket:");
        exit(-1);
    }
    int i = 0;
    setsockopt(server_sd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)); //&(int){1},sizeof(int)
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        
    if(connect(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect:");
        exit(-1);
    }
    
    printf("Connected to server \n");
    
    char myIP[16];
    unsigned int myPort;
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);
    getsockname(server_sd, (struct sockaddr *)&my_addr, &len);
    inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    myPort = ntohs(my_addr.sin_port);
    
    char buffer[256];
    char bufferCopy[256];
    char bufferCopy2[256];
    int isAuth = -1;
    int new_Port, pid, transfersocket, status;
    while(1){
        char bufferTemp[256];
        bzero(buffer,sizeof(buffer));
        fgets(buffer,sizeof(buffer),stdin);
        strcpy(bufferTemp,buffer);
        char *token;
        token = strtok(bufferTemp, " ");

        if(strcmp(token, "USER") == 0 || strcmp(token, "PASS") == 0 || strcmp(token, "CWD") == 0 || strcmp(token, "QUIT") == 0 || strcmp(token, "PWD") == 0 || strcmp(token, "LIST") == 0)
        {
            buffer[strcspn(buffer, "\n")] = 0;
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
        else if(strcmp(token, "!CWD") == 0){
			bzero(bufferCopy2,sizeof(bufferCopy2));
            fgets(bufferCopy2,sizeof(bufferCopy2),stdin);
			chdir(bufferCopy2);
		}
		else if(strcmp(token, "!PWD") == 0){
			bzero(bufferCopy2,sizeof(bufferCopy2));
            getcwd(bufferCopy2, 256); // removed &
			printf("%s \n", bufferCopy2);
		}
		else if(strcmp(token, "!LIST") == 0){
			FILE* fptr = popen("ls -l", "r");			
			char l[256];
			while(fgets(l, 255, fptr)!= NULL){
				printf("%s\n", l);
			}
		}
        else
        {
            i++;
            new_Port = myPort + i;
            //printf("%d",new_Port);
            char msg[256] = "PORT 127.0.0.1";
            char* sp = " ";
            strcat(msg, sp);
            char nP[256];
            sprintf(nP, "%d", new_Port);
            strcat(msg, nP);
            if(send(server_sd, msg, strlen(msg), 0) < 0){
                perror("send");
                exit(-1);
            }
            //printf("sent from client");
            bzero(bufferCopy, sizeof(bufferCopy));
            recv(server_sd, &bufferCopy, sizeof(bufferCopy), 0);
            if(strcmp(bufferCopy, "200 PORT command successful"))
            {
                printf("%s \n",bufferCopy);
                printf("buffer : %s \n",buffer);
                send(server_sd, buffer, sizeof(buffer), 0);
             
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
                    
                    if(strcmp(token, "RETR") == 0){
                        recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
                        printf("%s \n", buffer);
                        FILE* fptr = fopen(toSend2, "w");
                        while(fgets(buffer, strlen(buffer), fptr) != NULL){
                        	fprintf(fptr, "%s", buffer);	
                        }
                        fclose(fptr);
                        bzero(buffer, sizeof(buffer));
                        recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
                        printf("%s \n", buffer);
                    }
                    else if(strcmp(token, "STOR") == 0){
                        FILE* fptr = fopen(toSend2, "r");
                        char fmsg[2048];
                        while(fgets(fmsg, strlen(fmsg), fptr) != NULL){
                        	send(new_dedicated_data_sd, fmsg, sizeof(fmsg), 0);	
                        }
                        bzero(buffer, sizeof(buffer));
                        recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
                        printf("%s \n", buffer);
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

    }

    // Close the socket
    close(server_sd);

    return 0;
}
