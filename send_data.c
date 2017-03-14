#include   <stdio.h> 
#include   <stdlib.h> 
#include   <string.h> 
#include   <sys/types.h> 
#include   <sys/socket.h> 
#include   <errno.h> 
#include   <unistd.h> 
#include   <netinet/in.h> 
#include   <limits.h> 
#include   <netdb.h> 
#include   <arpa/inet.h> 
#include   <ctype.h>
#include	"malloc.h"
#include   <fcntl.h>

//server

//#define SERVER_IP "182.92.10.18"
#define SERVER_IP "127.0.0.1"
#define PORT 3800
#define MAX_BUFFER_SIZE (1024 * 100)

#define FILENAME "snap_0.jpg"


struct data {
	unsigned int data_type;
	unsigned int data_lenth;
	char data_stream[MAX_BUFFER_SIZE];
};

int get_filesize(char* filename)  
{  
    FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  
    fseek(fp,0L,SEEK_END);  
    int size=ftell(fp);  
    fclose(fp);  
      
    return size;  
} 

int tcpclient_creat(const char* ip, int port)
{
	int socket_fd, flags;
	struct sockaddr_in server_addr;     
	
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(port);  
	inet_pton(AF_INET, ip, &server_addr.sin_addr);
    //server_addr.sin_addr = inet_addr(ip);  
  
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
        return -1;  
    }  
	/* Setting socket to nonblock */
  	flags = fcntl(socket_fd, F_GETFL, 0);
 	fcntl(socket_fd, flags|O_NONBLOCK);
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {  
        return -1;  
    }  
    return socket_fd; 
}

static int send_data(int socket, char *buff, int size) {  
    int sent=0, tmpres=0;  
    while(sent < size) {  
        tmpres = send(socket,buff+sent,size-sent,0);  
        if(tmpres == -1){  
            return -1;  
        }  
		printf("send buffer lenth = %d\n", tmpres);
        sent += tmpres;  
    }  
    return sent;  
} 

static int recv_data(int socket, char *buff, int lenth){  
    int recvnum = 0;  
    recvnum = recv(socket, buff, lenth, 0);  
    return recvnum;  
} 

int main()
{
	int socked_fd,  file_size, ret;
	struct data data_send;
	double longitude = 13.45678;
	double latitude = 34.987623;
	FILE *file_fp;
	char *filebuff, *recvbuff;
	file_size = get_filesize(FILENAME);
	filebuff = (char*)malloc(file_size * sizeof(char));
	recvbuff = (char*)malloc(1024 * sizeof(char));
	
	file_fp = fopen(FILENAME,"r");
	ret = fread((void*)filebuff, sizeof(char), file_size, file_fp);
	if (ret != file_size) {
		printf("read file failed!\n");
		return -1;
	}
	socked_fd = tcpclient_creat(SERVER_IP, PORT);
	
	//send longitude
	data_send.data_type = 0xEAD1;
	data_send.data_lenth = sizeof(double);
	memcpy((void*)data_send.data_stream, (void*)&longitude, sizeof(double));
	send_data(socked_fd, (char*)&data_send, 8 + sizeof(double));
	recv_data(socked_fd, recvbuff, 1024);
	printf("recv_info: %s\n", recvbuff);
	
	//usleep(1);
	
	//send latitude
	data_send.data_type = 0xEAD2;
	data_send.data_lenth = sizeof(double);
	memcpy((void*)data_send.data_stream, (void*)&latitude, sizeof(double));
	send_data(socked_fd, (char*)&data_send, 8 + sizeof(double));
	recv_data(socked_fd, recvbuff, 1024);
	printf("recv_info: %s\n", recvbuff);
	
	//usleep(1);

	
	//send pic
	data_send.data_type = 0xEAD3;
	data_send.data_lenth = file_size;
	memcpy((void*)data_send.data_stream, (void*)filebuff, file_size);
	send_data(socked_fd, (char*)&data_send, 8 + file_size);
	recv_data(socked_fd, recvbuff, 1024);
	printf("recv_info: %s\n", recvbuff);
	
	free(filebuff);
	free(recvbuff);
	close(socked_fd);
	fclose(file_fp);
	return 0;
	
}
