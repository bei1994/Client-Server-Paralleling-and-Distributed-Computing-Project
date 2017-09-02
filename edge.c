#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define MAXLINE 4096 /* biggest number edge server can receive from client every line */
#define UDP_PORT_OR 21714 /* server_or port number */
#define UDP_PORT_AND 22714 /* server_and port number */
#define TCP_PORT 23714 /* edge server TCP port number */
#define UDP_PORT 24714 /* edge server UDP port number */
const int MAX = 10; /* biggest number of connected clients */
const char IP[] = "127.0.0.1"; /* localhost IP */


void sigchld_handler(int s)
{
    
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
} 
/* 
  process children signal 
*/


int main(){

    struct sigaction sa;

    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == -1){
        perror("socket: ");
        return -1;
    } /* create a TCP socket */

    struct sockaddr_in server;
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=htonl(INADDR_ANY); 
    server.sin_port=htons(TCP_PORT);

    /* 
        initialize TCP socket address
    */

    if(bind(server_fd,(struct sockaddr *)&server,sizeof(server))==-1){
        perror("bind: ");
        return -1;
    } /* bind local address to socket */
    if(listen(server_fd,MAX) == -1){
        perror("listen: ");
        return -1;
    } /* server listening if client send request */

    sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;
    }
    /* 
      reap all dead process
    */

    printf("The edge server is up and running.\n");

    struct sockaddr_in client;
    socklen_t client_len=sizeof(client);
    int conn = accept(server_fd, (struct sockaddr *)&client, &client_len); 
    if(conn == -1){
        perror("accept: ");
        return -1;
    } /* accept connection request from client */
    char buff[100];
    char line[100][25];
    memset(buff,0,sizeof(buff));
    memset(line,0,sizeof(line));
    int socket_num=0;
        
        if(recv(conn,buff,MAXLINE,0) == -1){
            perror("recv: ");
            return -1;
        } /* receive line number from client */
        int i=0;
        while(buff[i]!='\0'){
         socket_num=10*socket_num+(buff[i]-'0');
         i++;
         }
         /* 
           convert char into int 
         */

        int counter_and=0,counter_or=0;
        
        for(int i=0;i<socket_num;i++){
            
           if(recv(conn,line[i],MAXLINE,0) == -1){
            perror("recv: ");
            return -1;
            }          

        }  /* receive data line by line from client */     
         printf("The edge server has received %d lines from the client using TCP over port %d.\n",socket_num,client.sin_port);
          
          char resul[100][25];
          memset(resul,0,sizeof(resul));

          int s=socket(AF_INET,SOCK_DGRAM,0);
                if(s==-1){
                   perror("create socket error: ");
                   return -1;
                } /* create a UDP socket */

        struct sockaddr_in serv_and;
        bzero(&serv_and,sizeof(serv_and));
        serv_and.sin_family=AF_INET;   
        serv_and.sin_addr.s_addr = inet_addr(IP);   
        serv_and.sin_port=htons(UDP_PORT_AND);

        /* 
           initialize UDP server_and socket address
        */

        struct sockaddr_in serv_or;
        bzero(&serv_or,sizeof(serv_or));
        serv_or.sin_family=AF_INET;   
        serv_or.sin_addr.s_addr = inet_addr(IP);    
        serv_or.sin_port=htons(UDP_PORT_OR);   

        /* 
           initialize UDP server_or socket address
        */   
        
         for(int i=0;i<socket_num;i++){
            
            char ope[10];
            memset(ope,0,sizeof(ope));
            sscanf(line[i],"%[a-z]",ope); /* decide if operation is 'and' or 'or' */

            if(strcmp(ope,"and")==0){            
            int ret;
            int sed;       
            while(1){
                
                 if((sed=sendto(s,line[i],strlen(line[i]),0,(struct sockaddr *)&serv_and,sizeof(serv_and)))==-1){
                    perror("sendto error: ");   
                    break;                
                 } /* send data to server_and */
                 socklen_t addr_len=sizeof(serv_and);
                 if((ret=recvfrom(s,resul[i],105,0,(struct sockaddr *)&serv_and,&addr_len))==-1){
                  perror("recvform error: ");
                  break;                  
                 } /* receive computation result from server_and */ 
                 if(sed>0&&ret>0){
                     
                     break;
                 }       
               } 
                 counter_and++; /* count line number that server_and recieve */
                               
                if(send(conn,resul[i],strlen(resul[i]),0)==-1){
                    perror("send: ");
                    break;
               } /* send computation result to client */

             } 

            else{         
       
                 int ret;
                 int sed;
        
            while(1){
                 
                 if((sed=sendto(s,line[i],strlen(line[i]),0,(struct sockaddr *)&serv_or,sizeof(serv_or)))==-1){
                    perror("sendto error: ");   
                    break;                
                 } /* send data to server_or */      

                 socklen_t addr_len=sizeof(serv_or);
                 if((ret=recvfrom(s,resul[i],105,0,(struct sockaddr *)&serv_or,&addr_len))==-1){
                  perror("recvform error: ");
                  break;                 
                 } /* receive computation result from server_or */ 
                 if(sed>0&&ret>0){                   
                     break;
                 }        
             } 
                 counter_or++; /* count line number that server_and recieve */
                 
                 if(send(conn,resul[i],strlen(resul[i]),0)==-1){
                       perror("send: ");
                       break;
               } /* send computation result to client */
            }
        }

        if(sendto(s,"end",strlen("end"),0,(struct sockaddr *)&serv_and,sizeof(serv_and))==-1) {
                    perror("sendto error: ");   
                    return -1;                
                } 

        if(sendto(s,"end",strlen("end"),0,(struct sockaddr *)&serv_or,sizeof(serv_or))==-1) {
                    perror("sendto error: ");   
                    return -1;                
                }    
        /* 
           when all data is sent, send 'end' sign to server_or and server-and
        */
 
         printf("The edge has successfully sent %d lines to Backend-Server OR.\n",counter_or);
         printf("The edge has successfully sent %d lines to Backend-Server AND.\n",counter_and);
         printf("The edge server start receiving the computation results from Backend-Server OR and Backend-Server AND using UDP over port %d .\n",UDP_PORT);
         printf("The computation results are:\n");
         for(int i=0;i<socket_num;i++){

         char first[10];
         char second[10];
         char ope[10];
         memset(first,0,sizeof(first));
         memset(second,0,sizeof(second));
         memset(ope,0,sizeof(ope));
         sscanf(line[i],"%[a-z]",ope); /* get operation */
         int fir,sec;       
         for(int j=0;j<strlen(line[i])-1;j++){
            if(line[i][j]==','){
                fir=j; 
                break;              
            }          
            }         
        for(int j=fir+1;j<strlen(line[i])-1;j++){
            if(line[i][j]==','){
                sec=j; 
                break;              
            }
        }              
        for(int j=fir+1;j<sec;j++){
            
            first[j-fir-1]=line[i][j];       
        }
        for(int j=sec+1;j<strlen(line[i])-1;j++){
            
            second[j-sec-1]=line[i][j];      
        } 
        /* 
           get first and second operand
        */

        printf("%s %s %s %s %s\n", first,ope,second,"=",resul[i]);

        }

        printf("The edge server has successfully finished receiving all computation results from the Backend-Server OR and Backend-Server AND.\n");
        printf("The edge server has successfully finished sending all computation results to the client.\n");
        close(conn); /* close TCP socket */
        close(server_fd); /* close child socket */
        close(s); /* close UDP socket */

        while(1){}; /* keep edge active */
        return 0;
        
}
