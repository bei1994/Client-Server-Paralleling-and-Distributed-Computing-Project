#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT 21714 /* server_or port number */
const char IP[] = "127.0.0.1"; /* localhost IP */

int main(){ 

    int s=socket(AF_INET,SOCK_DGRAM,0);
        if(s==-1){
        perror("create socket error: ");
        return -1;
    } /* create a UDP socket */

    struct sockaddr_in serv,client;
    bzero(&serv,sizeof(serv));
    serv.sin_family=AF_INET;   
    serv.sin_addr.s_addr=inet_addr(IP);
    serv.sin_port=htons(PORT);  

    /* 
        initialize UDP socket address
    */

    if(bind(s,(struct sockaddr*)&serv,sizeof(serv))==-1){
        perror("bind error: ");
        return -1;
    } /* bind local address to socket */
    printf("The Server AND is up and running using UDP on port %d.\n", PORT);
    printf("The Server AND start receiving lines from the edge server for AND computation. The computation results are:\n");
    char buff[100][25];
    char resul[100][25];
    memset(buff,0,sizeof(buff));
    memset(resul,0,sizeof(resul));
    int row=0;
    while(1){
        
        socklen_t addr_len=sizeof(client);
        int ret=recvfrom(s,buff[row],105,0,(struct sockaddr *)&client,&addr_len); 
        if(ret==-1){
            perror("recvfrom error: ");
            break;
        } /* receive data from edge */
        if(strcmp(buff[row],"end")==0){         
            break;
        } /* if receive 'end' from edge, quit process */
           
        char first[10];
        char second[10];
        char ope[5];
        memset(first,0,sizeof(first));
        memset(second,0,sizeof(second));
        memset(ope,0,sizeof(ope));
        sscanf(buff[row],"%[a-z]",ope);
        int fir,sec;       
        for(int j=0;j<ret-1;j++){
            if(buff[row][j]==','){
                fir=j; 
                break;              
            }          
            }
           
        for(int k=fir+1;k<ret-1;k++){
            if(buff[row][k]==','){
                sec=k; 
                break;              
            }
        }
         
        for(int j=fir+1;j<sec;j++){
            
            first[j-fir-1]=buff[row][j];
            
        }
        for(int j=sec+1;j<ret-1;j++){
            
            second[j-sec-1]=buff[row][j];      

        }

        /* 
           get first and second operand
        */

        int len1=strlen(first);
        int len2=strlen(second);
        char temp[25];
        memset(temp,0,sizeof(temp));   
        if(len1==len2){
            int f,s;           
            for(int i=0;i<len1;i++){

                f=first[i]-'0';
                s=second[i]-'0';               
                if(f==0&&s==0){
                    temp[i]='0';
                }
                else{
                    temp[i]='1';
                }
                
            }

            for(int i=0;i<len1;i++){
                resul[row][i]=temp[i];
            }
        }

        if(len1>len2){
            int f,s; 
            int dura=len1-len2;          
            for(int i=0;i<dura;i++){              
                temp[i]=first[i];              
            }  
            for(int i=dura;i<len1;i++){              
                f=first[i]-'0';
                s=second[i-dura]-'0';               
                if(f==0&&s==0){
                    temp[i]='0';
                }
                else{
                    temp[i]='1';
                }           
            }

            int start=0;
            while(start<len1){
                if(temp[start]=='0'){
                    start++;
                }
                else{
                    break;
                }
           }
          
           for(int i=start;i<len1;i++){
               resul[row][i-start]=temp[i];
           }
 
        }

        if(len1<len2){
            int f,s; 
            int dura=len2-len1;          
            for(int i=0;i<dura;i++){              
                temp[i]=second[i];              
            }  
            for(int i=dura;i<len2;i++){              
                f=first[i-dura]-'0';
                s=second[i]-'0';               
                if(f==0&&s==0){
                    temp[i]='0';
                }
                else{
                    temp[i]='1';
                }           
            }
           
            int start=0;
            while(start<len1){
                if(temp[start]=='0'){
                    start++;
                }
                else{
                    break;
                }
           }
          
           for(int i=start;i<len2;i++){
               resul[row][i-start]=temp[i];
           }
 
        }

        /* 
           compute 'or' operation result
        */

        printf("%s %s %s %s %s\n",first,ope,second,"=",resul[row]);
    
        if(sendto(s,resul[row],ret,0,(struct sockaddr *)&client,addr_len)==-1){
            perror("sendto error: ");
            break;
        } /* send computation result to edge */

        row++;
      
    }

    printf("The Server OR has successfully received %d lines from the edge server and finished all AND computations\n",row);
    printf("The Server OR has successfully finished sending all computation results to the edge server.\n");
    close(s); /* close UDP socket */
    while(1){}; /* keep server_or active */
    return 0;

}