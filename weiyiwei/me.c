#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "10.15.3.142"
#define SERVER_PORT 8600
#define MSG_STR "Hello, Unix Network Program World!"

int main(int argc, char **argv)
{
	int conn_fd = -1;
	int rv = -1;
	char buf[1024];
	struct sockaddr_in serv_addr;
	printf("socket\n");
	
	
	conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(conn_fd < 0)
	{
		printf("create socket failure: %s\n", strerror(errno));
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	inet_aton( SERVER_IP, &serv_addr.sin_addr );
	
	printf("connect\n");
	
	
	if( connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("connect to server [%s:%d] failure: %s\n", SERVER_IP,SERVER_PORT, strerror(errno));
		return 0;
	}
	printf("write\n");


	if( write(conn_fd, MSG_STR, strlen(MSG_STR)) < 0 )
	{
		printf("Write data to server [%s:%d] failure: %s\n", SERVER_IP,SERVER_PORT, strerror(errno));
		goto cleanup;
	}

	memset(buf, 0, sizeof(buf));
	printf("read\n");


	rv = read(conn_fd, buf, sizeof(buf));
	if(rv < 0)
	{
		printf("Read data from server failure: %s\n", strerror(errno));
		goto cleanup;
	}
	else if(0==rv)  
	{
	      	printf("Client connect to server get disconnected\n");
	      	goto cleanup;
	}
	printf("Read %d bytes data from server: '%s'\n", rv, buf);


	cleanup:
	close(conn_fd);
}
