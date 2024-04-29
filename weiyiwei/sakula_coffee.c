/*select()下面的break会推出到哪里呢*/
/*rv是干啥的捏*/
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<sys/select.h>
#include<sys/time.h>
#include<errno.h>
#include<libgen.h>

/*测量数组大小*/
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
static inline void menu(char *progname);
static inline void msleep(unsigned long ms);
int socket_init(char *listen_ip,int listen_port);

int main(int argc,char **argv)
{	
	int opt;
	int serv_port;
	char *progname=NULL;
	int daemon_run;
	int rv;
	int on;
	int listenfd;
	int fds_array[1024];
	int i,j;
	fd_set rdset;
	int maxfd;
	int connectfd;
	char buf[1024];
	/*menu的一部分，主要获取serv_port*/
	struct option long_options[]=
	{
		{"daemon",no_argument,NULL,'d'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	progname=basename(argv[0]);
	while((opt=getopt_long(argc,argv,"dp:h",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case'd':
			daemon_run=0;
			break;
			case'p':
			serv_port=atoi(optarg);
			break;
			case'h':
			menu(progname);
			return EXIT_SUCCESS;
			defult:
			break;
		}
	}
	if(!serv_port)
	{
		menu(progname);
		return -1;
	}
	/*socket_init()   把serv_port传给socket_init()*/
	printf("socket_init\n");



	if((listenfd=socket_init(NULL,serv_port))<0)
	{
		printf("socket_init()need helppp!");
		return -2;
	}
	printf("%s is listening on port:%d",argv[0],serv_port);
	/*demon()启动！  可以让select()进入后台*/
	printf("demon_run\n");
	
	
	
	if(daemon_run)
	{
		daemon(0,0);
	}
	/*重置fds数组   select()需要遍历*/
	for(i=0;i<=ARRAY_SIZE(fds_array);i++)
	{
		fds_array[i]=-1;
	}
	fds_array[0]=listenfd;
	
	/*进入select()循环当中吧!*/
	printf("循环开始\n");
	
	
	
	for( ; ; )
	{
		/*更新fd_set*/
		FD_ZERO(&rdset);
		for(i=0;i<ARRAY_SIZE(fds_array);i++)
		{
			if(fds_array[i]<0)
			{
				continue;
			}
			maxfd=maxfd>fds_array[i]?maxfd:fds_array[i];
			FD_SET(fds_array[i],&rdset);
		}
		/*select()启动  我猜break会跳到这里来*/
		printf("select\n");



		rv=select(maxfd+1,&rdset,NULL,NULL,NULL);
		if(rv<0)
		{
			printf("select() broked:%s\n",strerror(errno));
			break;
		}
		else if(rv==0)
		{
			printf("select() timeout\n");
			continue;
		}


		/*1/2  未连接的端口*/
		if(FD_ISSET(listenfd,&rdset))
		{	
			printf("未连接的端口\n");
			printf("accept\n");
			if((connectfd=accept(listenfd,(struct sockaddr *)NULL,NULL))<0)
			{
				printf("accept() need helppp:%s",strerror(errno));
				continue;
			}
			for(i=0;i<=ARRAY_SIZE(fds_array);i++)
			{
				if(fds_array[i]<0)
				{
					fds_array[i]=connectfd;
					break;
				}
			}
			
		}
		/*1/2  已连接的端口*/
		else
		{	
			printf("已经连接的端口分支\n");



			for(i=0;i<ARRAY_SIZE(fds_array);i++)
			{
				if(fds_array[i]<0 || !FD_ISSET(fds_array[i],&rdset))
				{
					//sleep(1000);
					continue;
				}
				printf("read\n");
				if((rv=read(fds_array[i],buf,sizeof(buf)))<0)
				{
					printf("read() need helppp!:socket[%d]",fds_array[i]);
					close(fds_array[i]);
					fds_array[i]=-1;
				}
				else
				{
					printf("socket%d got %d bytes deta!\n",fds_array[i],rv);
					for(j=0;j<rv;j++)
					{
						buf[j]=toupper(buf[j]);
					}
					printf("write\n");
					if((write(fds_array[i],buf,rv))<0)
					{
						printf("wirte() need helppp:%s",strerror(errno));
						close(fds_array[i]);
						fds_array[i]=-1;
					}
				
				}


			}	
		}

	}
	CleanUp:
        close(listenfd);
	return 0;

}






static inline void menu(char *progname)
{
	printf(" HELLO!MENU is here\n");
	printf("example:%s -d -p 8600\n",progname);
	return;	
}

static inline void msleep(unsigned long ms)
{
	struct timeval tv;
	tv.tv_sec=ms/1000;
	tv.tv_usec=(ms%1000)*1000;
	select(0,NULL,NULL,NULL,&tv);
}

int socket_init(char *listen_ip,int listen_port)
{	int rv;
	int listenfd;
	int on;
	struct sockaddr_in servaddr;
	/*socket()*/
	printf("socket\n");


	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("socket()is saying HELPPP!:%s",strerror(errno));
		rv=-1;
	}
	/*setsockopt()*/

	printf("setsocketopt\n");
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(listen_port);
	/*listen_ip选择*/
	if(!listen_ip)
	{
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	else
	{
		if((inet_pton(AF_INET,listen_ip,&servaddr.sin_addr))<=0)
		{
			printf("inet_pton()is saying HELPPP!\n");
			rv=-2;
			goto cleanup;
		}
	}
	/*bind()*/
	printf("bind\n");


	if((bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))<0)
	{
		printf("bind()is saying HELPPP!:%s\n",strerror(errno));
		rv=-3;
		goto cleanup;
	}
	/*listen()*/
	printf("listen\n");

	if((listen(listenfd,13))<0)
	{
		printf("listen()is saying HELPPP!%s\n",strerror(errno));
		rv=-4;
		goto cleanup;
	}

	cleanup:
		if(rv<0)
		{
			close(listenfd);
		}
		else
	        {
			rv=listenfd;
			return rv;
		}
}
	






