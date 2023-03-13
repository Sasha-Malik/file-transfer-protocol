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
    server_addr.sin_port = htons(9000);
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
    int client_sd;
    int client_sock;
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
                        int port = atoi(buffer);
                        printf("%s \n",buffer);
                        
                        int pid = fork(); //fork a child process
                        if(pid == 0)   //if it is the child process
                        {
                            close(client_sd);
                            
                            if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
                            {
                                perror("socket failed");
                                exit(EXIT_FAILURE);
                            }
                           
                            int value = 1;
                            setsockopt(client_sock,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value)); //&(int){1},sizeof(int)
                            // Connect to the client's listening port
                            struct sockaddr_in client_address;
                            client_address.sin_family = AF_INET;
                            client_address.sin_addr.s_addr = inet_addr("127.0.0.1");
                            client_address.sin_port = htons(port);
                            
                           
                            // connect to the client address and port
                            if (connect(client_sock, (struct sockaddr *)&client_address, sizeof(client_address)) < 0){
                                perror("Error: connect failed");
                                exit(EXIT_FAILURE);}

                            // send a message to the client
                            char *message = "Hello, client!";
                            if (send(client_sock, message, strlen(message), 0) < 0){
                                perror("Error: send failed");
                                exit(EXIT_FAILURE);}
                            bzero(buffer,sizeof(buffer));
                            
                            recv(client_sock,buffer,sizeof(buffer),0);
                            printf("%s \n",buffer);
                            bzero(buffer,sizeof(buffer));

                            // close client socket and exit child process
                            close(client_sock);
                            exit(EXIT_SUCCESS);
                        }
                        
                        else
                        {
                            //parent process
                            close(client_sock);
                        }
                    }
                    

                }
            }
        }

    }

    //close
    close(server_sd);
    return 0;
}
