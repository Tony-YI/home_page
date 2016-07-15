#define _XOPEN_SOURCE 700  /* Needed for crypt(), strptime()
                            * Remember to define this before the include's. */
#define _BSD_SOURCE     /* Needed for timegm() */
#include <arpa/inet.h>     /* inet_ntoa, ntohs */
#include <netdb.h>         /* gai_strerror, getaddrinfo */
#include <netinet/in.h>    /* inet_ntoa, struct sockaddr_in */
#include <stdio.h>         /* perror */
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>    /* gai_strerror, getaddrinfo, inet_ntoa */
#include <sys/types.h>     /* gai_strerror, getaddrinfo */
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#define HTTP_BUF_SIZE 1024 * 64
#define CONTENT_BUF_SIZE 1024 * 64
#define MAX_HOST_NUM 10
#define MAX_HOST_LENGTH 36
#define MAX_URL_LENGTH 1024

pid_t gettid(void)
{
	return syscall(__NR_gettid);
}
void *Work(void *Arg);
struct Thre_Arg
{
	int cli_sd;
	//struct sockaddr_in client_addr;
}__attribute__ ((packed));
const char HTTP_RESPONSE_200[] = "HTTP/1.1 200 OK\r\nServer: Apache\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\n";
const char HTTP_RESPONSE_304[] = "HTTP/1.1 304 Not Modified\r\nServer:Apache\r\nConnection: Keep-Alive\r\n";
struct Host_Info
{
	char HOST[MAX_HOST_LENGTH];
	int ser_sock;
}__attribute__ ((packed));
struct host_info_all
{
	struct Host_Info *p;
	int *size;
}__attribute__ ((packed));
int readAvailable(int sockfd)
{
        int ret = 0;
        struct timeval tv = {0};
        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        ret = select(sockfd+1, &readfds, NULL , NULL, &tv);
        if (ret == -1) {
                perror("select");
                exit(0);
        }
        if (FD_ISSET(sockfd, &readfds))
        {
                return 1;
        } 
        return 0;
}

unsigned short PORT = 10000;
unsigned short MILESTONE = 1;
struct Thre_Arg *con_Info;
void Milestone1and2(int cli_sd);
void Milestone3(int cli_sd);
void tolow(char *pointer,int size);
void FindHeader(const char *subheader,const char *header_name,char **pro_start,int *size,char ** value);
int Get_Host_sd(const char *buffer,struct Host_Info * host_info,int * size);
int GetHostNamePort(const char *pro,char *Host,char *Port);
int Set_PROXY_Port(int PORT_NUM);//THIS FUNCTION RETURN A SOCKET NUM BOUND TO PORT PORT_NUM
int Accept_Client_Connect(int sd);//THIS FUNCTION RETURN A NEW SOCKET NUM FROM THE LISNTENING SOCKET sd 
pthread_attr_t Worker_ATTR();//THIS FUNCTION RETURN THE DETACHED PTHREAD ATTRIBUTE
int Safe_read(int socket_num,void *pointer,int size);//THIS FUNCTION ENSURES A READ WITH EXACTLY size NUMBER OF BYTES
int Safe_write(int socket_num,void *pointer,int size);//THIS FUNCTION ENSURES A WRITE WITH EXACTLY size NUMBER OF BYTES
int Read_HTTP_Header(int cli_sd,char *header,int *size);
int Get_Content_Length(const char *buffer);//THIS FUNCTION RETURN THE CONTENT LENGTH OF A HTTP RESPONSE
int URL(const char *buffer,char *target);//THIS FUNCTION GET THE URL FROM buffer AND STORE IT INTO target
void CryptFilename(const char *url,char *filename);//THIS FUNCTION CYRPT THE url AND STORE THE FILENAME IN filename
void CleanUp(void *pointer);//THIS IS THE FUNCTION TO CLEAN UP THE THREAD LEFTCOVER
int CacheAndSend(int cli_sd,int target_sd,FILE *fd,int leftover,const char *path);//THIS FUNCTION CACHE AND SEND THE CONTENT FROM THE SERVER WITH STATUS CODE 200
void NoCacheSendAll(int cli_sd,int ser_sd,int leftover);//THIS FUNCTION SEND WHATEVER IS RETURNED FROM THE SERVER
void SendStatus304(int cli_sd);//THIS FUNCTION SEND A HTTP RESPONSE WITH STATUS CODE 304
int SendStatus200(int cli_sd,const char *path,const char *type,struct stat file_info);
void BranchOnResponse(int cli_sd,int target_sd,char *path,const char *type,char *HTTP_res,struct stat local_file_info);
void CloseFD(void * fd)
{
	fclose((FILE *) fd);
	printf("THREAD_ID:%d, closing fd\n",gettid());
	return ;
}

void unlock(void *mutex)
{
	if(pthread_mutex_unlock((pthread_mutex_t *)mutex) != 0)
	{perror("pthread_mutex_unlock()");}
	printf("THREAD_ID:%d pthread_mutex_unlock()\n",gettid());
	return ;
}
void CloseAllSd(void *host_info)
{
	printf("THREAD_ID:%d CloseAllSd()\n",gettid());
	struct host_info_all *pointer =  (struct host_info_all *)host_info;
	struct Host_Info *local = pointer->p;
	//printf("THREAD_ID:%d local:%p,host_info->p:%p\n",gettid(),local,((struct host_info_all *) host_info)->p);
	int size = *(((struct host_info_all *) host_info)->size);
	for(int count = 0;count < size;count++)
	{
		//printf("THREAD_ID%d local[%d] host_name:%s close(%d) size:%d\n",gettid(),count,local[count].HOST,local[count].ser_sock,size);
		//if(local[count].ser_sock != 0)
		close(local[count].ser_sock);
		//else
		//system("pause");
		
	}
	printf("THREAD_ID:%d CloseAllSd() return\n",gettid());
	return ;
}

int URLCheck(const char *url,char *type);
void GetDate(char *buffer);
int packet = 0;
pthread_mutex_t mutex;
pthread_mutex_t whole;
/*
 * 
 * replace the 'target' with 'content' of the 'header' in the 'buffer'  
 * 
 */
int Modify_Buffer(char *buffer,const char *header,const char *target,const char *content,int used_size);

int ser_sd = 0;
int cli_sd = 0;
int main(int argc,char **argv)
{
	
	if(argc < 3)
	{printf("Usage:myproxy [PORT] [MILESTONE]\nDEFAULT SETTING PORT = 10000,MILESTONE = 1\n");}
	else
	{PORT=atoi(argv[1]);MILESTONE=atoi(argv[2]);}
	if(MILESTONE > 3)
	{printf("Invalid MILESTONE\n");exit(0);}
	ser_sd = Set_PROXY_Port(PORT);
	pthread_attr_t detached = Worker_ATTR();
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&whole,NULL);
	while(1)
	{
		cli_sd = Accept_Client_Connect(ser_sd);
		printf("NEW TCP CONNECTION\n");
		struct Thre_Arg *Arg = (struct Thre_Arg *)malloc(sizeof(struct Thre_Arg));
		Arg->cli_sd = cli_sd;
		//(*Work)(Arg);
		pthread_t tid1;
		if(pthread_create(&tid1,&detached,Work,(void *) Arg) < 0)
		{perror("pthread_create()");exit(0);}
		pthread_detach(tid1);
	}
	return 0;
}
void CloseSD(void *p)
{
	printf("THREAD_ID:%d Closing CLient sd\n",gettid());
	close(*((int*)p));
	printf("THREAD_ID:%d Closed CLient sd\n",gettid());
	return ;
}
void *Work(void *Arg)//THREAD FUNCTION
{
	struct Thre_Arg *temp = (struct Thre_Arg *)Arg;
	int Loc_cli_sd = temp->cli_sd;
	pthread_cleanup_push(CleanUp,temp);
	pthread_cleanup_push(CloseSD,&Loc_cli_sd);
	if(MILESTONE == 1 || MILESTONE == 2)
		Milestone1and2(Loc_cli_sd);
	else if(MILESTONE == 3)
		Milestone3(Loc_cli_sd);
	
	pthread_cleanup_pop(1);//close the cli_sd
	pthread_cleanup_pop(1);//CLEANUP ARGUMENT
	pthread_exit(NULL);
	return NULL; 
}
int Set_PROXY_Port(int PORT_NUM)
{
	int sd = socket(AF_INET,SOCK_STREAM,0);//EATABILISHING A NEW SOCKET
		
		//SET THE SOCKET AS REUSABLE PORT
		long val = 1;
		if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(long)) == -1)
		{perror("setsocket()");exit(1);}
		//-----------------------------
		//SETTING THE SERVER ADDRESS, PORT
		struct sockaddr_in server_addr;
		memset(&server_addr,0,sizeof(server_addr));
		server_addr.sin_family=AF_INET;
		server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
		server_addr.sin_port=htons(PORT);
		//------------------------------
		if(bind(sd,(struct sockaddr *)&server_addr,sizeof(server_addr)) == -1)
		{	perror("bind():");exit(1);}
		
		if( listen(sd,1024) == -1 )
		{
			perror("listen()");
			exit(1);
		}
		return sd;
}
void Milestone1and2(int cli_sd)
{
	pid_t tid = gettid();
	char *HTTP_req = (char *)malloc(HTTP_BUF_SIZE);//HTTP REQUEST HEADER BUFFER;
	memset(HTTP_req,0,HTTP_BUF_SIZE);
	char *HTTP_res = (char *)malloc(HTTP_BUF_SIZE);//HTTP RESPONSE HEADER BUFFER;
	memset(HTTP_res,0,HTTP_BUF_SIZE);
	pthread_cleanup_push(CleanUp,HTTP_req);
	pthread_cleanup_push(CleanUp,HTTP_res);
	int host_num = 0;
	struct Host_Info *host_info = (struct Host_Info *)malloc(sizeof(struct Host_Info) * MAX_HOST_NUM);
	memset(host_info,0,sizeof(struct Host_Info) * MAX_HOST_NUM);
	pthread_cleanup_push(CleanUp,host_info);
	struct host_info_all all;
	printf("THREAD_ID:%d &all:%p\n",gettid(),&all);
	all.p = host_info;
	all.size = &host_num;
	pthread_cleanup_push(CloseAllSd,&all);
	while(1)
	{
		int req_head_size = 0,res_head_size = 0,target_sd,used_req_size = 0;
		printf("THREAD_ID:%d Fetching New Request\n",tid);
		used_req_size = Read_HTTP_Header(cli_sd,HTTP_req,&req_head_size);
		if(used_req_size > req_head_size)
		{
			fprintf(stderr,"THREAD_ID:%d NOT ONLY ONE HTTP REQUEST IS READ\n---------------------------------\n",gettid());
			//pthread_exit(NULL);
			break;
		}
		if(req_head_size == 0)
		{
			printf("THREAD_ID%d READ NO MORE HEADER\n",tid);
			//pthread_exit(NULL);
			break;
		}
		if(used_req_size < 0)
			break;//pthread_exit(NULL);
		
		if(MILESTONE == 1)
		{
			if((req_head_size = Modify_Buffer(HTTP_req,"Connection","keep-alive","close",0)) < 0)
			{printf("Property not found in Buffer\n");}
			if((req_head_size = Modify_Buffer(HTTP_req,"Proxy-Connection","keep-alive","close",0)) < 0)
			{printf("Property Not found in Buffer\n");}
		}
		target_sd = Get_Host_sd(HTTP_req,host_info,&host_num);
		
		if(Safe_write(target_sd,HTTP_req,req_head_size) < 0)
		break;
		printf("THREAD_ID:%d req_head_size%d\n",tid,req_head_size);
		write(1,HTTP_req,req_head_size);
		int used_HTTP_res  = 0;
		if((used_HTTP_res = Read_HTTP_Header(target_sd,HTTP_res,&res_head_size)) == -1)
		{printf("return value 0 of Read_HTTP_Header\n");}
		
		if(MILESTONE == 1)
		used_HTTP_res+=(Modify_Buffer(HTTP_res,NULL,"\r\n\r\n","\r\nProxy-Connection: close\r\n\r\n",used_HTTP_res) - res_head_size);
		
		res_head_size = strstr(HTTP_res,"\r\n\r\n") - HTTP_res + 4;
		
		if(Safe_write(cli_sd,HTTP_res,used_HTTP_res) < 0)
		break;
		write(1,HTTP_res,res_head_size);
		//printf("ADDRESS OF HTTP_res is %p\n",HTTP_res);
		int content_len = Get_Content_Length(HTTP_res);
		printf("THREAD_ID:%d Content-Length:[%d bytes] HTTP_HEADER_LENGTH: [%dbytes] Return %d bytes\n",tid,content_len,res_head_size,used_HTTP_res);
		int leftover = content_len + res_head_size - used_HTTP_res;
		if(leftover < 0)
		{
				write(1,HTTP_res,HTTP_BUF_SIZE);
				fprintf(stderr,"\n--------------------------\n");
				fprintf(stderr,"leftover negative\ncontent-length:%d http-length:%d received-length:%d\n",content_len,res_head_size,used_HTTP_res);
		}
		NoCacheSendAll(cli_sd,target_sd,leftover);
		if(MILESTONE == 1)
		{
			printf("THREAD_ID:%d MILESTONE 1 CLOSING ALL CONNECTIONS\n",gettid());
			//close(target_sd);
			//close(cli_sd);
			//pthread_exit(NULL);
			break;
		}
	}
	printf("THREAD_ID:%d,pthread_exit()\n",tid);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
	return ;
}

/*
 * THIS FUNCTION STORED THE URL in char *target
 */
int URL(const char *buffer,char *target)
{
	char *p1 = NULL;
	char *p2 = NULL;
	if(strstr(buffer,"GET") == NULL)
	{
		fprintf(0,"THIS IS NOT A GET METHOD\n");
		return -1;
	}
	if((p1 = strstr(buffer," ")) == NULL)
	{fprintf(stderr,"strstr()");return -1;}
	p1++;
	if((p2 = strstr(p1 + 1," ")) == NULL)
	{fprintf(stderr,"strstr()");return -1;}
	if(p2 - p1 > MAX_URL_LENGTH)
	{fprintf(stderr,"URL too long\n");write(1,p1,p2 - p1);return -1;}
	memcpy(target,p1,p2 - p1);
	target[p2 - p1] = '\0';
	return -1;
}
void CryptFilename(const char *url,char *filename)
{
	char *result = crypt(url, "$1$00$");
	//printf("Entire result from crypt(): %s\n", result);  /* strlen(result) = 28 */
	result += 6;  /* The first 6 bytes are useless, strlen(result) = 22 */
  for (int i=0; i<22; i++) {
    if (result[i] == '/') { result[i] = '_'; }
  }
  //printf("You should create your cache file in: cache/%s\n", result);
  strcpy(filename,result);
  return ;
}
/*
 *THIS FUCNTION IS THE CLEAN_UP ROUNTINE FOR THPREAD
 */
void CleanUp(void *pointer)
{
	printf("THREAD_ID:%d Going to clean\n",gettid());
	printf("THREAD_ID:%d CLEANNING UP POINTER:%p\n",gettid(),pointer);
	free(pointer);
	printf("THREAD_ID:%d CLEANNING DONE\n",gettid());
	return ;
}
/*
 * 
 * const char * buffer is the buffer where the HTTP header is stored
 * sturct Host_Info * host_info is the pointer points to the host information struct
 * int *size is the size of the current host_info array
 * return value is the socket num where the HTTP header should be forwarded to
 * 
 * 
 */
 
int Get_Host_sd(const char * buffer,struct Host_Info * host_info,int * size)
{
	printf("THREAD_ID:%d Get_Host_sd \n",gettid());
	char *Host_name;
	char *pro_start;
	int Length_of_Host;
	int target_sd = 0;
	FindHeader(buffer,"Host: ",&pro_start,&Length_of_Host,&Host_name);
	printf("THREAD_ID:%d Pushing Host_name %p\n",gettid(),Host_name);
	pthread_cleanup_push(CleanUp,Host_name);
	printf("THREAD_D:%d TOTALLY %d HOST INFO\n",gettid(),*size);
	for(int count = 0;count < *size;count++)
	{
		//printf("%s :: %s\n",host_info[count].HOST,Host_name);
		if(strcmp(Host_name,host_info[count].HOST) == 0)
		{printf("using old connection HOST:%s socket:%d\n",host_info[count].HOST,host_info[count].ser_sock);
		if(readAvailable(host_info[count].ser_sock) == 1)
		{
			perror("THREAD_ID:%d Server SOCKET CLOSED\n");
			printf("THREAD_ID:%d Recreating connection with%s\n",gettid(),host_info[count].HOST);
			close(host_info[count].ser_sock);
			char *Host = (char *)malloc(128);
			memset(Host,0,128);
			printf("THREAD_ID:%d Pushing Host %p\n",gettid(),Host_name);
			pthread_cleanup_push(CleanUp,Host);
			char Port[5];
			memset(Port,0,5);
			GetHostNamePort(Host_name,Host,Port);
			///////////////////////////////////////////////////
			///////////////////////////////////////////////////
			struct addrinfo hints, *results;
			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_INET;        /* IPv4 */
			hints.ai_socktype = SOCK_STREAM;  /* TCP connection */
			hints.ai_flags = 0;
			hints.ai_protocol = 0;
			int ret = getaddrinfo(Host,Port, &hints, &results);

			/* Error-handling */
			if (ret != 0) 
			{
				if (ret == EAI_SYSTEM) 
					perror("getaddrinfo"); 
				else 
				{
					fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
					write(1,buffer,strstr(buffer,"\r\n\r\n") - buffer + 4);
				}
				for(int count = 0;count < *size;count++)
				{close(host_info[count].ser_sock);}
				return -1;
			}
			if((target_sd = socket(AF_INET,SOCK_STREAM,0))== -1)
			{perror("socket() in thread:");pthread_exit(NULL);}
		
			struct sockaddr_in *my_addr = (struct sockaddr_in *) results->ai_addr;
			if(my_addr == NULL)
			{printf("Invalid sockaddr_in equal to NULL\n");pthread_exit(NULL);}
			printf("ADDRESS:%s\n",inet_ntoa(my_addr->sin_addr));
			if(connect(target_sd,(struct sockaddr *) my_addr,sizeof(struct sockaddr_in)) < 0 )
			{perror("connect() in pthread:");pthread_exit(NULL);}
			freeaddrinfo(results);
			/////////////////////////////////////////////////////
			/////////////////////////////////////////////////////
			/////////////////////////////////////////////////////
			host_info[count].ser_sock = target_sd;
			pthread_cleanup_pop(1);
		}
		printf("THREAD_ID:%d Get_Host_sd return(%d) *size:%d\n",gettid(),host_info[count].ser_sock,*size);
		return host_info[count].ser_sock;}
	}
	if(*size < 10)
	{
		printf("THREAD_ID:%d CREATING NEW SOCKET FOR NEW HOST NAME\n",gettid());
		strcpy(host_info[*size].HOST,Host_name);
		//printf("THREAD_ID:%d flag1",gettid());
		char *Host = (char *)malloc(128);
		memset(Host,0,128);
		printf("THREAD_ID:%d Pushing Host %p\n",gettid(),Host);
		pthread_cleanup_push(CleanUp,Host);
		char Port[5];
		memset(Port,0,5);
		GetHostNamePort(Host_name,Host,Port);
		//printf("HOSTNAME:|%s| PORT:%s\n",Host,Port);
		
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		struct addrinfo hints, *results;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;        /* IPv4 */
		hints.ai_socktype = SOCK_STREAM;  /* TCP connection */
		hints.ai_flags = 0;
		hints.ai_protocol = 0;
		int ret = getaddrinfo(Host,Port, &hints, &results);

		/* Error-handling */
		if (ret != 0) 
		{
			if (ret == EAI_SYSTEM) 
				perror("getaddrinfo"); 
			else 
			{
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
				write(1,buffer,strstr(buffer,"\r\n\r\n") - buffer + 4);
			}
			for(int count = 0;count < *size;count++)
			{close(host_info[count].ser_sock);}
			pthread_exit(NULL);
		}
		if((target_sd = socket(AF_INET,SOCK_STREAM,0))== -1)
		{perror("socket() in thread:");pthread_exit(NULL);}
		
		struct sockaddr_in *my_addr = (struct sockaddr_in *) results->ai_addr;
		if(my_addr == NULL)
		{printf("Invalid sockaddr_in equal to NULL\n");pthread_exit(NULL);}
		printf("THREAD_D:%d ADDRESS:%s\n",gettid(),inet_ntoa(my_addr->sin_addr));
		if(connect(target_sd,(struct sockaddr *) my_addr,sizeof(struct sockaddr_in)) < 0 )
		{perror("connect() in pthread:");pthread_exit(NULL);}
		freeaddrinfo(results);
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////
		host_info[*size].ser_sock = target_sd;
		(*size)++;
		pthread_cleanup_pop(1);
	}
	else 
	{
		for(int count = 0;count < *size;count++)
		{close(host_info[count].ser_sock);memset(host_info[count].HOST,0,MAX_HOST_LENGTH);host_info[count].ser_sock = 1;}
		*size = 0;
		strcpy(host_info[*size].HOST,Host_name);
		char *Host = (char *)malloc(128);
		memset(Host,0,128);
		printf("THREAD_ID:%d Pushing Host %p\n",gettid(),Host);
		pthread_cleanup_push(CleanUp,Host);
		char Port[5];
		memset(Port,0,5);
		GetHostNamePort(Host_name,Host,Port);
		printf("THREAD_D:%d HOSTNAME:|%s| PORT:%s\n",gettid(),Host,Port);
		
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		struct addrinfo hints, *results;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;        /* IPv4 */
		hints.ai_socktype = SOCK_STREAM;  /* TCP connection */
		hints.ai_flags = 0;
		hints.ai_protocol = 0;
		int ret = getaddrinfo(Host,Port, &hints, &results);

		/* Error-handling */
		if (ret != 0) 
		{
			if (ret == EAI_SYSTEM) 
				perror("getaddrinfo"); 
			else 
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
			pthread_exit(NULL);
		}
		if((target_sd = socket(AF_INET,SOCK_STREAM,0))== -1)
		{perror("socket() in thread:");pthread_exit(NULL);}
		
		struct sockaddr_in *my_addr = (struct sockaddr_in *) results->ai_addr;
		if(my_addr == NULL)
		{printf("Invalid sockaddr_in equal to NULL\n");pthread_exit(NULL);}
		printf("ADDRESS:%s\n",inet_ntoa(my_addr->sin_addr));
		if(connect(target_sd,(struct sockaddr *) my_addr,sizeof(struct sockaddr_in)) < 0 )
		{perror("connect() in pthread:");pthread_exit(NULL);}
		freeaddrinfo(results);
		pthread_cleanup_pop(1);
	}
	pthread_cleanup_pop(1);
	//printf("target_sd is %d\n",target_sd);
	printf("THREAD_ID:%d Get_Host_sd return(target_sd %d) size:%d\n",gettid(),target_sd,*size);
	return target_sd;
}

/*
 * 
 * replace the 'target' with 'content' of the 'header' in the 'buffer'  
 * return the header size after modification(if "target" not found, return the origin header size)
 */
int Modify_Buffer(char *buffer,const char *header,const char *target,const char *content,int res_size)
{
	printf("THREAD_ID:%d Modify_Buffer()\n",gettid());
	char *value;
	int size;
	char *pro_start;
	
	if(header == NULL)
	{
		/////CASE FOR APPENDING content///////////  
		///////////////////////////////////////
		char *record;
		int HTTP_size = strstr(buffer,"\r\n\r\n") - buffer + 4;
		if((record = strstr(buffer,target)) == NULL)
		{fprintf(stderr,"End of header not found in Modify_Buffer\n");write(1,buffer,65535);pthread_exit(NULL);}
		memmove(record + strlen(content),record + strlen(target),res_size - HTTP_size);
		memcpy(record,content,strlen(content));
		//write(1,buffer,HTTP_BUF_SIZE);
		return strstr(buffer,"\r\n\r\n") - buffer + 4;
	}
	FindHeader(buffer,header,&pro_start,&size,&value);
	if(pro_start != NULL)
	{
			char *record;
			printf("THREAD_ID:%d Pushing value in Modify_Buffer %p\n",gettid(),value);
			pthread_cleanup_push(CleanUp,value);
			if((record = strstr(value,target)) != NULL)
			{
				if((record = strstr(pro_start,target)) == NULL)
				{printf("WHAT THE FUCK!1\n");pthread_exit(NULL);}

				memmove(record + strlen(content),record + strlen(target),strstr(buffer,"\r\n\r\n") - (record + strlen(target)) + 4);
				strncpy(record,content,strlen(content));

				printf("THREAD_ID:%d Modify_Buffer()\n",gettid());
				return strstr(buffer,"\r\n\r\n") - buffer + 4;
			}
			else
			{return strstr(buffer,"\r\n\r\n") - buffer + 4;}
			pthread_cleanup_pop(1);
	}
	else
	{
	printf("THREAD_ID:%d Modify_Buffer() return\n",gettid());
	return strstr(buffer,"\r\n\r\n") - buffer + 4;
	}
}
int URLCheck(const char *url,char *type)
{
	char *p;
	if((p = strstr(url,".")) == NULL)
	return 0;
	else
	{
		if(strstr(p,"html") != NULL) 
		{strcpy(type,"html");return 1;}
		else if (strstr(p,"jpg") != NULL)
		{strcpy(type,"jpg");return 1;}
		else if(strstr(p,"gif") != NULL)
		{strcpy(type,"gif");return 1;}
		else if(strstr(p,"txt") != NULL)
		{strcpy(type,"txt");return 1;}
		else
		return 0;
	}
	return 0;
}
void Milestone3(int cli_sd)
{
	pid_t tid = gettid();
	char *HTTP_req = (char *)malloc(HTTP_BUF_SIZE);//HTTP REQUEST HEADER BUFFER;
	memset(HTTP_req,0,HTTP_BUF_SIZE);
	char *HTTP_res = (char *)malloc(HTTP_BUF_SIZE);//HTTP RESPONSE HEADER BUFFER;
	memset(HTTP_res,0,HTTP_BUF_SIZE);
	
	pthread_cleanup_push(CleanUp,(void*) HTTP_req);
	pthread_cleanup_push(CleanUp,(void*) HTTP_res);
	int host_num = 0;
	struct Host_Info *host_info = (struct Host_Info *)malloc(sizeof(struct Host_Info) * MAX_HOST_NUM);
	memset(host_info,0,sizeof(struct Host_Info) * MAX_HOST_NUM);
	pthread_cleanup_push(CleanUp,host_info);
	struct host_info_all all;
	all.p = host_info;
	all.size = &host_num;
	pthread_cleanup_push(CloseAllSd,&all);
	while(1)
	{
		int IS_FILE = 0,EXIST_FILE = 0,req_head_size = 0,res_head_size = 0,target_sd,used_req_size = 0,used_res_size = 0;
		int NO_CACHE = 0;
		long IMS_INT = 0;
		char url[MAX_URL_LENGTH];
		memset(url,0,MAX_URL_LENGTH);
		char filename[32],path[64] = "cache/";
		memset(filename,0,32);
		used_req_size = Read_HTTP_Header(cli_sd,HTTP_req,&req_head_size);
		/////////////////////////////
		/////////////////////////////
		if(used_req_size > req_head_size)
		{
			fprintf(stderr,"NOT ONLY ONE HTTP REQUEST IS READ\n");
			write(1,HTTP_req,used_req_size);
			pthread_exit(NULL);
		}
		if(req_head_size == 0)
		{
			printf("THREAD_ID:%d READ NO MORE HEADER\n",tid);
			for(int count = 0;count < host_num;count++)
			{printf("THREAD_ID:%d host_info[%d] HOST:%s, socket_num:%d\n",tid,count,host_info[count].HOST,host_info[count].ser_sock);}
			break;
		}
		/////////////////////////////
		/////////////////////////////
		target_sd = Get_Host_sd(HTTP_req,host_info,&host_num);
		/////////////////////RECOGNIZING FILE NAME//////////////
		URL(HTTP_req,url);
		char type[6];
		if((IS_FILE = URLCheck(url,type)))
		{
			CryptFilename(url,filename);
			strcat(path,filename);
			printf("THREAD_ID:%d URL:[%s] filename:[%s] path:[%s]\n",tid,url,filename,path);
		}
		else
			printf("THREAD_ID:%d URL:[%s] INVALID filename\n",tid,url);
		if(IS_FILE)
		{
			//printf("IT'S A FILE\n");
			struct stat local_file_info;
			//struct stat remote_file_info;
			int ret = 0;
			if((ret = stat(path,&local_file_info)) != 0)
			{
				if(errno == ENOENT)
				{EXIST_FILE = 0;fprintf(stderr,"THREAD_ID:%d, FILE DOESN'T EXIST\n",gettid());}
				else
				{perror("stat()");fprintf(stderr,"THREAD_ID:%d,PATH:%s\n",tid,path);pthread_exit(NULL);}
			}
			else
			EXIST_FILE = 1;
			
			
			if(EXIST_FILE)
			{	
				printf("THE FILE DOES EXIST\n");
				////////////////////CASE FOR ALREADY CACHED FILE////////////////////////
				////////////////////////////////////////////////////////////////////////
				////////////////////////////////////////////////////////////////////////
				struct tm local_file_tm;
				struct tm remote_file_tm;
				/////////TRANSFER THE LOCAL_TIME FROM STAT local_file_info TO local_file_tm///////////
				//////////////////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////////////////
				gmtime_r(&local_file_info.st_mtime,&local_file_tm);
				char LOCAL_IMS_CHAR[50] = {0};//THIS IS A C-STRING STROING LOCAL_IMS_TIME
				strftime(LOCAL_IMS_CHAR, sizeof(LOCAL_IMS_CHAR),"%a, %d %b %Y %H:%M:%S GMT", &local_file_tm);
				//printf("THREAD_ID:%d,LOCAL_IMS:%s\n",gettid(),LOCAL_IMS_CHAR);
				/* convert string (GMT) back to local integer time */
				////CONVERT THE LOCAL TIME INTO LONG TYPE////
				long LOCAL_IMS_INT = timegm(&local_file_tm);
				
				char *value;
				char *pro_start;
				int size;
				////////////////FINDING 'NO-CACHE'//////////////////////
				////////////////////////////////////////////////////////
				FindHeader(HTTP_req,"Cache-Control: ",&pro_start,&size,&value);
				if(pro_start == NULL)
				{
					NO_CACHE = 0;
				}
				else if(strstr(value,"no-cache") == NULL)
				{
					NO_CACHE = 0;
				}
				else
					NO_CACHE = 1;
				free(value);
				///////////////////////////////////////////////////////
				///////////////////////////////////////////////////////
				
				
				if(NO_CACHE)
				{
					//CASE FOR EXIST NO_CACHE
					FindHeader(HTTP_req,"If-Modified-Since: ",&pro_start,&size,&value);
					pthread_cleanup_push(CleanUp,value);
					if(pro_start == NULL)
					{
						//CASE FOR NO IMS AND NO-CACHE
						char *IMS_Header = (char *)malloc(100);
						memset(IMS_Header,0,100);
						pthread_cleanup_push(CleanUp,IMS_Header);
						strcpy(IMS_Header,"\r\nIf-Modifed-Since: ");
						strcat(IMS_Header,LOCAL_IMS_CHAR);
						strcat(IMS_Header,"\r\n\r\n");
						used_req_size = Modify_Buffer(HTTP_req,NULL,"\r\n\r\n",IMS_Header,used_req_size);
						pthread_cleanup_pop(1);
						
						if(Safe_write(target_sd,HTTP_req,used_req_size) < 0)
						break;
						BranchOnResponse(cli_sd,target_sd,path,type,HTTP_res,local_file_info);
					}
					else
					{	
						//CASE FOR IMS AND NO-CACHE
						char remote_time_char[50] = {0};
						memcpy(remote_time_char,pro_start,size);
						remote_time_char[size] = '\0';
						//printf("THREAD_ID:%d %s\n",gettid(),remote_time_char);
						strptime(remote_time_char, "%a, %d %b %Y %H:%M:%S GMT", &remote_file_tm);
						IMS_INT = timegm(&remote_file_tm);
						if(LOCAL_IMS_INT > IMS_INT)
						{
							printf("THREAD_ID:%d, IMS&no-cache REPLACING OLD IMS\n",gettid());
							req_head_size = Modify_Buffer(HTTP_req,"If-Modified-Since: ",remote_time_char,LOCAL_IMS_CHAR,req_head_size);
							write(1,HTTP_req,req_head_size);
						}
						else
							printf("THREAD_ID:%d IMS&no-cache SIMPLY FORWARD\n",gettid());
							if(Safe_write(target_sd,HTTP_req,used_req_size) < 0)
							break;
						BranchOnResponse(cli_sd,target_sd,path,type,HTTP_res,local_file_info);
						
					}
					pthread_cleanup_pop(1);
					
				}
				else
				{
					//CASE FOR NO NO_CACHE
					FindHeader(HTTP_req,"If-Modified-Since: ",&pro_start,&size,&value);
					pthread_cleanup_push(CleanUp,value);
					if(pro_start == NULL)
					{
						printf("THREAD_ID:%d NO IMS & NO no-cache SendStatus200()\n",tid);
						//NO IMS, NO NO_CACHE STATUS CODE 200
						//////////////////////////////////////////
						/////////////CRITICAL SECTION/////////////
						//////////////////////////////////////////
						pthread_mutex_lock(&mutex);
						pthread_cleanup_push(unlock,&mutex);
						SendStatus200(cli_sd,path,type,local_file_info);
						pthread_cleanup_pop(1);
						//////////////////////////////////////////
						//////////////////////////////////////////
						//////////////////////////////////////////
					}
					else
					{
						//////////////////////////////////////////////
						////////EXIST IMS NO NO_CACHE/////////////////
						//////////////////////////////////////////////
						char remote_time_char[50] = {0};
						memcpy(remote_time_char,pro_start,size);
						remote_time_char[size] = '\0';
						//printf("THREAD_ID:%d %s\n",gettid(),remote_time_char);
						
						//CHANGE THE REMOTE IMS TIME FROM C-STRING TO struct tm in remote_file_tm
						strptime(remote_time_char, "%a, %d %b %Y %H:%M:%S GMT", &remote_file_tm);
						IMS_INT = timegm(&remote_file_tm);
						//printf("THREAD_ID:IMS_INT%ld LOCAL_IMS_INT:%ld\n",IMS_INT,LOCAL_IMS_INT);
						if(LOCAL_IMS_INT >= IMS_INT)
						{
							printf("THREAD_ID:%d IMS & NO no-cache FILE UP TO DATE, RETURN 304\n",gettid());
							//THE CACHED FILE IS UP TO DATE
							SendStatus304(cli_sd);
						}
						else
						{
							printf("THREAD_ID:%d IMS & NO no-cache, FILE TOO OLD, SendStatus200\n",gettid());
							///////////////////////////////////////////
							//////////CRITICAL SETION//////////////////
							///////////////////////////////////////////
							pthread_mutex_lock(&mutex);
							pthread_cleanup_push(unlock,&mutex);
							SendStatus200(cli_sd,path,type,local_file_info);
							pthread_cleanup_pop(1);
							///////////////////////////////////////////
							///////////////////////////////////////////
							///////////////////////////////////////////
						}
						
					}
					pthread_cleanup_pop(1);
				}
			}
			else
			{
				//CASE FOR FILE DON'T EXIST
				if(Safe_write(target_sd,HTTP_req,req_head_size) < 0)
				break;
				used_res_size = Read_HTTP_Header(target_sd,HTTP_res,&res_head_size);
				if(Safe_write(cli_sd,HTTP_res,used_res_size) < 0)
				break;
				
				if(strstr(HTTP_res,"HTTP/1.1 200 OK") != NULL)
				{
					//int ND = 0;
					/////////////////////////////////////
					//////////CRITICAL SECTION///////////
					/////////////////////////////////////
					pthread_mutex_lock(&mutex);
					
					FILE *fd;
					if((fd = fopen(path,"w")) == NULL)
					{perror("fopen()");fprintf(stderr,"THREAD_ID:%d",tid);pthread_mutex_unlock(&mutex);return;}
					pthread_cleanup_push(unlock,&mutex);
					fwrite(HTTP_res + res_head_size,1,used_res_size - res_head_size,fd);
					int content_len = Get_Content_Length(HTTP_res);
					int leftover = content_len + res_head_size - used_res_size;
					CacheAndSend(cli_sd,target_sd,fd,leftover,path);
					pthread_cleanup_pop(1);
					
					//pthread_mutex_unlock(&mutex);
					/////////////////////////////////////
					/////////////////////////////////////
					/////////////////////////////////////
				}
				else
				{
					printf("THREAD_ID:%d FILE DOESN'T EXIST, RETURN VALUE != 200\n",gettid());
					NoCacheSendAll(cli_sd,target_sd,-1);
				}
				
			}
		}
		else
		{
			//CASE FOR NOT A VALID FILENAME(NOT HTML,JPG,GIF,TXT)
			if(Safe_write(target_sd,HTTP_req,req_head_size) < 0)
			break;
			used_res_size = Read_HTTP_Header(target_sd,HTTP_res,&res_head_size);
			
				if(Safe_write(cli_sd,HTTP_res,used_res_size) < 0)
				break;
				int content_len = Get_Content_Length(HTTP_res);
				int leftover = content_len + res_head_size - used_res_size;
				NoCacheSendAll(cli_sd,target_sd,leftover);
		}
		memset(HTTP_res,0,HTTP_BUF_SIZE);
		memset(HTTP_req,0,HTTP_BUF_SIZE);
	}
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	close(cli_sd);
	return ;
}
void BranchOnResponse(int cli_sd,int target_sd,char *path,const char *type,char *HTTP_res,struct stat local_file_info)
{
	printf("THREAD_ID:%d BranchOnResponse()\n",gettid());
	int used_res_size = 0,res_head_size = 0;
	
	if(( used_res_size = Read_HTTP_Header(target_sd,HTTP_res,&res_head_size)) < 0)
			{pthread_exit(NULL);}
	else
	write(1,HTTP_res,used_res_size);
	
						if(strstr(HTTP_res,"HTTP/1.1 200 OK\r\n") != NULL)
						{
							printf("THREAD_ID:%d IMS&no-cache, return 200 from server, CacheAndSend\n",gettid());
							if(Safe_write(cli_sd,HTTP_res,used_res_size) < 0)
							return ;
							
							int Content_Len = Get_Content_Length(HTTP_res);
							int leftover = Content_Len + res_head_size - used_res_size;
							//printf("THREAD_ID:%d already sent %d bytes, lacking %d bytes\n",gettid(),used_res_size,leftover);
							///////////CRITICAL SECTION//////////////
							/////////////////////////////////////////
							/////////////////////////////////////////
							pthread_mutex_lock(&mutex);
							FILE *fd;
							if((fd = fopen(path,"w")) == NULL)
							{perror("fopen()");return;}
							fwrite(HTTP_res + res_head_size,1,used_res_size - res_head_size,fd);
							//pthread_cleanup_push(DelNotCompleFile,path);
							pthread_cleanup_push(unlock,&mutex);

							CacheAndSend(cli_sd,target_sd,fd,leftover,path);
							pthread_cleanup_pop(1);

							/////////////////////////////////////////
							/////////////////////////////////////////
							/////////////////////////////////////////
						}
						else if(strstr(HTTP_res,"HTTP/1.1 304 Not Modified\r\n") != NULL)
						{
							printf("THREAD_ID:%d IMS&no-cache, return 304 from server, SendStatus200()\n",gettid());
							///////////CRITICAL SECTION//////////////
							/////////////////////////////////////////
							/////////////////////////////////////////
							pthread_mutex_lock(&mutex);
							pthread_cleanup_push(unlock,&mutex);
							SendStatus200(cli_sd,path,type,local_file_info);
							pthread_cleanup_pop(1);
							/////////////////////////////////////////
							/////////////////////////////////////////
							/////////////////////////////////////////
						}
						else
						{
							printf("THREAD_ID:%d IMS&no-cache, return else, NoCacheSendAll(-1)\n",gettid());
							NoCacheSendAll(cli_sd,ser_sd,-1);
						}
		printf("THREAD_ID:%d BranchOnResponse() return \n",gettid());
		return ;
}
void GetDate(char *buffer)
{
	strcat(buffer,"Date: ");
	char local_time_char[50] = {0};
	time_t local_time = time(NULL);
	struct tm local_time_tm;
	gmtime_r(&local_time, &local_time_tm);
	strftime(local_time_char,sizeof(local_time_char),"%a, %d %b %Y %H:%M:%S GMT", &local_time_tm);
	strcat(buffer,local_time_char);
	strcat(buffer,"\r\n");
	return ;
}
void SendStatus304(int cli_sd)
{
	printf("THREAD_ID:%d SendStatus304(cli%d)\n",gettid(),cli_sd);
	char *HTTP_RESPONSE = (char *) malloc(HTTP_BUF_SIZE);
	memset(HTTP_RESPONSE,0,HTTP_BUF_SIZE);
	strcat(HTTP_RESPONSE,HTTP_RESPONSE_304);
	char *Date = (char *)malloc(50);
	memset(Date,0,50);
	GetDate(Date);
	strcat(HTTP_RESPONSE,Date);
	strcat(HTTP_RESPONSE,"\r\n");
	pthread_cleanup_push(CleanUp,HTTP_RESPONSE);
	pthread_cleanup_push(CleanUp,Date);
	Safe_write(cli_sd,(void *)HTTP_RESPONSE,strlen(HTTP_RESPONSE));
	//printf("THREAD_ID:%d HTTP_RESPONSE:%s",gettid(),HTTP_RESPONSE);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	printf("THREAD_ID:%d SendStatus304() return\n",gettid());
	return ;
}
int SendStatus200(int cli_sd,const char *path,const char *type,struct stat file_info)
{
	printf("THREAD_ID:%d SendStatus200(cli %d)\n",gettid(),cli_sd);
	char *HTTP_RESPONSE = (char *)malloc(HTTP_BUF_SIZE);
	memset(HTTP_RESPONSE,0,HTTP_BUF_SIZE);
	pthread_cleanup_push(CleanUp,HTTP_RESPONSE);
	strcat(HTTP_RESPONSE,HTTP_RESPONSE_200);
	struct tm file_time;
	gmtime_r(&file_info.st_mtime, &file_time);
	char Last_Modified[100] = "Last-Modified: ";
	char last_modified_time[50] = {0};
	strftime(last_modified_time, sizeof(last_modified_time),"%a, %d %b %Y %H:%M:%S GMT", &file_time);
	strcat(Last_Modified,last_modified_time);
	strcat(Last_Modified,"\r\n");
	strcat(HTTP_RESPONSE,Last_Modified);
	char Date_Header[100] = {0};
	GetDate(Date_Header);
	strcat(HTTP_RESPONSE,Date_Header);

	FILE *fd;
	if((fd = fopen(path,"r")) == NULL)
	{perror("fopen()");fprintf(stderr,"cannot open [%s]\n",path);return -1;}
	pthread_cleanup_push(CloseFD,fd);
	long file_size = 0;
	fseek(fd,0,SEEK_END);
	file_size = ftell(fd);
	fseek(fd,0,SEEK_SET);
	char Content_Type[50] = "Content-Type: ";
	if(strcmp(type,"jpg") == 0)
		strcat(Content_Type,"image/jpeg\r\n");
	else if(strcmp(type,"gif") == 0)	
		strcat(Content_Type,"image/gif\r\n");
	else if(strcmp(type,"txt") == 0)
		strcat(Content_Type,"text/plain\r\nContent-Encoding: gzip\r\n");
	else if(strcmp(type,"html") == 0)
		strcat(Content_Type,"text/html\r\nContent-Encoding: gzip\r\n");
	char Content_Length[70] = "Content-Length: ";
	char length[50] = {0};
	sprintf(length,"%ld",file_size);
	strcat(Content_Length,length);
	strcat(Content_Length,"\r\n\r\n");

	strcat(HTTP_RESPONSE,Content_Type);
	strcat(HTTP_RESPONSE,Content_Length);

	Safe_write(cli_sd,HTTP_RESPONSE,strstr(HTTP_RESPONSE,"\r\n\r\n") - HTTP_RESPONSE + 4);
	long leftover = file_size;
	printf("THREAD_ID:%d leftover:%ld\n",gettid(),leftover);
	void *buffer = malloc(HTTP_BUF_SIZE);
	memset(buffer,0,HTTP_BUF_SIZE);
	pthread_cleanup_push(CleanUp,buffer);
	int rv = 0;
	while(leftover > 0)
	{
		if(leftover < HTTP_BUF_SIZE)
		{	
			
			rv = fread(buffer,1,leftover,fd);
			Safe_write(cli_sd,buffer,rv);
			printf("THREAD_ID:%d Sending %d bytes",gettid(),rv);
			leftover-=rv;
		}
		else
		{
			rv = fread(buffer,1,HTTP_BUF_SIZE,fd);
			Safe_write(cli_sd,buffer,rv);
			printf("THREAD_ID:%d Sending %d bytes",gettid(),rv);
			leftover-=rv;
		}
	}
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	printf("THREAD_ID:%d SendStatus200() return\n",gettid());
	return 1;
}
int CacheAndSend(int cli_sd,int target_sd,FILE* fd,int leftover,const char *path)
{
	printf("THREAD_ID:%d CacheAndSend(cli:%d ser%d)\n",gettid(),cli_sd,target_sd);
	if(leftover > 0)
	{
		void *content_buffer = malloc(CONTENT_BUF_SIZE + 1);
		memset(content_buffer,0,CONTENT_BUF_SIZE + 1);
		pthread_cleanup_push(CleanUp,content_buffer);
		while(leftover > 0)
		{
			if(leftover > CONTENT_BUF_SIZE)
			{
				if(Safe_read(target_sd,content_buffer,CONTENT_BUF_SIZE) < 0)
				break;//READ FROM SERVER
				Safe_write(cli_sd,content_buffer,CONTENT_BUF_SIZE);//WRITE TO CLIENT
				fwrite(content_buffer,1,CONTENT_BUF_SIZE,fd);
				printf("THREAD_ID:%d Returning %d bytes\n",gettid(),CONTENT_BUF_SIZE);
				leftover-=CONTENT_BUF_SIZE;
			}
			else
			{
				if(Safe_read(target_sd,content_buffer,leftover) < 0)
				break;
				Safe_write(cli_sd,content_buffer,leftover);
				fwrite(content_buffer,1,leftover,fd);
				printf("THREAD_ID:%d Returning %d bytes\n",gettid(),leftover);
				leftover-=CONTENT_BUF_SIZE;
			}
		}
		pthread_cleanup_pop(1);
	}
	printf("THREAD_ID:%d CacheAndSend() return\n",gettid());
	if(leftover != 0)
	{printf("THREAD_ID:%d DIDN'T FINSIH CACHING\n",gettid());fclose(fd);if(remove(path) !=0)exit(1); return 0;}
	else
	{fclose(fd);}
	return (leftover == 0);
}
/*
 * THIS FUNCTION WILL SENT WHATEVER IS READ TO THE BROWSER.
 */
void NoCacheSendAll(int cli_sd,int target_sd,int leftover)
{
	void *content_buffer = malloc(CONTENT_BUF_SIZE);
	memset(content_buffer,0,CONTENT_BUF_SIZE);
	pthread_cleanup_push(CleanUp,content_buffer);
	printf("THREAD_ID:%d NoCacheSendAll(cli:%d ser:%d) leftover:%d content_buffer:%p\n",gettid(),cli_sd,target_sd,leftover,content_buffer);
	if(leftover > 0)
	{
		while(leftover > 0)
		{
			if(leftover > CONTENT_BUF_SIZE)
			{
				if(Safe_read(target_sd,content_buffer,CONTENT_BUF_SIZE) < 0)
				break;//READ FROM SERVER
				Safe_write(cli_sd,content_buffer,CONTENT_BUF_SIZE);//WRITE TO CLIENT
				printf("THREAD_ID:%d Returning %d bytes\n",gettid(),CONTENT_BUF_SIZE);
				leftover-=CONTENT_BUF_SIZE;
			}
			else
			{
				if(Safe_read(target_sd,content_buffer,leftover) < 0)
				break;
				Safe_write(cli_sd,content_buffer,leftover);
				printf("THREAD_ID:%d Returning %d bytes\n",gettid(),leftover);
				leftover-=CONTENT_BUF_SIZE;
			}
		}
	}
	else if(leftover < 0)
	{
		printf("THREAD_ID:%d CRAZY MODE\n",gettid());
		
		printf("THREAD_ID:%d READING FROM SERVER\n",gettid());
		int rv = read(target_sd,content_buffer,CONTENT_BUF_SIZE);//READ FROM SERVER
		if(rv > 0)
		Safe_write(cli_sd,content_buffer,rv);//WRITE TO CLIENT
		while(rv > 0)
		{
				rv = read(target_sd,content_buffer,CONTENT_BUF_SIZE);//READ FROM SERVER
				if(rv > 0)
				{
					Safe_write(cli_sd,content_buffer,rv);//WRITE TO CLIENT
					printf("THREAD_ID:%d Returning %d bytes Mode:EASY TRANSERFERING\n",gettid(),rv);
				}
		}
	}
	pthread_cleanup_pop(1);
	printf("THREAD_ID:%d NoCacheSendAll(cli:%d ser:%d) return\n",gettid(),cli_sd,target_sd);
	return ;
}
int Get_Content_Length(const char *buffer)
{
	printf("THREAD_ID:%d Get_Content_Length()\n",gettid());
	char *pro_start;
	char *value;
	int size = 0;
	int length = 0;
	FindHeader(buffer,"Content-Length:",&pro_start,&size,&value);
	if(value == NULL || pro_start == NULL)
	{
		printf("THREAD_ID:%d Content-Length doesn't exist.returning 0\n",gettid());
		//printf("returing!!!\n");
		return 0;
	}
	length = atoi(value);
	
	free(value);
	printf("THREAD_ID:%d Get_Content_Length return(%d)\n",gettid(),length);
	return length;
}
int Accept_Client_Connect(int sd)
{
	socklen_t addlen = sizeof(struct sockaddr_in);
	int con;
	struct sockaddr_in client_addr;
	memset(&client_addr,0,sizeof(client_addr));
	if((con = accept(sd,(struct sockaddr *) &client_addr,&addlen)) == -1)
	{perror("accept():");exit(1);}
	return con;
}
/*
 * THIS FUNCTION RETURN THE THREAD ATTRIBUTE AS DETACHED ATTRIBUTE
 */
pthread_attr_t Worker_ATTR()
{
	pthread_attr_t worker_attr;//WORKER THREAD ATTRIBUTE
	int rv = pthread_attr_init(&worker_attr);
	if(rv != 0)
	{perror("pthread_attr_init(&worker_attr)");exit(0);}
	rv = pthread_attr_setdetachstate(&worker_attr,PTHREAD_CREATE_DETACHED);
	if(rv != 0)
	{perror("pthread_attr_setdetachstate()");exit(0);}
	return worker_attr;
}
void tolow(char *pointer,int size)
{
	for(int count = 0;count < size;count++)
	if(pointer[count] >= 'A' && pointer[count] <= 'Z')
		pointer[count] = pointer[count] + 'a' - 'A';
	return ;
}

int Read_HTTP_Header(int cli_sd,char *header,int *size)
{
	printf("THREAD_ID:%d Read_HTTP_Header(%d)\n",gettid(),cli_sd);
	int rv = 0,length = 0;
	if((rv = read(cli_sd,(void *)header,HTTP_BUF_SIZE)) < 0)
	{perror("read() in Read_HTTP_Header()");return -1;}
	if(rv >= 0)
	{
		length += rv;
		while(strstr(header,"\r\n\r\n") == NULL && rv > 0)
		{
			rv = read(cli_sd,(void *) (header + length),HTTP_BUF_SIZE - length);
			
			length += rv;
			if(length > HTTP_BUF_SIZE)
			{printf("Limit exceeded\n");return -1;}
		}
		if(rv == 0)
		{fprintf(stderr,"THREAD_ID:%dREAD_HTTP_HEADER connection broken down\n",gettid());return -1;}
		if(length > HTTP_BUF_SIZE)
		{fprintf(stderr,"THREAD_ID%d READ HTTP HREADER TOO LARGE\n",gettid());}
		*size = strstr(header,"\r\n\r\n") - header + 4;
		printf("THREAD_ID:%d Read_HTTP_Header(%d) return\n",gettid(),cli_sd);
		return length;
	}
	else if(rv < 0)
	{
		fprintf(stderr,"rv < 0 socket id:%d\n",cli_sd);
		perror("read() in READ_HTTP_HEADER\n");
		return -1;
	}
	printf("THREAD_ID:%d Read_HTTP_Header(%d)return \n",gettid(),cli_sd);
	return -1;
}
/*
 * 
 * subheader is the buffer for the HTTP REQUEST
 * header_name is the name of the targeted header
 * pro_start is a pointer pointing at the starting position of the property of the header in the buffer
 * size is a pointer pointing to the length of the property
 * value is a tempory storage of the whole property value of a header inside the heap !!!REMEMBER to freese after usage
 * 
 */
void FindHeader(const char *subheader,const char *header_name,char **pro_start,int *size,char **value)
{
		
		char *sub;//pointer points to the beginning of the position of the header_name
		char *pro_end;//Property end pointer
		if((sub = strstr(subheader,header_name)) == NULL)
		{
			*pro_start = NULL;
			*size = 0;
			*value = NULL;
			return;
		}//sub pointing at the beginning of the header_name
		*pro_start = sub + strlen(header_name);//Pro_start pointing at the begining of the property of a header
		if((pro_end = strstr(sub,"\r\n")) == NULL)//Pro_end pointing at the end of the property of a header
		{fprintf(stderr,"Dn not find '\r\n' in header\n");return ;}
		int pro_length = pro_end - *pro_start + 1;
		*size = pro_length;
		*value = (char *) malloc(*size);
		memset(*value,0,*size);
		memcpy(*value,*pro_start,*size - 1);
		(*value)[*size - 1] = '\0';
		tolow(*value,pro_length - 1);
		//printf("returning value:[%s]\n",*value);
		return ;
}
/*
 * 
 * THIS FUNCTION RETURN STORED THE HOST NAME AND PORT in 'Host' and 'Port'
 * 
 */
int GetHostNamePort(const char *pro,char *Host,char *Port)
{
	char *p;
	if((p = strstr(pro,":")) != NULL)
	{	
		strncpy(Host,pro,p - pro);
		if(p - pro + 1 < 32)
		Host[p - pro + 1] = '\0';
		else
		{fprintf(stderr,"\nERROR:Host name too long\n");return -1;}
		strcpy(Port,p);
	}
	else
	{	
		strcpy(Host,pro);
		Port[0] = '8';Port[1] = '0';Port[2] = '\0';
	}
	return 1;
}
int Safe_read(int socket_num,void *pointer,int size)
{
	int leftover = size;
	int rv = 0;
	while(leftover > 0)
	{
		if(leftover > HTTP_BUF_SIZE)
		{fprintf(stderr,"THREAD_ID:%d the leftover is too large to read",gettid());system("pause");}
		rv = recv(socket_num,(char *)pointer + (size - leftover),leftover,MSG_NOSIGNAL);
		if(rv == -1)
		{fprintf(stderr,"THREAD_ID EXIT:%d socket:%d %s\n",gettid(),socket_num,strerror(errno));return -1;}
		else if(rv == 0)
		{
			fprintf(stderr,"THREAD_ID:%d RECV %d Fail to recv()\n",gettid(),socket_num);
			return -1;
		}
		leftover-=rv;
	}
	return size;
}

int Safe_write(int socket_num,void *pointer,int size)
{
	int leftover = size;
	int rv = 0;
	while(leftover > 0)
	{
		rv = send(socket_num,(char *)pointer + (size - leftover),leftover,MSG_NOSIGNAL);
		
		if(rv == -1)
		{perror("send()");fprintf(stderr,"THREAD_ID EXIT :%d socket:%d\n",gettid(),socket_num);return -1;}
		else if(rv == 0)
		{
			fprintf(stderr,"THREAD_ID:%d Send %d Fail to Send\n",gettid(),socket_num);
			return -1;

		}
		leftover-=rv;
	}
	return size;
}

