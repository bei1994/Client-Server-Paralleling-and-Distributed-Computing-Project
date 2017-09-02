#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define MAXLINE 4096 /* biggest number edge client can send to edge server every line */
const int port = 23714; /* edge server port number */
const char IP[] = "127.0.0.1"; /* localhost IP */

int main(int argc,char* argv[]){

    FILE *fp;  
    char line[100][25]; 
    char resul[100][25];
    memset(line,0,sizeof(line));
    memset(resul,0,sizeof(resul));

    if (argc != 2)
    {
        printf("Usage: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Can't open %s\n", argv[1]);
        exit(1);
    }
    int row=0;
    while(fgets(line[row],30,fp)!=NULL)  
    { 
    	if(strcmp(line[row],"\n")==0){
             memset(line[row],0,sizeof(line[row]));
             break;
        }

        row++;
    }  
    
    int socket_num = row;
   
    /* 

    read data from input file 
    
    */


    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd == -1){
        perror("socket: ");
        return -1;
    } /* create a TCP socket */

    struct sockaddr_in server;
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=inet_addr(IP);
    server.sin_port=htons(port);

    /* 
        initialize TCP socket address
    */

    if(connect(client_fd,(struct sockaddr *)&server,sizeof(server))==-1){
        perror("client_fdect: ");
        return -1;
    } /* connect to edge server */

    printf("The client server is up and running.\n");
   
    char rev[50];
    sprintf(rev,"%d",socket_num); /* convert int socket_num into char type */
    

        if(send(client_fd,rev,strlen(rev),0)==-1){
            perror("send: ");
            return -1;
        } /* send line number to edge server */
        for(int i=0;i<socket_num;i++){
           sleep(1);
          
           if(send(client_fd,line[i],strlen(line[i]),0)==-1){
            perror("send: ");
            return -1;

        }
            
        } /* send data line by line to edge server */

        printf("The client has successfully finished sending %d lines to the edge server.\n",socket_num);
        
        for(int i=0;i<socket_num;i++){

         	if(recv(client_fd,resul[i],MAXLINE,0) == -1){
            perror("recv: ");
            return -1;
            }         
          
         } /* receive computation results in order from edge server */
         printf("The client has successfully finished receiving all computation results from the edge server.\n");
         printf("The final computation result are:\n");

         for(int i=0;i<socket_num;i++)
         {
             printf("%s\n",resul[i]); 
         } /* print computation results */
         

    
        close(client_fd); /* close socket */
        while(1){}; /* keep client active */
   
        return 0;
}
