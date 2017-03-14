#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h>

#define portnumber 3800

static int send_data(int socket, char *buff, int size) {  
    int sent=0, tmpres=0;  
    while(sent < size) {  
        tmpres = send(socket,buff+sent,size-sent,0);  
        if(tmpres == -1){  
			printf("send data failed\n");
            return -1;  
        }     
        sent += tmpres;  
    }  
    return sent;  
} 

int main() 
{ 
    int sockfd,new_fd; 
	unsigned int lenth, type;
    struct sockaddr_in server_addr; 
    struct sockaddr_in client_addr; 
    int sin_size; 
    int nbytes;
	double longitude, latitude;
    char buffer[1024];
	char *send_buff = "recv finished";
 
    /* 服务器端开始建立sockfd */ 
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
    { 
        fprintf(stderr,"Socket error:%s\n\a",strerror(errno)); 
        exit(1); 
    }

    /* 服务器端填充 sockaddr结构 */ 
    bzero(&server_addr,sizeof(struct sockaddr_in)); // 初始化,置0
    server_addr.sin_family=AF_INET;                 // Internet
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);  // (将本机器上的long数据转化为网络上的long数据)和任何主机通信  //INADDR_ANY 表示可以接收任意IP地址的数据，即绑定到所有的IP
    server_addr.sin_port=htons(portnumber);         // (将本机器上的short数据转化为网络上的short数据)端口号
    /* 捆绑sockfd描述符到IP地址 */ 
    if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
    { 
        fprintf(stderr,"Bind error:%s\n\a",strerror(errno)); 
        exit(1); 
    }

    /* 设置允许连接的最大客户端数 */ 
    if(listen(sockfd,5)==-1) 
    { 
        fprintf(stderr,"Listen error:%s\n\a",strerror(errno)); 
        exit(1); 
    }
	/* 服务器阻塞,直到客户程序建立连接 */ 
    sin_size=sizeof(struct sockaddr_in); 
    if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1) 
    { 
        fprintf(stderr,"Accept error:%s\n\a",strerror(errno)); 
         exit(1); 
    } 
    fprintf(stderr,"Server get connection from %s\n\n",inet_ntoa(client_addr.sin_addr)); // 将网络地址转换成.字符串 

    while(1) 
    { 
        memset(buffer, 0, 1024);
        if((nbytes=recv(new_fd,buffer,4,0))==-1) 
        { 
            fprintf(stderr,"Read Error:%s\n",strerror(errno)); 
            exit(1); 
        }
		printf("nbytes = %d\n", nbytes);
        type = *(unsigned int*)buffer;
		printf("type = 0X%X\n", type);
		
		if((nbytes=recv(new_fd,buffer,4,0))==-1) 
        { 
            fprintf(stderr,"Read Error:%s\n",strerror(errno)); 
            exit(1); 
        } 
		lenth = *(unsigned int*)(buffer);
		printf("lenth = %d\n", lenth);
		
		if((nbytes=recv(new_fd,buffer,lenth,0))==-1) 
        { 
            fprintf(stderr,"Read Error:%s\n",strerror(errno)); 
            exit(1); 
        } 
		if (type == 0xEAD1) {
			longitude = *(double*)(buffer);
			printf("longitude = %f\n\n", longitude);
		} else if (type == 0xEAD2) {
			latitude = *(double*)(buffer);
			printf("latitude = %f\n\n", latitude);
		}
		sleep(3);
		send_data(new_fd, send_buff, strlen(send_buff));
    }
    /* 结束通讯 */ 
    close(sockfd); 
    exit(0); 
}