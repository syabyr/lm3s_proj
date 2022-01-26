#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<time.h>
#include <stdlib.h>
#define LISTENPORT 9999
#define BACKLOG 30			//队列数
#define MAXDATASIZE 409600
#define DEBUG 1
void process_cli(int connectfd,sockaddr_in client);
void *start_routine(void *arg);



struct ARG {
	 int connfd;
	 sockaddr_in client;
	 };

int main(void)
{
	int listenfd,connectfd;	//socket descriptor
	pthread_t thread;	   //id of thread
	ARG *arg;		//pass this var to the thread
	struct sockaddr_in server; //server's address info
	struct sockaddr_in client; //client's address info
	int sin_size;



#ifdef DEBUG
	printf("socket....\n");
#endif
	//create tcp socket
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("creating socket failed.\n");
		exit(1);
	}
	int opt=SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	bzero(&server,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(LISTENPORT);
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	printf("bind....\n");
	if(bind(listenfd,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1)
	{
		perror("bind error.\n");
		exit(1);
	}
	printf("listen....\n");
	if(listen(listenfd,BACKLOG) == -1)
	{
		perror("listen() error\n");
		exit(1);
	}
	sin_size=sizeof(struct sockaddr_in);
	//主循环,等待连接
	while(1)
	{
		//accept() using main thread
//		printf("accepting....\n");
		if((connectfd = accept(listenfd,(struct sockaddr *)&client,(socklen_t*)&sin_size)) == -1)
		{
			printf("accept() error\n");
		}
		arg = new ARG;
		arg->connfd = connectfd;
		memcpy((void *)&arg->client, &client, sizeof(client));
		//invoke start_routine to handle this thread

//		printf("thread_creating....\n");

		if(pthread_create(&thread, NULL, start_routine, (void*)arg))
		{
			perror("pthread_create() error");
			exit(1);
		}	    
	}
	close(listenfd);	
}


//invoked by pthread_create
void *start_routine(void *arg)
{
	ARG *info;
	info=(ARG *)arg;
	//handle client's requirement
	process_cli(info->connfd,info->client);
	delete arg;
	pthread_exit(NULL);
}

//handle the request of the client
void process_cli(int connectfd, sockaddr_in client)
{

	int num;
	//char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cli_name[MAXDATASIZE];
	char requestline[MAXDATASIZE],filepath[MAXDATASIZE],cmd[MAXDATASIZE],extname[MAXDATASIZE];
	int c;
	FILE *fp;
	FILE *cfp;
	fp=fdopen(connectfd,"r");
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep);
	memset(requestline,0,sizeof requestline);
#ifdef DEBUG
	printf("Connected from:%s:%d at %s",inet_ntoa(client.sin_addr),client.sin_port,asctime(localtime(&timep)));
#endif

	unsigned long maxsize=409600;
	while (fgets((requestline+strlen(requestline)),maxsize,fp))		//从socket中接收字符
	{
		maxsize=409600-strlen(requestline);
		printf("maxsize:%d\n",maxsize);
	}

#ifdef DEBUG
	printf("The command is:%s",requestline);	//输出接收的字符
#endif
	int i;
			for(i=0;i<strlen(requestline);i++)
			{
				printf("%x ",requestline[i]);
			}
			printf("\n");
	if(requestline[0]=='a')
	{
		printf("It`s a echo command.\n");
		char* a;
		a="A";
		send(connectfd,a,strlen(a),0);
	}
	else if(requestline[0]=='b')
	{
		printf("It`s a request command.\n");
		char* b;
		b="B";
		send(connectfd,b,strlen(b),0);
		char del[10];
		char filename[18+6];
		char buffer[409600];
		sscanf(requestline,"%s%s",del,filename);
		printf("User name:%s\n",filename);
		int len;
		len=strlen(filename);
		filename[len]='_';
		sprintf(filename+len+1,"%d0%d%d%d%d%d.txt", (1900+p->tm_year),( 1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
		printf("File name:%s\n",filename);
		FILE *fp;
		len+=strlen(del);
		if((fp=fopen(filename,"a"))>=0)
		{
			int i;
			for(i=len+3;requestline[i]!=0;i++)
			{
				fprintf(fp,"%c",(unsigned char)requestline[i]);
				printf("%x ",(unsigned char)requestline[i]);
			}
			printf("\n");
			fclose(fp);
		}
	}
	else
	{
		printf("It`s a common command.\n");
		char* c;
		c="ccccccccccc";
		send(connectfd,c,strlen(c),0);
	}

#ifdef DEBUG
	printf(":::::::::::::::::::::::::::end:::::::::::::::::::::::::::::\n\n");    
#endif

	close(connectfd);
}


