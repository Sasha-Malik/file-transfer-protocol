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
    int server_sd = socket(AF_INET,SOCK_STREAM,0);
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
    server_addr.sin_port = htons(9000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY, INADDR_LOOP

    //connect
    if(connect(server_sd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
    {
        perror("connect");
        exit(-1);
    }
    
    
    char buffer[256];
    fgets(buffer,sizeof(buffer),stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if(send(server_sd,buffer,strlen(buffer),0)<0)
    {
        perror("send");
        exit(-1);
    }
    
    close(server_sd);
    
    int port = atoi(buffer);
    server_addr.sin_port = htons(port);
    
    server_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

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
    
    
    
    // Change the port number
    
    int connfd, newsockfd;
    
    while (1)
    {
        
        newsockfd = accept(server_sd, 0, 0);
        if (newsockfd < 0) {
            perror("Failed to accept incoming connection");
            exit(EXIT_FAILURE);
        }
        
        recv(newsockfd,buffer,sizeof(buffer),0);
        printf("%s \n",buffer);
        bzero(buffer,sizeof(buffer));
        
        fgets(buffer,sizeof(buffer),stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if(send(newsockfd,buffer,strlen(buffer),0)<0)
        {
            perror("send");
            exit(-1);
        }
        bzero(buffer,sizeof(buffer));

        // Close the new connection
        close(newsockfd);
    }
  
    // Fork new processes to handle incoming connections
    /*while (1)
    {
        
        connfd = accept(server_sd, 0, 0);
        if (connfd < 0) {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        }

        // Fork a new process to handle the connection
        int pid = fork();
        if (pid < 0) {
            perror("Failed to fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            
            // Child process: handle the connection
        
            recv(connfd,buffer,sizeof(buffer),0);
            printf("%s \n",buffer);

            // Close the connection
            close(connfd);

            // Exit the child process
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process: continue accepting new connections
            close(connfd);
        }
    }*/

    // Close the socket
    close(server_sd);

    return 0;
}
