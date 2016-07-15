#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <pthread.h>


#define PORT 60000
#define MAX_MESSAGE_SIZE 255
#define MESSAGELENGTH 2

/*

protacal from phone to board

type operation

connect request cr  1
start movement  sm  2 
confirm picture cp  3 

pattern 1 p1  4
pattern 2 p2  5
pattern 3 p3  6
pattern 4 p4  7

car 				 
forward  fw	 8
backward bw	 9
left     lw	 10
right    rw 0x04   11 


camera 				 
forward   up	 12
backward  do	 13
left      le	 14
right     ri	 15


lift 				
up li         16
down ld		 17

-------------------------------------

protacal from board to  phone

connect established 0x00 0x01


pattern 1 ack 0x05 0x01  
pattern 2 ack 0x05 0x02
pattern 3 ack 0x05 0x03  
pattern 4 ack 0x05 0x04

car
forward	ack	  0x05 0x05
backward ack  0x05 0x06
left ack 	  0x05 0x07
right ack     0x05 0x08


camera
forward	ack	  0x05 0x09
backward ack  0x05 0x0a
left ack 	  0x05 0x0b
right ack     0x05 0x0c


lift
up ack		  0x05 0x0d
down ack      0x05 0x0e

finish  0x00 0x04

*/

char msg_code[MESSAGELENGTH];
char content[MESSAGELENGTH];
char tempBuffer[MAX_MESSAGE_SIZE];

int client_sd;
struct sockaddr_in client_addr;


int init_server_socket();
void send_msg();
int read_msg();







void send_established();
void send_finished_ack();
void send_pattern1_ack();
void send_pattern2_ack();
void send_pattern3_ack();
void send_pattern4_ack();
void send_car_forward_ack();
void send_car_backward_ack();
void send_car_left_ack();
void send_car_right_ack();
void send_camera_forward_ack();
void send_camera_backward_ack();
void send_camera_left_ack();
void send_camera_right_ack();
void send_lift_up_ack();
void send_lift_down_ack();








/*
int main(int argc, char** argv){
	
	int server_socket = init_server_socket();

	int val=1;
	if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(long))==-1){
		perror("setsockopt");
		exit(1);
	}

				


	

		socklen_t addr_len=sizeof(client_addr);

				printf("before accept client\n");

	
		if((client_sd=accept(server_socket,(struct sockaddr *) &client_addr,&addr_len))<0)
		{
			printf("accept erro: %s (Errno:%d)\n",strerror(errno),errno);
			exit(0);
		}

		printf("after accept client\n");
	
				
	  			 
					int i ;
					for (i= 0; i < 10; ++i)
					{
							send_msg();
					}

		while(1)
		{


		int read_type=read_msg();

		switch(read_type){
			case -1: 			
			printf("Error: Received undefined message from other client.\n");
			break;

			case 1:
			printf("Command: connect request\n");
			break;

			case 2:
			printf("Command: start movement\n");
			break;

			case 3:
			printf("Command: confirm picture\n");
			break;


			case 4:
			//use pattern1
			printf("Command:  pattern1\n");
			break;


			case 5:
			printf("Command: pattern 2\n");
			break;

			case 6:
			printf("Command: pattern3\n");
			break;

			case 7:
			printf("Command: pattern4\n");
			break;

			case 8:
			printf("Command: car forward\n");
			break;

			case 9:
			printf("Command: car backward\n");
			break;

			case 10:
			printf("Command: car left\n");
			break;

			case 11:
			printf("Command: car right\n");
			break;

			case 12:
			printf("Command: camera up\n");
			break;

			case 13:
			printf("Command: camera down\n");
			break;

			case 14:
			printf("Command: camera left\n");
			break;

			case 15:
			printf("Command: camera right\n");
			break;

			case 16:
			printf("Command: lift up\n");
			break;

			case 17:
			printf("Command: lift down\n");
			break;


			default:
			printf("Error: undefined read_type.\n");
			break;


		}

		}


	close(server_socket);
	return 0;
}
*/



void send_msg()
{
			printf("Send msg to client\n");
					
					int already_sent=0;
					while(1){
						int len=send(client_sd,msg_code+already_sent,MESSAGELENGTH-already_sent,0);
						if(len<=0){
							perror("send()");
						}else{
							already_sent+=len;
						}
						if(already_sent>=MESSAGELENGTH){
							break;
						}
					}
}




void send_established()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x00;
msg_code[1]=0x01;
send_msg();
}

void send_finished_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x00;
msg_code[1]=0x04;
send_msg();
}


void send_pattern1_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x01;
send_msg();
}

void send_pattern2_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x02;
send_msg();
}

void send_pattern3_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x03;
send_msg();
}


void send_pattern4_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x04;
send_msg();
}

void send_car_forward_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x05;
send_msg();
}

void send_car_backward_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x06;
send_msg();
}

void send_car_left_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x07;
send_msg();
}


void send_car_right_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x08;
send_msg();
}


void send_camera_forward_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x09;
send_msg();
}

void send_camera_backward_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x0a;
send_msg();
}

void send_camera_left_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x0b;
send_msg();
}


void send_camera_right_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x0c;
send_msg();
}

void send_lift_up_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x0d;
send_msg();
}


void send_lift_down_ack()
{
memset(msg_code,0,MESSAGELENGTH);
msg_code[0]=0x05;
msg_code[1]=0x0e;
send_msg();
}











int read_msg()
{
	printf("Receive msg from client\n");

	memset(tempBuffer,0,MAX_MESSAGE_SIZE);
	memset(content,0,MESSAGELENGTH);
	
	int receiveByte=0;
	int alreadyReceiveByte=0;

	// receive hello message



	while(alreadyReceiveByte<MESSAGELENGTH){


		if((receiveByte = recv(client_sd,tempBuffer+alreadyReceiveByte,MAX_MESSAGE_SIZE-alreadyReceiveByte,0))<0)
		{
			 printf("Error: Couldn't receive\n");
			// exit(0);
		}

		alreadyReceiveByte+=receiveByte;

		
			if(alreadyReceiveByte>=MESSAGELENGTH)
			{
				memcpy(&content,tempBuffer,sizeof(char)*MESSAGELENGTH);
				printf("%s\n",content);
				 


				if(strcmp(content,"cr")==0)
					return 1;
				else if (strcmp(content,"sm")==0)
					return 2;
				else if (strcmp(content,"cp")==0)
					return 3;
				else if (strcmp(content,"p1")==0)
					return 4;
				else if (strcmp(content,"p2")==0)
					return 5;
				else if (strcmp(content,"p3")==0)
					return 6;
				else if (strcmp(content,"p4")==0)
					return 7;
				else if (strcmp(content,"Up")==0)
					return 8;
				else if (strcmp(content,"Do")==0)
					return 9;
				else if (strcmp(content,"Le")==0)
					return 10;
				else if (strcmp(content,"Ri")==0)
					return 11;
				else if (strcmp(content,"cU")==0)
					return 12;
				else if (strcmp(content,"cD")==0)
					return 13;
				else if (strcmp(content,"cL")==0)
					return 14;
				else if (strcmp(content,"cR")==0)
					return 15;
				else if (strcmp(content,"li")==0)
					return 16;
				else if (strcmp(content,"ld")==0)
					return 17;
				else
					return -1;


				/*switch (content[0]){

				case 0x00:
				if(content[1]==0x00)
					return 1;
				else if(content[1]==0x02)
					return 2;
				else if(content[1]==0x03)
					return 3;
				else 
					return -1;
				break;

				case 0x01:
				if(content[1]==0x01)
					return 4;
				else if(content[1]==0x02)
					return 5;
				else if(content[1]==0x03)
					return 6;
				else if(content[1]==0x04)
					return 7;
				else 
					return -1;
				break;



				case 0x02:
				if(content[1]==0x01)
					return 8;
				else if(content[1]==0x02)
					return 9;
				else if(content[1]==0x03)
					return 10;
				else if(content[1]==0x04)
					return 11;
				else 
					return -1;
				break;


				case 0x03:
				if(content[1]==0x01)
					return 12;
				else if(content[1]==0x02)
					return 13;
				else if(content[1]==0x03)
					return 14;
				else if(content[1]==0x04)
					return 15;
				else 
					return -1;
				break;


				case 0x04:
				if(content[1]==0x01)
					return 16;
				else if(content[1]==0x02)
					return 17;
				else 
					return -1;
				break;

				default:
				return -1;


				}*/

			}
			else{continue;}
	}



}










int init_server_socket(){

	int sd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_addr;

	long val=1;
	if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(long))==-1){
		perror("setsockopt");
		exit(1);
	}
	memset(&server_addr,0,sizeof(server_addr));

	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port=htons(PORT);
	
	if(bind(sd,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		printf("bind error: %s (Errno:%d)\n",strerror(errno),errno);
		exit(0);
	}
	
	if(listen(sd,2)<0){
		printf("listen error: %s (Errno:%d)\n",strerror(errno),errno);
		exit(0);
	}
		


	printf("**Server Standby** PORT:%d\n",PORT);
	return sd;

}
