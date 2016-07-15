#include <arpa/inet.h>     /* inet_ntoa, ntohs */
#include <netdb.h>         /* gai_strerror, getaddrinfo */
#include <netinet/in.h>    /* inet_ntoa, struct sockaddr_in */
#include <stdio.h>         /* perror */
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>    /* gai_strerror, getaddrinfo, inet_ntoa */
#include <sys/types.h>     /* gai_strerror, getaddrinfo */
#include <errno.h>    /* needed to access errno */
#include <pthread.h>  /* pthread_* */
#include <unistd.h>   /* sleep */
#include <string.h>

#define PROXY_IPADDR "127.0.0.1"
#define HTTP_size_max 16*1024//maximum size of HTTP header is 16kB
#define IP_SIZE	15+1
#define PORT_SIZE 32+1//????????????????????????????????????????????

int in_sd;//socket descripter of client to proxy
int accept_sd;

void init_in_socket(char* argv);//initiallize the proxy socket and bind to port num [argv] for receive and send HTTP message to client

void get_ipaddr_port(char* HTTP_message, char* IPaddr, char* Port);//get the host's IP Address
void init_out_socket(char* IPaddr, char* Port, int* out_sd);//initiallize the proxy socket and bind to port num for receive and send HTTP message to server

int recv_rnrn(char* buff, int size);//check whether a HTTP message is received, 1 is true, 0 is false

void* deal_with_client_1(void* client_sd);//accept client sd
void* deal_with_client_2(void* client_sd);//accept client sd
void* deal_with_client_3(void* client_sd);//accept client sd

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("Usage: ./myproxy [Port] [Milestone]\n");
		exit(0);
	}
	
	int milestone = atoi(argv[2]);
	init_in_socket(argv[1]);//initiallize the socket and port for the proxy
	
	//****************************pthread program********************************//
	while(1)
	{	
		struct sockaddr_in client_addr;//client socket structer
		socklen_t addr_len = sizeof( client_addr );//The length of client address, IPV4 is 32 bits

		//connect to the client using client_sd
		if( (accept_sd = accept(in_sd, (struct sockaddr*) &client_addr, &addr_len)) < 0 )
		{//accept() is blocking function
			printf("accept error: %s (Errno:%d)\n", strerror(errno), errno);
			close(in_sd);
			exit(0);
		}
		
		///////////////accept client successed/////////////////
		pthread_t client_thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		
		int client_sd = accept_sd;
		int ret_val;
		
		if(milestone == 1)
		{
			ret_val = pthread_create( &client_thread, &attr, deal_with_client_1, &client_sd );//dealing with client
		}
		else if(milestone == 2)
		{
			ret_val = pthread_create( &client_thread, &attr, deal_with_client_2, &client_sd );//dealing with client
		}
		else if(milestone == 3)
		{
			ret_val = pthread_create( &client_thread, &attr, deal_with_client_3, &client_sd );//dealing with client
		}
		
		if( ret_val < 0 )
		{
			printf("pthread create error: %s (Errno:%d)\n", strerror(errno), errno);
			close(accept_sd);
			close(in_sd);
			exit(0);
		}
		
		//destory thread source if necessary
		pthread_attr_destroy(&attr);
	}
	//****************************end of pthread program********************************//
	
	close(in_sd);
	
	return 0;
}

void init_in_socket(char* argv)
{
	int PORT = atoi(argv);
	in_sd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in proxy_addr;
	memset( &proxy_addr, 0, sizeof(proxy_addr) );
	proxy_addr.sin_family = AF_INET;
	proxy_addr.sin_addr.s_addr = inet_addr( PROXY_IPADDR );//declear the IP address of the proxy
	proxy_addr.sin_port = htons(PORT);
	
	if( bind(in_sd, (struct sockaddr *) &proxy_addr, sizeof(proxy_addr)) < 0 )
	{
		printf("bind error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	
	if( listen(in_sd,100) < 0 )
	{
		printf("listen error: %s (Errno:%d)\n", strerror(errno),errno);
		exit(0);
	}
}

void get_ipaddr_port(char* HTTP_message, char* IPaddr, char* Port)
{
	///////////////////////////////////////////////////////////////
	strcpy(IPaddr, "137.189.91.192");
	strcpy(Port, "80");
}

void init_out_socket(char* IPaddr, char* Port, int* out_sd)
{
	*out_sd = socket(AF_INET, SOCK_STREAM, 0);//AF_INET stands for IPV4, SOCK_STREAM stands for TCP
	struct sockaddr_in server_addr;//struct of socket
	memset( &server_addr, 0, sizeof(server_addr) );//Filled memory with constant bytes. void *memset(void *s, int c, size_t n);
													//The memset() function fills the first n bytes of the memory
													//area pointed to by s with the constant byte c.
	server_addr.sin_family = AF_INET;//Stands for IPV4
	server_addr.sin_addr.s_addr = inet_addr( IPaddr );//IPaddr the IP address of server
	server_addr.sin_port = htons( atoi(Port) );//Port is the port number of the server. htons() stands for "host to network short"
										//send data to the network without worry about the big-endian or little-endian
	/******************************************************************/
	//no need to bind.
	
	/***********************connect to the server**********************/
	if( connect(*out_sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 )//int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
																				//The connect() system call connects the socket referred to
																				//by the file descriptor sock fd to the address specified by addr.
																				//If  the connection or binding succeeds, 0 is returned.
																				//On error, -1 is returned, and errno is set appropriately.
	{
		printf("connection error: %s (Errno:%d)\n", strerror(errno), errno);
		close(*out_sd);
		exit(0);
	}
	
	//printf("out_socket: %d\n", *out_sd);
	//printf("IPaddr: %s\n", IPaddr);
	//printf("Port: %d\n", atoi(Port));
}

int recv_rnrn(char* buff, int size)
{
	int i = 0; 
	for(; i < size; i++)
	{
		if(buff[i] == '\r' && buff[i+1] == '\n' && buff[i+2] == '\r' && buff[i+3] == '\n')
		{
			return 1;
		}
	}
	
	return 0;
}

void* deal_with_client_1(void* client_sd)
{
	pthread_cleanup_push(NULL, NULL);
	
	int out_sd;//socket descripter of proxy to server
	
	////////////////////////////should be change when not keep-alive/////////////////////////
	char* IPaddr = malloc(IP_SIZE);
	char* Port = malloc(PORT_SIZE);

	
	while(1)
	{
		int ret_val_recv = 0;
		int ret_val_send = 0;
		char* buff_cts = (char*) malloc(HTTP_size_max);//cts means client to server
		char* buff_stc = (char*) malloc(HTTP_size_max);//stc means server to client
		
		get_ipaddr_port( buff_cts, IPaddr, Port );
		init_out_socket(IPaddr, Port, &out_sd);
		
		int already_recv = 0;
		while( !recv_rnrn(buff_cts, already_recv) )
		{
			ret_val_recv = recv( *(int*) client_sd, buff_cts + already_recv, HTTP_size_max, 0);//sizeof(buff) is 4 or 8
			if( ret_val_recv == -1 )
			{
				printf("ERROR recv1\n");
				pthread_exit(0);
			}
			already_recv += ret_val_recv;
		}
		printf("\nalready_recv_client_to_proxy: %d\n", already_recv);
		int i = 0;
		for(; i < already_recv; i++)
		{
			printf("%c", buff_cts[i]);
		}
			
		//get_ipaddr_port( buff, IPaddr, Port );
		//init_out_socket(IPaddr, Port, &out_sd);
		
		//http message from client through proxy to server//
		int already_send = 0;
		while( already_send < already_recv )
		{
			ret_val_send = send( out_sd, buff_cts + already_send, already_recv - already_send, 0 );
			if( ret_val_send == -1 )
			{
				printf("ERROR send1\n");
				pthread_exit(0);
			}
			already_send += ret_val_send;
		}
		printf("\nalready_send_proxy_to_server: %d\n", already_send);
		for(i = 0; i < already_send; i++)
		{
			printf("%c", buff_cts[i]);
		}
		
		//http message from server through proxy to client//
		ret_val_recv = 0; already_recv = 0;
		while( !recv_rnrn(buff_stc, already_recv) )
		{
			ret_val_recv = recv( out_sd, buff_stc + already_recv, HTTP_size_max, 0);//sizeof(buff) is 4 or 8
			if( ret_val_recv == -1 ){printf("ERROR recv1\n");}
			already_recv += ret_val_recv;
		}
		printf("\nalready_recv_server_to_proxy: %d\n", already_recv);
		
		for(i = 0; i < already_recv; i++)
		{
			printf("%c", buff_stc[i]);
		}
		
		ret_val_send = 0; already_send = 0;
		while( already_send < already_recv )
		{
			ret_val_send = send( *(int*) client_sd, buff_stc + already_send, already_recv - already_send, 0 );
			if( ret_val_send == -1 )
			{
				printf("ERROR send1\n");
				pthread_exit(0);
			}
			already_send += ret_val_send;
		}
		printf("\nalready_send_proxy_to_client: %d\n", already_send);
		for(i = 0; i < already_send; i++)
		{
			printf("%c", buff_stc[i]);
		}
		
		free(buff_cts);
		free(buff_stc);
	}
	
	free(IPaddr);
	free(Port);
	close(out_sd);
	pthread_exit(0);
	pthread_cleanup_pop(1);
}

void* deal_with_client_2(void* client_sd)
{
	pthread_cleanup_push(NULL, NULL);
	pthread_exit(0);
	pthread_cleanup_pop(1);
}

void* deal_with_client_3(void* client_sd)
{
	pthread_cleanup_push(NULL, NULL);
	pthread_exit(0);
	pthread_cleanup_pop(1);
}
