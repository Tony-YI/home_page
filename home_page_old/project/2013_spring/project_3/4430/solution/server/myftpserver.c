# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <pthread.h>
# include <arpa/inet.h>
# include <dirent.h>
# include "myftp.h"

# define SERVER_IPADDR "127.0.0.1"
# define PORT 12345
# define data_base "access.txt"
# define dir "filedir"//download or upload file position
# define max_lines 1024//maximum line number in one read of access.txt
# define server_state (char)'1'		//1 is up, 0 is down

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

int sd;//socket discripter
int accept_sd;

void connect_client()
{
	/****************************creat a socket************************/
	sd = socket( AF_INET, SOCK_STREAM, 0 );//AF_INET stands for IPV4, SOCK_STREAM stands for TCP
	//int client_sd;
	
	struct sockaddr_in server_addr;//server socket structer
	//struct sockaddr_in client_addr;//client socket structer
	
	memset( &server_addr, 0, sizeof(server_addr) );//Filled memory with constant bytes. void *memset(void *s, int c, size_t n);
													//The memset() function fills the first n bytes of the memory
													//area pointed to by s with the constant byte c.
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl( INADDR_ANY );//The htonl() function converts the unsigned integer  hostlong  from
														//host byte order to network byte order.
														//INADDR_ANY means that the server IP address can be any number.
														
	server_addr.sin_addr.s_addr = inet_addr( SERVER_IPADDR );//declear the IP address of the server
	server_addr.sin_port = htons( PORT );
	/******************************************************************/
	
	
	
	/*******bind the server socket to a specific port of server********/
	if( bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 )//int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
																			//When a socket is created with socket(2), it exists in a name space
																			//(address family) but has no address assigned to it. bind()
																			//assigns the address specified to by addr to the socket referred to
																			//by the file descriptor sockfd.  addrlen  specifies  the  size,  in
																			//bytes,  of  the  address structure pointed to by addr.

	{
		printf("Bind error: %s (Errno:%d)\n",strerror(errno),errno);
		close(sd);
		exit(0);
	}
	/******************************************************************/
	
}

void listen_client()
{
	/*******set the maximum number of connection of this socket********/
	//???????????1是干什么的???????????//
	if( listen(sd, 100) < 0 )//int listen(int sockfd, int backlog);
							//listen()  marks  the  socket  referred  to  by sockfd as a passive
							//socket, that is, as a socket that will be used to accept  incoming
							//connection requests using accept(2).
							// The backlog argument defines the maximum length to which the queue
							//of  pending  connections  for  sockfd  may  grow.  If a connection
							//request arrives when the queue is full, the client may receive an error
	{
		printf("Listen error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	/******************************************************************/
}

int valid_OPEN_CONN_REQUEST(struct message_s OPEN_CONN_REQUEST, struct message_s OPEN_CONN_REPLY)//-1 is wrong protocal message
																								//1 is valid OPEN_CONN_REQUEST
{
	if( memcmp(OPEN_CONN_REPLY.protocol, OPEN_CONN_REQUEST.protocol, 6) == 0 
		&& OPEN_CONN_REQUEST.type == (char)0xA1 && ntohl(OPEN_CONN_REQUEST.length) == 12 )//(char)0xA1
	{
		return 1;
	}
	else{return -1;}
}

int valid_AUTH_REQUEST(struct message_s AUTH_REQUEST, struct message_s AUTH_REPLY)//-1 is wrong protocal message
																						//1 is valid AUTH_REQUEST
{
	if( memcmp(AUTH_REPLY.protocol, AUTH_REQUEST.protocol, 6) == 0 
		&& AUTH_REQUEST.type == (char)0xA3 && ntohl(AUTH_REQUEST.length) >= 12 )//(char)0xA1
	{
		return 1;
	}
	else{return -1;}
}

int main_request_type(struct message_s REQUEST)//-1 is wrong protocal message
										//0 is valid LIST_REQUEST
										//1 is valid GET_REQUEST
										//2 is valid PUT_REQUEST
										//3 is valid GQUIT_REQUEST
{
	char protocol[6];
	memcpy(protocol, "0myftp", 6);
	protocol[0] = 0xE3;
	
	if( memcmp(protocol, REQUEST.protocol, 6) == 0 )
	{
		if( REQUEST.type == (char)0xA5 && ntohl(REQUEST.length) == 12){ return 0; }//list
		if( REQUEST.type == (char)0xA7 && ntohl(REQUEST.length) >= 12){ return 1; }//get
		if( REQUEST.type == (char)0xA9 && ntohl(REQUEST.length) >= 12){ return 2; }//put
		if( REQUEST.type == (char)0xAB && ntohl(REQUEST.length) == 12){ return 3; }//quit
	}
	
	return -1;
}

void open_connection( int client_sd )
{
	//initiallize OPEN_CONN_REPLY
	struct message_s OPEN_CONN_REPLY;
	memset( &OPEN_CONN_REPLY, 0, sizeof(OPEN_CONN_REPLY) );
	memcpy( OPEN_CONN_REPLY.protocol, "0myftp", 6 );
	OPEN_CONN_REPLY.protocol[0] = 0xE3;
	OPEN_CONN_REPLY.type = 0xA2;
	OPEN_CONN_REPLY.status = server_state;
	OPEN_CONN_REPLY.length = htonl(12);//must use htonl()
		
	//initiallize OPEN_CONN_REQUEST
	struct message_s OPEN_CONN_REQUEST;
	memset( &OPEN_CONN_REQUEST, 0, sizeof(OPEN_CONN_REQUEST) );
	
	//get request
	int len_r;//received data length
	if( (len_r = recv(client_sd, &OPEN_CONN_REQUEST, sizeof(OPEN_CONN_REQUEST), 0)) < 0 )
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		//exit(0);
		pthread_exit(0);
	}
	else if( len_r == 0 )//client down
	{
		printf("Client %d down, connection with client closed.\n", client_sd);
		close(client_sd);
		pthread_exit(0);
	}
	//check whether REQUEST valid or not
	int flag = valid_OPEN_CONN_REQUEST(OPEN_CONN_REQUEST, OPEN_CONN_REPLY);
	if( flag == 1 )
	{
		printf("Valid OPEN_CONN_REQUEST, sending OPEN_CONN_REPLY. Client %d.\n", client_sd);
		//send reply
		int len_s;
		if( (len_s = send(client_sd, &OPEN_CONN_REPLY, sizeof(OPEN_CONN_REPLY), 0)) < 0 )
		{
			printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
			close(client_sd);
			//exit(0);
			pthread_exit(0);
		}
	}
	else if( flag == -1 )
	{
		printf("Unknow error, terminating client %d connection.\n", client_sd);
		close(client_sd);
		//exit(0);
		pthread_exit(0);
		
	}
	
	if(server_state == '1')
	{
		printf("Connection with client %d successed.\n", client_sd);
	}
	else
	{
		printf("Server down, connection with client %d closed.\n", client_sd);
		close(client_sd);
		pthread_exit(0);
	}
	//print(OPEN_CONN_REQUEST, OPEN_CONN_REPLY);
	
	//pthread_exit(0);
}

int search_data_base( char* pay_load )
{
	FILE *fd = fopen( data_base, "r");
	
	if( fd == NULL )//open file failed
	{
		printf("Open file error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	else//file open successed
	{
		int line_num = 0;
		char name_password[max_lines][512];//assunme each name and password is less than 512 characters
										//every time we read 1024 lines of databse
		while( fgets( name_password[line_num], 512, fd ) != NULL && strcmp(name_password[line_num], "\n") != 0 )
		{
			name_password[line_num][strlen( name_password[line_num] ) - 1] = '\0';		
			line_num++;//number of lines
			
			if(line_num == max_lines)
			{
				line_num = 0;
				int j;
				for(j = 0; j < max_lines; j++)
				{
					//printf("%s:%d\n", name_password[j], j);
					if( strcmp(name_password[j], pay_load) == 0 )
					{
						return 1;
					}
				}
			}
		}
		
		int j;
		for(j = 0; j < line_num; j++)
		{
			//printf("%s:%d\n", name_password[j], j);
			if( strcmp(name_password[j], pay_load) == 0 )
			{
				return 1;
			}
		}
			
		fclose(fd);
	}
	
	return 0;
}
void authentication( int client_sd )
{
	//initiallize AUTH_REPLY
	struct message_s AUTH_REPLY;
	memset( &AUTH_REPLY, 0, sizeof(AUTH_REPLY) );
	memcpy( AUTH_REPLY.protocol, "0myftp", 6 );
	AUTH_REPLY.protocol[0] = 0xE3;
	AUTH_REPLY.type = 0xA4;
	AUTH_REPLY.length = htonl(12);
	
	//initiallize AUTH_REQUEST and receive from client
	struct message_s AUTH_REQUEST;
	memset( &AUTH_REQUEST, 0, sizeof(AUTH_REQUEST) );
	
	int len_r_REQUEST = recv(client_sd, &AUTH_REQUEST, sizeof(AUTH_REQUEST), 0);//received data length
	if( len_r_REQUEST  < 0 )
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		//exit(0);
		pthread_exit(0);
	}
	
	//what about recv() is less than 12 charcacters?
	
	else if( len_r_REQUEST == 0 )
	{
		printf("Client %d down, connection with client closed.\n", client_sd);
		close(client_sd);
		pthread_exit(0);
	}
	
	else//receive request successed
	{
		int flag = valid_AUTH_REQUEST( AUTH_REQUEST, AUTH_REPLY);
		if( flag == -1 )
		{
			printf("Unknow error, terminating connection with client %d.\n", client_sd);
			close(client_sd);
			//exit(0);
			pthread_exit(0);
		}
		else if( flag == 1 )
		{
			printf("AUTH_REQUEST received, receiving payload from client %d.\n", client_sd);
			
			//initiallize payload and receive from client
			int exist = 0;
			char pay_load[ ntohl(AUTH_REQUEST.length) - 12 ];
			//printf("%ld\n", sizeof(pay_load) );///////////////////////////////////////////
			int len_r_pay;//receive pay_load
			if( (len_r_pay = recv(client_sd, pay_load, sizeof(pay_load), 0)) < 0 )
													//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
													//The system calls send() is used to transmit a message to another socket.
													// With a zero flags argument, send() is equivalent to write(2).
													//On success, these calls return the number  of  characters sent.
													//On error, -1 is returned, and errno is set appropriately.
			{
				printf("Receive Error: %s (Errno:%d)\n", strerror(errno), errno);
				close(client_sd);
				//exit(0);
				pthread_exit(0);
			}
			
			//???????????what if len_r_pay is less than sizeof(pay_load)?????????????
			
			else//receive pay_load successed
			{
				//exist = 1;
				exist = search_data_base( pay_load );
			}
			
			//initiallize AUTH_REPLY and send to client
			if(exist == 0){ AUTH_REPLY.status = (char)'0';}
			else{ AUTH_REPLY.status = (char)'1'; }
			
			int len_s_AUTH = send(client_sd, &AUTH_REPLY, sizeof(AUTH_REPLY), 0);//send AUTH_REPLY
			if( len_s_AUTH < 0 )
										//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
										//The system calls send() is used to transmit a message to another socket.
										// With a zero flags argument, send() is equivalent to write(2).
										//On success, these calls return the number  of  characters sent.
										//On error, -1 is returned, and errno is set appropriately.
			{
				printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
				close(client_sd);
				//exit(0);
				pthread_exit(0);
			}	
			
			if( exist == 0 )
			{
				printf("No such client name, connection with client %d closed.\n", client_sd);
				close(client_sd);
				pthread_exit(0);
			}
			else
			{
				printf("Authentication for cliend %d successed.\n", client_sd);
			}
		}
	}
	
	//print(AUTH_REQUEST, AUTH_REPLY);
	
	//pthread_exit(0);
}

void main_list( int client_sd )
{
	//find file list and initiallize pay_load
	DIR *dirp;
	struct dirent *direntp;
	
	dirp = opendir( dir );
	int file_num = 0;
	while ( (direntp = readdir( dirp )) != NULL )//read directory until no dir is left
	{
		if( direntp->d_type == DT_REG )//regular file
		{
			file_num++;
			//printf( "%s\n", direntp->d_name );
		}
	}
	
	char file_list[file_num][256];//assume each file name is less than 256 characters
	file_num = 0;
	
	seekdir(dirp, 0);//return to the begining position
	
	while ( (direntp = readdir( dirp )) != NULL )//read directory until no dir is left
	{
		if( direntp->d_type == DT_REG )//regular file
		{
			strcpy( file_list[file_num], direntp->d_name );
			file_num++;
			//printf( "%s\n", direntp->d_name );
		}
	}
	closedir( dirp );
	//get the pay_load_length
	int i; int pay_load_length = 0;
	for(i = 0; i < file_num; i++)
	{
		pay_load_length += (strlen(file_list[i]) + 1);//1 byte for '\n';
		//printf("%s\n", file_list[i]);
	}
	++pay_load_length;//for '\0'
	char pay_load[pay_load_length];
	memset( pay_load, 0, sizeof(pay_load) );//must initiallize first
	for(i = 0; i < file_num; i++)
	{
		strcat( file_list[i], "\n" );
		strcat( pay_load, file_list[i] );
	}
	
	//initiallize and send LIST_REPLY
	struct message_s LIST_REPLY;
	memset( &LIST_REPLY, 0, sizeof(LIST_REPLY) );
	memcpy( LIST_REPLY.protocol, "0myftp", 6 );
	LIST_REPLY.protocol[0] = 0xE3;
	LIST_REPLY.type = 0xA6;
	LIST_REPLY.length = htonl( 12 + pay_load_length );// + 1 );//must use htonl()

	printf("Valid LIST_REQUEST, sending LIST_REPLY and payload. Client %d.\n", client_sd);
	//send reply
	int len_s;
	if( (len_s = send(client_sd, &LIST_REPLY, sizeof(LIST_REPLY), 0)) < 0 )
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		//exit(0);
		pthread_exit(0);
	}
	//send reply successed and send pay_load
	int len_s_p;
	if( (len_s_p = send(client_sd, &pay_load, pay_load_length, 0)) < 0 )
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		//exit(0);
		pthread_exit(0);
	}
	else//pay_load send successed
	{
		printf("Payload-send successed. Client %d.\n", client_sd);
		//printf("%s\n", pay_load);
		//printf( "%d\n", ntohl(LIST_REPLY.length) );
	}
}

void main_get( int client_sd, int pl_length )//REQUEST has been received
{
	printf("GET_REQUEST received, receiving payload from client %d.\n", client_sd);
	
	//initiallize GET_REPLY
	struct message_s GET_REPLY;
	memset( &GET_REPLY, 0, sizeof(GET_REPLY) );
	memcpy( GET_REPLY.protocol, "0myftp", 6 );
	GET_REPLY.protocol[0] = 0xE3;
	GET_REPLY.type = 0xA8;
	GET_REPLY.length = htonl(12);
				
	//initiallize payload and receive from client
	int exist = 0;
	char pay_load[pl_length];
	int len_r_pay;//receive pay_load
	char file_path [512];
	if( (len_r_pay = recv(client_sd, pay_load, sizeof(pay_load), 0)) < 0 )
													//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
													//The system calls send() is used to transmit a message to another socket.
													// With a zero flags argument, send() is equivalent to write(2).
													//On success, these calls return the number  of  characters sent.
													//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Receive Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		pthread_exit(0);
	}
			
	//???????????what if len_r_pay is less than sizeof(pay_load)?????????????
			
	else//receive pay_load successed
	{
		strcpy( file_path, "./filedir/" );
		strcat( file_path, pay_load );
		FILE* P = fopen(file_path, "r");
		if( P ){ exist = 1; fclose(P);}
		else{ exist = 0; }
	}
			
	//initiallize GET_REPLY and send to client
	if(exist == 0){ GET_REPLY.status = (char)'0';}
	else
	{ GET_REPLY.status = (char)'1'; }
			
	int len_s_REPLY = send(client_sd, &GET_REPLY, sizeof(GET_REPLY), 0);//send GET_REPLY
	if( len_s_REPLY < 0 )
								//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
								//The system calls send() is used to transmit a message to another socket.
								// With a zero flags argument, send() is equivalent to write(2).
								//On success, these calls return the number  of  characters sent.
								//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		pthread_exit(0);
	}
			
	if(exist == 1)//file exist. send to the client
	{
		printf("Sending file to cliend %d.\n", client_sd);
				
		//find out the file size
		FILE * fd = fopen( file_path, "r" );
		if( fd == NULL )
		{
			printf("File open error: %s (Errno:%d)\n", strerror(errno), errno);
			close(client_sd);
			pthread_exit(0);
		}
		fseek( fd, 0L, SEEK_END );  
		int file_size = ftell(fd);  
		fclose(fd);
				
		//initiallize FILE_DATA and send
		struct message_s FILE_DATA;
		memset( &FILE_DATA, 0, sizeof(FILE_DATA) );
		memcpy( FILE_DATA.protocol, "0myftp", 6 );
		FILE_DATA.protocol[0] = 0xE3;
		FILE_DATA.type = 0xFF;
		FILE_DATA.length = htonl(12 + file_size);
				
		int len_s_FD = send(client_sd, &FILE_DATA, sizeof(FILE_DATA), 0);//send FILE_DATA
		if( len_s_FD < 0 )
											//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
											//The system calls send() is used to transmit a message to another socket.
											// With a zero flags argument, send() is equivalent to write(2).
											//On success, these calls return the number  of  characters sent.
											//On error, -1 is returned, and errno is set appropriately.
		{
			printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
			close(client_sd);
			pthread_exit(0);
		}
				
		//send successed, initiallize payload and send
		char buffer[1024 * 4];//4kB ;
		memset(buffer, 0, sizeof(buffer));
				
		FILE * fp = fopen( file_path, "r" );
		if( fd == NULL )
		{
			printf("File open error: %s (Errno:%d)\n", strerror(errno), errno);
			close(client_sd);
			pthread_exit(0);
		}
				
		int remain_size = file_size;
		while(remain_size >= 4 * 1024)
		{
			fread(buffer, 4 * 1024, 1, fp);
			int sentLen = 0;
			while( sentLen < 4 * 1024 )
			{
				int len_t = send(client_sd, buffer + sentLen, 4 * 1024 - sentLen, 0);
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
				int len_t = send(client_sd, buffer + sentLen, remain_size - sentLen, 0);
				sentLen += len_t;
			}
		}
		remain_size = 0;
		printf("File sent.\n");
		fclose(fp);
	}
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
void main_put( int client_sd, int pl_length )//REUEST has been received
{
	printf("PUT_REQUEST received, receiving payload from client %d.\n", client_sd);
	
	//initiallize payload and receive from client
	char pay_load[pl_length];
	int len_r_pay;//receive pay_load
	char file_path [512];
	if( (len_r_pay = recv(client_sd, pay_load, sizeof(pay_load), 0)) < 0 )
													//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
													//The system calls send() is used to transmit a message to another socket.
													// With a zero flags argument, send() is equivalent to write(2).
													//On success, these calls return the number  of  characters sent.
													//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Receive Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		pthread_exit(0);
	}
			
	//???????????what if len_r_pay is less than sizeof(pay_load)?????????????
			
	strcpy( file_path, "./filedir/" );
	strcat( file_path, pay_load );
	
	//initiallize PUT_REPLY and send
	struct message_s PUT_REPLY;
	memset( &PUT_REPLY, 0, sizeof(PUT_REPLY) );
	memcpy( PUT_REPLY.protocol, "0myftp", 6 );
	PUT_REPLY.protocol[0] = 0xE3;
	PUT_REPLY.type = 0xAA;
	PUT_REPLY.length = htonl(12);
	
	int len_s_REPLY = send(client_sd, &PUT_REPLY, sizeof(PUT_REPLY), 0);//send PUT_REPLY
	if( len_s_REPLY < 0 )
								//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
								//The system calls send() is used to transmit a message to another socket.
								// With a zero flags argument, send() is equivalent to write(2).
								//On success, these calls return the number  of  characters sent.
								//On error, -1 is returned, and errno is set appropriately.
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		pthread_exit(0);
	}	
			
	//Receive the FILE_DATA message
	printf("Receiving file from cliend %d.\n", client_sd);
	
	struct message_s FILE_DATA;
	memset( &FILE_DATA, 0, sizeof(FILE_DATA) );
			
	int len_r_FD;//received data length
	if( (len_r_FD = recv(client_sd, &FILE_DATA, sizeof(FILE_DATA), 0)) < 0 )
	{
		printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		pthread_exit(0);
	}
	else if( len_r_FD == 0 )//client down
	{
		printf("Client down, connection with client closed.\n");
		close(client_sd);
		pthread_exit(0);
	}
	else
	{
		int flag = valid_FILE_DATA( FILE_DATA , PUT_REPLY );
		if( flag == -1 )
		{
			printf("Unknow error, terminating connection with client.\n");
			close(client_sd);
			pthread_exit(0);
		}
		else if( flag == 1 )
		{
			//Download file here
			int file_size = ntohl(FILE_DATA.length) - 12;
			int remain_size = file_size;
			FILE* fd = fopen(file_path, "w");
			char buffer[4 * 1024];
			memset(buffer, 0, sizeof(buffer));
					
			while(remain_size >= 4 * 1024)
			{
				int recvLen = 0;
				while( recvLen < 4 * 1024 )
				{
					int len_t = recv(client_sd, buffer + recvLen, 4 * 1024 - recvLen, 0);
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
					int len_t = recv(client_sd, buffer + recvLen, remain_size - recvLen, 0);
					recvLen += len_t;
				}
				fwrite(buffer, remain_size, 1, fd);
			}
			remain_size = 0;
			printf("File received.\n");
			fclose(fd);
		}
	}
}

void main_quit( int client_sd )
{
	//initiallize QUIT_REPLY
	struct message_s QUIT_REPLY;
	memset( &QUIT_REPLY, 0, sizeof(QUIT_REPLY) );
	memcpy( QUIT_REPLY.protocol, "0myftp", 6 );
	QUIT_REPLY.protocol[0] = 0xE3;
	QUIT_REPLY.type = 0xAC;
	QUIT_REPLY.length = htonl(12);//must use htonl()

	printf("Valid QUIT_REQUEST, sending QUIT_REPLY. Client %d.\n", client_sd);
	//send reply
	int len_s;
	if( (len_s = send(client_sd, &QUIT_REPLY, sizeof(QUIT_REPLY), 0)) < 0 )
	{
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		close(client_sd);
		//exit(0);
		pthread_exit(0);
	}
	//send reply successed
	printf("Server for client %d is done. Connection closed.\n", client_sd);
	
	close(client_sd);
	pthread_exit(0);
}

void* deal_with_client( void* client_sd )
{
	open_connection( *(int*) client_sd );
	authentication( *(int*) client_sd );
	
	//main function//
	enum main_state {wait_request, list, get, put, quit};
	enum main_state state = wait_request;
	
	while(1)
	{
		//initiallize REQUEST
		struct message_s REQUEST;
		memset( &REQUEST, 0, sizeof(REQUEST) );
		
		switch( state )
		{
			case wait_request:
			{
				//get request
				int len_r;//received data length
				if( (len_r = recv( *(int*) client_sd, &REQUEST, sizeof(REQUEST), 0) ) < 0 )
				{
					printf("Receive error: %s (Errno:%d)\n", strerror(errno), errno);
					close( *(int*) client_sd );
					//exit(0);
					pthread_exit(0);
				}
				else if( len_r == 0 )//client down
				{
					printf("Client %d down, connection with client closed.\n", *(int*)client_sd);
					close( *(int*)client_sd );
					pthread_exit(0);
				}
				
				//reveive request successed
				//check REQUEST type, -1 is wrong requset, 0 is list, 1 is get, 2 is put, 3 is quit
				int type = main_request_type( REQUEST );
				
				if( type == -1 )
				{
					printf("Unknow error, terminating client %d connection.\n", *(int*) client_sd);
					close( *(int*) client_sd );
					//exit(0);
					pthread_exit(0);
				}
				else if( type == 0 ){state = list;}
				else if( type == 1 ){state = get;}
				else if( type == 2 ){state = put;}
				else if( type == 3 ){state = quit;}
			}break;
			
			case list:
			{
				main_list( *(int*) client_sd );//no pthread_exit(0) if no error
				state = wait_request;
			}break;
			
			case get:
			{
				main_get( *(int*) client_sd, ntohl(REQUEST.length) - 12 );//no pthread_exit(0) if no error
				state = wait_request;
			}break;
			
			case put:
			{
				main_put( *(int*) client_sd, ntohl(REQUEST.length) - 12 );//no pthread_exit(0) if no error
				state = wait_request;
			}break;
			
			case quit:
			{
				main_quit( *(int*) client_sd );//has involved pthread_exit(0)
			}break;
		}
	}
	
	//pthread_exit(0);
}

int main(int argc, char** argv)
{
	connect_client(sd);
	listen_client();
	
	while(1)
	{	
		struct sockaddr_in client_addr;//client socket structer
		socklen_t addr_len = sizeof( client_addr );//The length of client address, IPV4 is 32 bits

		//connect to the client using client_sd
		if( (accept_sd = accept(sd, (struct sockaddr*) &client_addr, &addr_len)) < 0 )
		{//accept() is blocking function
			printf("accept error: %s (Errno:%d)\n", strerror(errno), errno);
			close(sd);
			exit(0);
		}
		
		///////////////accept client successed/////////////////
		pthread_t client_thread;
		int client_sd = accept_sd;
		int ret_val = pthread_create(&client_thread, NULL, deal_with_client, &client_sd );//dealing with client
		if( ret_val < 0 )
		{
			printf("pthread create error: %s (Errno:%d)\n", strerror(errno), errno);
			close(sd);
			close(accept_sd);
			exit(0);
		}
		/////////////////////////////////////////////////////	
	}
	
	//close(client_sd);
	close(sd);//close the server socket
		
	return 0;
}
