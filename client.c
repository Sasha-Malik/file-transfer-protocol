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
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	int server_sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0 || server_sd < 0){
		perror("socket:");
		exit(-1);
	}
	int i = 0;
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)); //&(int){1},sizeof(int)
	struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(21);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	char myIP[16];
	unsigned int myPort;
	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));
	getsockname(sockfd, (struct sockaddr *)&my_addr, &sizeof(my_addr));
	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
	myPort = ntohs(my_addr.sin_port);
	
	if(connect(server_sd, (struct sockaddr *)&server_addr, sizeof(serv_addr)) < 0){
		perror("connect:")
		exit(-1);
	}
	char buffer[256];
	char bufferCopy[256];
	char space[1] = " ";
	int isAuth = -1;
	int new_Port, pid, transfersocket, status, client_sockfd, new_dedicated_data_sd;
	while(1){
		fgets(buffer,sizeof(buffer),stdin);
		strcpy(bufferCopy, buffer);
		char* bc = strtok(bufferCopy, space);
		if(strcmp(bc, "USER") == 0 || strcmp(bc, "PASS") == 0 || strcmp(bc, "CWD") == 0 || strcmp(bc, "QUIT") == 0){		
		buffer[strcspn(buffer, "\n")] = 0;
		if(send(server_sd, buffer,strlen(buffer),0)<0)
		{
		    perror("send");
		    exit(-1);
		}
		recv(server_sd,buffer, 0, 0);
		if(strcmp(buffer, "230, User logged in, proceed") == 0){
			isAuth = 1;
		}
		printf("%s\n", buffer);
		}
		else{
			new_Port = myPort + i++;
			msg = strcat(msg, "PORT 127.0.0.1");
			msg = strcat(msg, " %s", itoa(new_Port));
			if(send(server_sd, msg, strlen(msg), 0) < 0){
				perror("send");
				exit("-1");
			}
			bzero(bufferCopy, sizeof(bufferCopy));
			recv(bufferCopy, 0, 0);
			if(strcmp(bufferCopy, "200 PORT command successful")){
				send(server_sd, buffer, sizeof(buffer), 0);
				pid = fork();
				if(pid == 0){
					transfersocketfd = socket(AF_INET, SOCK_STREAM, 0);
					struct sockaddr_in data_server_addr;
					bzero(&data_server_addr,sizeof(data_server_addr));
					data_server_addr.sin_family = AF_INET;
					data_server_addr.sin_port = htons(new_Port);
					data_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
					if(bind(transfersocketfd, (struct sockaddr *)data_server_addr, sizeof(data_server_addr)) < 0){
						perror("bind error:");
						exit(-1);
					}
					status = listen(transfersocketfd, 5);
					if(status < 0){
						perror("listen error:");
						exit(-1);
					}
					new_dedicated_data_sd = accept(transfersocketfd, 0, 0);
					bzero(bufferCopy, sizeof(bufferCopy));
					strcpy(bufferCopy, buffer);
					char* bc = strtok(bufferCopy, space);
					if(strcmp(bc, "RETR") == 0){
						recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
						printf("%s", buffer);
					}
					else if(strcmp(bc, "STOR") == 0){
						bc = strtok(bc, space);
						send(new_dedicated_data_sd, bc, sizeof(bc), 0);
					}
					else if(strcmp(bc, "LIST") == 0){
						recv(new_dedicated_data_sd, buffer, sizeof(buffer), 0);
						printf("%s", buffer);
					}
					
					
					
				}
				else{
					close();	
				}
				
				
				
				
			}
			else{
				perror("not successful:");
				exit(-1);
			}
			
			
			
		}
    }

    
    /*
    char buffer[256];
    char inputBuffer[256];
    fgets(buffer,sizeof(buffer),stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if(send(server_sd,buffer,strlen(buffer),0)<0)
    {
        perror("send");
        exit(-1);
    }
    
    //close(server_sd);
    
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
    //the newsockfd doesn't have anything to do with the port, right? 
    int connfd, newsockfd;
    
    while (1)
    {
        bzero(buffer,sizeof(inputbuffer));
        fgets(buffer,sizeof(inputBuffer),stdin);
        
        newsockfd = accept(server_sd, &((struct sockaddr*)client_addr), sizeof(client_addr));
        if (newsockfd < 0) {
            perror("Failed to accept incoming connection");
            exit(EXIT_FAILURE);
        }
        recv(newsockfd,buffer,sizeof(buffer),0);
        if(inputBuffer.substring(0, 4) == "PORT"){
        	send
        
        
        	//we want to send client_addr.sin_addr.s_addr, client_addr.sin_port to the server. 
        	//
        	
        }
        else if(inputBuffer.substring(0, 4) == "STOR"){
        	//we want to send client_addr.sin_addr.s_addr, client_addr.sin_port to the server, and we need the stor command so the server knows. 
        	//open the text file on our end, store it in a buffer, send it to server_sd via client_addr.sin_port. 
        
        }
        else if(inputBuffer == "LIST"){
        	//we want to send client_addr.sin_addr.s_addr, client_addr.sin_port to the server, also we need to use the list command so the server knows. 
        	//then, we want to receive from anywhere, and display this data. 	
        
        }
        else if(inputBuffer == "!LIST"){
        	//display all of the files in the current directory, c package? 
        }
        else if(inputBuffer.substring(0, 4) == "RETR"){
        	//we want to send client_addr.sin_addr.s_addr, client_addr.sin_port to the server. 
        	//we want to receive from anywhere, and we display this data. 
        
        }
		else if(inputBuffer.substring(0, 3) == "CWD"){
			//we want to send client_addr.sin_addr.s_addr, client_addr.sin_port to the server, and we need to use the CWD command so the server knows. 
			//we then want to receive from anywhere, store it in a buffer, and display the data. 
		}
        else if(inputBuffer.substring(0, 4) == "!CWD"){
        	//switch our current directory, c package? 
        }
        else if(inputBuffer == "PWD"){
        	//c package?
        }
        else if(inputBuffer == "!PWD"){
        	//c package? 
        }
        else if(inputBuffer == "EXIT"){
        	exit(0);
        }
        
        
        
        
        
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
