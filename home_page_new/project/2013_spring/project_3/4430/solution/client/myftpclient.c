# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include "myftp.h"

//# define SERVER_IPADDR "127.0.0.1"
//# define SEVER_PORT 12345

void print(struct message_s OPEN_CONN_REQUEST, struct message_s OPEN_CONN_REPLY)
{
	printf("REQUEST\n");
	int i;
	for(i = 0; i < 6; i++)
	{
		printf("%c", OPEN_CONN_REQUEST.protocol[i]);
	}
	printf("\ntype %x\n", OPEN_CONN_REQUEST.type);
	printf("status %c\n", OPEN_CONN_REQUEST.status);
	printf("length %d\n", ntohl(OPEN_CONN_REQUEST.length));
	printf("\n");
	printf("REPLY\n");
	for(i = 0; i < 6; i++)
	{
		printf("%c", OPEN_CONN_REPLY.protocol[i]);
	}
	printf("\ntype %x\n", OPEN_CONN_REPLY.type);
	printf("status %c\n", OPEN_CONN_REPLY.status);
	printf("length %d\n", ntohl(OPEN_CONN_REPLY.length));
}

enum state_machine {IDLE, open_conn, auth, main_f};
enum state_machine state = IDLE;

int sd;//socket discriptor

void connect_server(char tokens[3][256])
{
	/****************************creat a socket************************/
	sd = socket(AF_INET, SOCK_STREAM, 0);//AF_INET stands for IPV4, SOCK_STREAM stands for TCP
	
	struct sockaddr_in server_addr;//struct of socket
	memset( &server_addr, 0, sizeof(server_addr) );//Filled memory with constant bytes. void *memset(void *s, int c, size_t n);
													//The memset() function fills the first n bytes of the memory
													//area pointed to by s with the constant byte c.
	server_addr.sin_family = AF_INET;//Stands for IPV4
	server_addr.sin_addr.s_addr = inet_addr( tokens[1]/*SERVER_IPADDR*/ );//argv[1] is the IP address of server
	server_addr.sin_port = htons( atoi(tokens[2])/*SEVER_PORT*/ );//PORT is the port number of the server. htons() stands for "host to network short"
										//send data to the network without worry about the big-endian or little-endian
	/******************************************************************/
	
	//no need to bind.
	
	/***********************connect to the server**********************/
	if( connect(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 )//int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
																				//The connect() system call connects the socket referred to
																				//by the file descriptor sock fd to the address specified by addr.
																				//If  the connection or binding succeeds, 0 is returned.
																				//On error, -1 is returned, and errno is set appropriately.
	{
		printf("connection error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	/******************************************************************/
}

void get_tokens(int* i, char temp[256 * 3], char tokens[3][256], char* retval)
{
	gets(temp);
	if( (retval = strtok( temp, " " )) )
	{
		strcpy( tokens[(*i)++], retval );
		while( (retval = strtok( NULL, " " )) )
		{
			strcpy( tokens[(*i)++], retval );
		}
	}
}

int valid_OPEN_CONN_REPLY(struct message_s OPEN_CONN_REQUEST, struct message_s OPEN_CONN_REPLY)//-1 is wrong protocal message
																								//0 is server down
																								//1 is server up
{
	if( memcmp(OPEN_CONN_REPLY.protocol, OPEN_CONN_REQUEST.protocol, 6) == 0 
		&& OPEN_CONN_REPLY.type == (char)0xA2 && ntohl(OPEN_CONN_REPLY.length) == 12 )//ntohl()
	{
		if(OPEN_CONN_REPLY.status == '1')
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else{return -1;}
}

int valid_AUTH_REPLY(struct message_s AUTH_REQUEST, struct message_s AUTH_REPLY)//-1 is wrong protocal message
																				//0 is auth deline
																				//1 is auth accept
{
	if( memcmp(AUTH_REPLY.protocol, AUTH_REQUEST.protocol, 6) == 0 
		&& AUTH_REPLY.type == (char)0xA4 && ntohl(AUTH_REPLY.length) == 12 )//ntohl()
	{
		if(AUTH_REPLY.status == '1')
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else{return -1;}
}

int valid_LIST_REPLY(struct message_s LIST_REQUEST, struct message_s LIST_REPLY)//-1 is wrong protocal message
																				//1 is list reply
{
	if( memcmp(LIST_REPLY.protocol, LIST_REQUEST.protocol, 6) == 0 
		&& LIST_REPLY.type == (char)0xA6 && ntohl(LIST_REPLY.length) >= 12 )//ntohl()
	{
		return 1;
	}
	else{return -1;}
}

int valid_PUT_REPLY(struct message_s PUT_REQUEST, struct message_s PUT_REPLY)//-1 is wrong protocal message
																			//1 is valid reply
{
	if( memcmp(PUT_REPLY.protocol, PUT_REQUEST.protocol, 6) == 0 
		&& PUT_REPLY.type == (char)0xAA && ntohl(PUT_REPLY.length) == 12 )//ntohl()
	{
		return 1;
	}
	else{return -1;}
}

int valid_GET_REPLY(struct message_s GET_REQUEST, struct message_s GET_REPLY)//-1 is wrong protocal message
																			//0 is no such file
																			//1 is file-exist
{
	if( memcmp(GET_REPLY.protocol, GET_REQUEST.protocol, 6) == 0 
		&& GET_REPLY.type == (char)0xA8 && ntohl(GET_REPLY.length) == 12 )//ntohl()
	{
		if(GET_REPLY.status == '1')
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else{return -1;}
}
int valid_FILE_DATA( struct message_s FILE_DATA, struct message_s something )//-1 is wrong protocal message
																			//1 is valided
{
	if( memcmp(FILE_DATA.protocol, something.protocol, 6) == 0 
		&& FILE_DATA.type == (char)0xFF && ntohl(FILE_DATA.length) >= 12 )//ntohl()
	{
		return 1;
	}
	else{return -1;}
}

int valid_QUIT_REPLY(struct message_s QUIT_REQUEST, struct message_s QUIT_REPLY)//-1 is wrong protocal message
																				//1 is quit reply
{
	if( memcmp(QUIT_REPLY.protocol, QUIT_REQUEST.protocol, 6) == 0 
		&& QUIT_REPLY.type == (char)0xAC && ntohl(QUIT_REPLY.length) == 12 )//ntohl()
	{
		return 1;
	}
	else{return -1;}
}

void open_connection()//has dealed with all possibilities in open_connecting state
{
	//initiallize OPEN_CONN_REQUEST
	struct message_s OPEN_CONN_REQUEST;
	memset( &OPEN_CONN_REQUEST, 0, sizeof(OPEN_CONN_REQUEST) );
	memcpy( OPEN_CONN_REQUEST.protocol, "0myftp", 6 );
	OPEN_CONN_REQUEST.protocol[0] = 0xE3;
	OPEN_CONN_REQUEST.type = 0xA1;
	OPEN_CONN_REQUEST.length = htonl(12);
	
	//initiallize OPEN_CONN_REPLY
	struct message_s OPEN_CONN_REPLY;
	memset( &OPEN_CONN_REPLY, 0, sizeof(OPEN_CONN_REPLY) );
	
	//send request
	int len_s;//send data length;
	if( (len_s = send(sd, &OPEN_CONN_REQUEST, sizeof(OPEN_CONN_REQUEST), 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//receive reply
	else
	{
		int len_r;//received data length
		if( (len_r = recv(sd, &OPEN_CONN_REPLY, sizeof(OPEN_CONN_REPLY), 0)) < 0 )
		{
			printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
			close(sd);
			exit(0);
		}
		else if( len_r == 0 )//server down
		{
			printf("Server down, connection with server closed.\n");
			close(sd);
			exit(0);
		}
		else
		{
			int flag = valid_OPEN_CONN_REPLY( OPEN_CONN_REQUEST, OPEN_CONN_REPLY);
			if( flag == -1 )
			{
				printf("Unknow error, terminating connection with server.\n");
				close(sd);
				exit(0);
			}
			else if( flag == 0 )
			{
				printf("Server down.\n");
				close(sd);
				state = IDLE;
			}
			else if( flag == 1 )
			{
				printf("Server connection accepted.\n");
				state = auth;
			}
		}
	}
	//print(OPEN_CONN_REQUEST, OPEN_CONN_REPLY);
}

void authentication()
{
	//get user name an pass word
	printf("User Name: ");
	char user_name[256];
	//scanf("%s", user_name);
	gets(user_name);
	user_name[strlen(user_name)] = '\0';//must put this in the char[], otherwise we can't use strcpy() and strcat()
	printf("Password: ");
	char pass_word[256];
	//scanf("%s", pass_word);
	gets(pass_word);
	pass_word[strlen(pass_word)] = '\0';//must put this in the char[], otherwise we can't use strcpy() and strcat()
	
	char pay_load[512];
	char temp[2] = " ";
	strcpy( pay_load, user_name );
	strcat( pay_load, temp );
	strcat( pay_load, pass_word );
	
	//initiallize AUTH_REQUEST and send to server
	struct message_s AUTH_REQUEST;
	memset( &AUTH_REQUEST, 0, sizeof(AUTH_REQUEST) );
	memcpy( AUTH_REQUEST.protocol, "0myftp", 6 );
	AUTH_REQUEST.protocol[0] = 0xE3;
	AUTH_REQUEST.type = 0xA3;
	AUTH_REQUEST.length = htonl( strlen(pay_load) + 1 + 12 );
	
	int len_s_AUTH;//send AUTH_REQUEST
	if( (len_s_AUTH = send(sd, &AUTH_REQUEST, sizeof(AUTH_REQUEST), 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//initiallize payload and send to server
	int len_s_pay;//send pay_load
	if( (len_s_pay = send(sd, pay_load, strlen(pay_load) + 1, 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//printf("len_s_pay: %d\n", len_s_pay);/////////////////////////////
	
	//initiallize AUTH_REPLY and receive from server
	struct message_s AUTH_REPLY;
	memset( &AUTH_REPLY, 0, sizeof(AUTH_REPLY) );
	
	int len_r_AUTH;//received data length
	if( (len_r_AUTH = recv(sd, &AUTH_REPLY, sizeof(AUTH_REPLY), 0)) < 0 )
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	else if( len_r_AUTH == 0 )//server down
	{
		printf("Server down, connection with server closed.\n");
		close(sd);
		exit(0);
	}
	else
	{
		int flag = valid_AUTH_REPLY( AUTH_REQUEST, AUTH_REPLY);
		if( flag == -1 )
		{
			printf("Unknow error, terminating connection with server.\n");
			close(sd);
			exit(0);
		}
		else if( flag == 0 )
		{
			printf("ERROR: Authentication rejected. Connection closed.\n");
			close(sd);
			state = IDLE;
		}
		else if( flag == 1 )
		{
			printf("Authentication granted.\n");
			state = main_f;
		}
	}
	//print(AUTH_REQUEST, AUTH_REPLY);
}

void main_list()
{
	//initiallize LIST_REQUEST and send to server
	struct message_s LIST_REQUEST;
	memset( &LIST_REQUEST, 0, sizeof(LIST_REQUEST) );
	memcpy( LIST_REQUEST.protocol, "0myftp", 6 );
	LIST_REQUEST.protocol[0] = 0xE3;
	LIST_REQUEST.type = 0xA5;
	LIST_REQUEST.length = htonl(12);
	
	int len_s_LIST;//send LIST_REQUEST
	if( (len_s_LIST = send(sd, &LIST_REQUEST, sizeof(LIST_REQUEST), 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}

	//initiallize LIST_REPLY and receive from server
	struct message_s LIST_REPLY;
	memset( &LIST_REPLY, 0, sizeof(LIST_REPLY) );
	
	int len_r_LIST;//received data length
	if( (len_r_LIST = recv(sd, &LIST_REPLY, sizeof(LIST_REPLY), 0)) < 0 )
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	else if( len_r_LIST == 0 )//server down
	{
		printf("Server down, connection with server closed.\n");
		close(sd);
		exit(0);
	}
	else//receive reply successed
	{
		int flag = valid_LIST_REPLY( LIST_REQUEST, LIST_REPLY);
		if( flag == -1 )
		{
			printf("Unknow error, terminating connection with server.\n");
			close(sd);
			exit(0);
		}
		else if( flag == 1 )
		{
			printf("Listing files...\n");
		}
	}
	
	//initiallize payload and receive from server
	char pay_load[ ntohl(LIST_REPLY.length) - 12 ];
	int len_r_pay;//receive pay_load
	if( (len_r_pay = recv(sd, pay_load, ntohl(LIST_REPLY.length) - 12, 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	else if( len_r_pay == 0 || len_r_pay < (ntohl(LIST_REPLY.length) - 12) )//server down
	{
		printf("Server down, connection with server closed.\n");
		close(sd);
		exit(0);
	}
	else//reveive play_load successed, listing file
	{
		printf("%s", pay_load);
		//printf( "%d\n", ntohl(LIST_REPLY.length) );
	}
}

void main_get( char* tokens )
{
	char file_name[256];//assume file name is less than 256 characters
	strcpy( file_name, tokens );
	
	//initiallize GET_REQUEST and send to server
	struct message_s GET_REQUEST;
	memset( &GET_REQUEST, 0, sizeof(GET_REQUEST) );
	memcpy( GET_REQUEST.protocol, "0myftp", 6 );
	GET_REQUEST.protocol[0] = 0xE3;
	GET_REQUEST.type = 0xA7;
	GET_REQUEST.length = htonl( strlen(file_name) + 1 + 12 );
	
	int len_s_GET;//send GET_REQUEST
	if( (len_s_GET = send(sd, &GET_REQUEST, sizeof(GET_REQUEST), 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//initiallize payload and send to server
	int len_s_pay;//send pay_load
	if( (len_s_pay = send(sd, file_name, strlen(file_name) + 1, 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//send successed, initiallize GET_REPLY and receive from server
	struct message_s GET_REPLY;
	memset( &GET_REPLY, 0, sizeof(GET_REPLY) );
	
	int len_r_GET;//received data length
	if( (len_r_GET = recv(sd, &GET_REPLY, sizeof(GET_REPLY), 0)) < 0 )
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	else if( len_r_GET == 0 )//server down
	{
		printf("Server down, connection with server closed.\n");
		close(sd);
		exit(0);
	}
	else
	{
		int flag = valid_GET_REPLY( GET_REQUEST, GET_REPLY);
		if( flag == -1 )
		{
			printf("Unknow error, terminating connection with server.\n");
			close(sd);
			exit(0);
		}
		else if( flag == 0 )
		{
			printf("ERROR: No such file.\n");
		}
		else if( flag == 1 )
		{
			printf("Downloading file...\n");
			
			//Receive the FILE_DATA message
			struct message_s FILE_DATA;
			memset( &FILE_DATA, 0, sizeof(FILE_DATA) );
			
			int len_r_FD;//received data length
			if( (len_r_FD = recv(sd, &FILE_DATA, sizeof(FILE_DATA), 0)) < 0 )
			{
				printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
				close(sd);
				exit(0);
			}
			else if( len_r_FD == 0 )//server down
			{
				printf("Server down, connection with server closed.\n");
				close(sd);
				exit(0);
			}
			else
			{
				int flag = valid_FILE_DATA( FILE_DATA , GET_REQUEST );
				if( flag == -1 )
				{
					printf("Unknow error, terminating connection with server.\n");
					close(sd);
					exit(0);
				}
				else if( flag == 1 )
				{
					//Download file here
					int file_size = ntohl(FILE_DATA.length) - 12;
					int remain_size = file_size;
					FILE* fd = fopen(tokens, "w");
					char buffer[4 * 1024];
					memset(buffer, 0, sizeof(buffer));
					
					while(remain_size >= 4 * 1024)
					{
						int recvLen = 0;
						while( recvLen < 4 * 1024 )
						{
							int len_t = recv(sd, buffer + recvLen, 4 * 1024 - recvLen, 0);
							recvLen += len_t;
						}
						fwrite(buffer, 4 * 1024, 1, fd);
						remain_size -= 4 * 1024;
					}
					if(remain_size)
					{
						int recvLen = 0;
						while( recvLen < remain_size )
						{
							int len_t = recv(sd, buffer + recvLen, remain_size - recvLen, 0);
							recvLen += len_t;
						}
						fwrite(buffer, remain_size, 1, fd);
					}
					remain_size = 0;
					printf("File downloaded.\n");
					fclose(fd);
				}
			}
		}
	}
}

void main_put( char* tokens )
{
	char file_name[256];//assume file name is less than 256 characters
	memset(file_name, 0, 256);
	int file_size;
	strcat( file_name, tokens );
	
	//check file existence and get file size
	FILE* temp_fd = fopen(file_name, "r");
	if(temp_fd)
	{
		fseek( temp_fd, 0L, SEEK_END );  
		file_size = ftell(temp_fd);
		fclose(temp_fd);
	}
	else
	{
		printf("ERROR: No such file.\n");
		return;
	}
	
	//file locally exists
	//initiallize PUT_REQUEST and send to server
	struct message_s PUT_REQUEST;
	memset( &PUT_REQUEST, 0, sizeof(PUT_REQUEST) );
	memcpy( PUT_REQUEST.protocol, "0myftp", 6 );
	PUT_REQUEST.protocol[0] = 0xE3;
	PUT_REQUEST.type = 0xA9;
	PUT_REQUEST.length = htonl( strlen(file_name) + 1 + 12 );
	
	int len_s_PUT;//send PUT_REQUEST
	if( (len_s_PUT = send(sd, &PUT_REQUEST, sizeof(PUT_REQUEST), 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//initiallize payload and send to server
	int len_s_pay;//send pay_load
	if( (len_s_pay = send(sd, file_name, strlen(file_name) + 1, 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//send successed, initiallize PUT_REPLY and receive from server
	struct message_s PUT_REPLY;
	memset( &PUT_REPLY, 0, sizeof(PUT_REPLY) );
	
	int len_r_PUT;//received data length
	if( (len_r_PUT = recv(sd, &PUT_REPLY, sizeof(PUT_REPLY), 0)) < 0 )
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	else if( len_r_PUT == 0 )//server down
	{
		printf("Server down, connection with server closed.\n");
		close(sd);
		exit(0);
	}
	else
	{
		int flag = valid_PUT_REPLY( PUT_REQUEST, PUT_REPLY);
		if( flag == -1 )
		{
			printf("Unknow error, terminating connection with server.\n");
			close(sd);
			exit(0);
		}
		else if( flag == 1 )//valid reply
		{
			printf("Uploading file...\n");
			
			//initiallize FILE_DATA and send
			struct message_s FILE_DATA;
			memset( &FILE_DATA, 0, sizeof(FILE_DATA) );
			memcpy( FILE_DATA.protocol, "0myftp", 6 );
			FILE_DATA.protocol[0] = 0xE3;
			FILE_DATA.type = 0xFF;
			FILE_DATA.length = htonl(12 + file_size);
					
			int len_s_FD = send(sd, &FILE_DATA, sizeof(FILE_DATA), 0);//send FILE_DATA
			if( len_s_FD < 0 )
												//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
												//The system calls send() is used to transmit a message to another socket.
												// With a zero flags argument, send() is equivalent to write(2).
												//On success, these calls return the number  of  characters sent.
												//On error, -1 is returned, and errno is set appropriately.
			{
				printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
				close(sd);
				exit(0);
			}
				
			//send successed, initiallize payload and send
			char buffer[1024 * 4];//4kB ;
			memset(buffer, 0, sizeof(buffer));		
			FILE* fp = fopen( file_name, "r" );
					
			int remain_size = file_size;
			while(remain_size >= 4 * 1024)
			{
				fread(buffer, 4 * 1024, 1, fp);
				int sentLen = 0;
				while( sentLen < 4 * 1024 )
				{
					int len_t = send(sd, buffer + sentLen, 4 * 1024 - sentLen, 0);
					sentLen += len_t;
				}
				remain_size -= 4 * 1024;
			}
			if(remain_size)
			{
				fread(buffer, remain_size, 1, fp);
				int sentLen = 0;
				while( sentLen < remain_size )
				{
					int len_t = send(sd, buffer + sentLen, remain_size - sentLen, 0);
					sentLen += len_t;
				}
			}
			remain_size = 0;
			printf("File uploaded.\n");
			fclose(fp);
		}
	}
}

void main_quit()
{
	//initiallize QUICK_REQUEST
	struct message_s QUIT_REQUEST;
	memset( &QUIT_REQUEST, 0, sizeof(QUIT_REQUEST) );
	memcpy( QUIT_REQUEST.protocol, "0myftp", 6 );
	QUIT_REQUEST.protocol[0] = 0xE3;
	QUIT_REQUEST.type = 0xAB;
	QUIT_REQUEST.length = htonl(12);
	
	//initiallize QUICK_REPLY
	struct message_s QUIT_REPLY;
	memset( &QUIT_REPLY, 0, sizeof(QUIT_REPLY) );
	
	//send request
	int len_s;//send data length;
	if( (len_s = send(sd, &QUIT_REQUEST, sizeof(QUIT_REQUEST), 0)) < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(sd);
		exit(0);
	}
	
	//receive reply
	else//send request successed
	{
		int len_r;//received data length
		if( (len_r = recv(sd, &QUIT_REPLY, sizeof(QUIT_REPLY), 0)) < 0 )
		{
			printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
			close(sd);
			exit(0);
		}
		else if( len_r == 0 )//server down
		{
			printf("Server down, connection with server closed.\n");
			close(sd);
			exit(0);
		}
		else//receive reply successed
		{
			int flag = valid_QUIT_REPLY( QUIT_REQUEST, QUIT_REPLY);
			if( flag == -1 )
			{
				printf("Unknow error, terminating connection with server.\n");
				close(sd);
				exit(0);
			}
			else if( flag == 1 )
			{
				printf("Thank you for using. Connection with server is closed.\n");
				close(sd);
				exit(0);
			}
		}
	}
}

int main(int argc, char** argv)
{
	while(1)
	{
		char temp[256 * 3];//assume each command is less than 256 characters and at 3 commands
		char tokens[3][256];
		char* retval = NULL;
		int i = 0;
		
		switch( state )
		{
			case IDLE:
			{//需要这个大括号
				printf("Clinet> ");
				get_tokens(&i, temp, tokens, retval);
				if( i == 3 && strcmp("open", tokens[0]) == 0 )//"open" command
				{
					connect_server( tokens );
					state = open_conn;
				}
				else
				{
					printf("usage: open SERVER_IP SERVER_PORT\n");
					state = IDLE;
				}
			}break;
				
			case open_conn:
			{
				open_connection();//included changing state to authentication
			}break;
			
			case auth:
			{
				authentication();//included changing state to main_f
			}break;
			
			case main_f:
			{
				printf("Clinet> ");
				get_tokens(&i, temp, tokens, retval);
				if( i == 1 && strcmp("quit", tokens[0]) == 0 )//"quit" command
				{
					main_quit();//has exit(0) in main_quit()
				}
				else if( i == 1 && strcmp("ls", tokens[0]) == 0 )//"ls" command
				{
					main_list();//no exit(0) in main_list() if listed files successed
					state = main_f;
				}
				else if( i == 2 && strcmp("get", tokens[0]) == 0 )//"get" command
				{
					main_get( tokens[1] );//no exit(0) in main_get() if get file successed
					state = main_f;
				}
				else if( i == 2 && strcmp("put", tokens[0]) == 0 )//"put" command
				{
					main_put( tokens[1] );//no exit(0) in main_put() if put file successed
					state = main_f;
				}
				else
				{
					printf("usage for quit: quit\n");
					printf("usage for list files on server: ls\n");
					printf("usage for download file from server: get FILE_NAME\n");
					printf("usage for upload file to server: put FILE_NAME\n");
					state = main_f;
				}
			}break;
		}
	}
	
	close(sd);
	return 0;
}
