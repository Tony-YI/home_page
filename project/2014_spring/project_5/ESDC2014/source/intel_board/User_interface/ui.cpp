#include "ui.h"

#include <string>
#include <iostream>
#include <cstdio>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <pthread.h>

#include <sys/wait.h> 
#include <arpa/inet.h> 


#include <sys/stat.h>  

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
right    rw    11 


camera 				 
forward   up	 12
backward  do	 13
left      le	 14
right     ri	 15


lift 				
up lu         16
down ld		 17

pattern 5 p1  18
pattern 6 p2  19
pattern 7 p3  20
pattern 8 p4  21
pattern 9 p1  22
pattern 10 p2  23
pattern diy p3  24
-------------------------------------

protacal from board to  phone

connect established 0x00 0x01


pattern 1 ack p1
pattern 2 ack p2
pattern 3 ack p3 
pattern 4 ack p4

car
forward	ack	  Up
backward ack  Do
left ack 	  Le
right ack     Ri


camera
forward	ack	  cU
backward ack  cD
left ack 	  cL
right ack     cR


lift
up ack		  lu
down ack      ld

send_finished_ack  fa
send_notification  nn
*/


UI::UI()
{
	pattern=undefined;
	file_transfer_connected=0;
	transfer_port=TRANSFER_PORT;
	connection();
}



UI::~UI()
{
	close(server_socket);
}



void UI::connection()
{
	server_socket = init_server_socket();

	int val=1;
	if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(long))==-1){
		perror("setsockopt");
		exit(1);
	}

				


	struct sockaddr_in client_addr;
	socklen_t addr_len=sizeof(struct sockaddr_in);

				// printf("before accept client\n");

	
	if((client_sd=accept(server_socket,(struct sockaddr *) &client_addr,&addr_len))<0)
	{
		printf("accept erro: %s (Errno:%d)\n",strerror(errno),errno);
		exit(0);
	}

		// printf("after accept client\n");
	if(wait_command()!=connect_request)
	{
		printf("Error: It's not a connect_request \n");
		exit(0);
	};

	//get_degree();
}			


command_type UI::wait_command()
{
		int read_type=read_msg();

		switch(read_type){
			case -1: 			
			printf("Error: Received undefined message from other client.\n");
			return undefined;

			case 1:
			printf("Command: connect request\n");
			send_established();
			return connect_request;

			case 2:
			printf("Command: start movement\n");
			send_start_ack();
			return start_movement;

			case 3:
			printf("Command: confirm picture\n");
			send_comfrim_ack();
			return confirm_picture;


			case 4:
			//use pattern1
			printf("Command:  pattern1\n");
			send_pattern1_ack();
			return pattern_1;


			case 5:
			printf("Command: pattern 2\n");
			send_pattern2_ack();
			return pattern_2;

			case 6:
			printf("Command: pattern3\n");
			send_pattern3_ack();
			return pattern_3;

			case 7:
			printf("Command: pattern4\n");
			send_pattern4_ack();
			return pattern_4;

			case 8:
			printf("Command: car forward\n");
			send_car_forward_ack();
			return car_forward;

			case 9:
			printf("Command: car backward\n");
			send_car_backward_ack();
			return car_backward;

			case 10:
			printf("Command: car left\n");
			send_car_left_ack();
			return car_left;

			case 11:
			printf("Command: car right\n");
			send_car_right_ack();
			return car_right;

			case 12:
			printf("Command: camera up\n");
			send_camera_forward_ack();
			return camera_forward;

			case 13:
			printf("Command: camera down\n");
			send_camera_backward_ack();
			return camera_backward;

			case 14:
			printf("Command: camera left\n");
			send_camera_left_ack();
			return camera_left;

			case 15:
			printf("Command: camera right\n");
			send_camera_right_ack();
			return camera_right;

			case 16:
			printf("Command: lift up\n");
			send_lift_up_ack();
			return lift_up;

			case 17:
			printf("Command: lift down\n");
			send_lift_down_ack();
			return lift_down;

			case 18:
			printf("Command: pattern 5\n");
			send_pattern5_ack();
			return pattern_5;

			case 19:
			printf("Command: pattern6\n");
			send_pattern6_ack();
			return pattern_6;

			case 20:
			printf("Command: pattern7\n");
			send_pattern7_ack();
			return pattern_7;

			case 21:
			printf("Command: pattern8\n");
			send_pattern8_ack();
			return pattern_8;

			case 22:
			printf("Command: pattern9\n");
			send_pattern9_ack();
			return pattern_9;

			case 23:
			printf("Command: pattern10\n");
			send_pattern10_ack();
			return pattern_10;

			case 24:
			printf("Command: pattern_diy and waing for diy data\n");
			diy_data();
			send_patterndiy_ack();
			return pattern_diy;

			case 25:
			printf("Command: pattern_diy and waing for diy data\n");
			send_patternvideo_ack();
			return pattern_video;

			default:
			printf("Error: undefined read_type.\n");
			return undefined;


		}

}


	





void UI::send_msg()
{
	// printf("Send msg to client\n");
	int already_sent=0;
	while(already_sent<MESSAGELENGTH){
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



void UI::old_send_finished_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="fa";
	memcpy(&msg_code,&temp,2*sizeof(char));	
	printf("send_finished_ack\n");
	send_msg();
}

void UI::send_notification()
{
	memset(msg_code,0,MESSAGELENGTH);
	if(glo_pattern==pattern_3)
	{
	char temp[]="nx";
	memcpy(&msg_code,&temp,2*sizeof(char));	
	}
	else
	{
		char temp[]="nn";
		memcpy(&msg_code,&temp,2*sizeof(char));	
	}

	
	printf("send_notification\n");
	send_msg();
}



void UI::send_fetch_degree_notice()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="fd";
	memcpy(&msg_code,&temp,2*sizeof(char));	
	 
	// printf("send content:%s\n",msg_code);
	send_msg();
}

void UI::send_fetch_degree_without_confirm()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="fc";
	memcpy(&msg_code,&temp,2*sizeof(char));	
	 
	// printf("send content:%s\n",msg_code);
	send_msg();
}


void UI::send_established()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="cr";
	memcpy(&msg_code,&temp,2*sizeof(char));	
	 
	// printf("send content:%s\n",msg_code);
	send_msg();
}


void UI::send_start_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="sm";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();



	memset(tempBuffer,0,MAX_MESSAGE_SIZE);
	
	int receiveByte=0;
	int alreadyReceiveByte=0;

	// receive hello message



	while(alreadyReceiveByte<8){


		if((receiveByte = recv(client_sd,tempBuffer+alreadyReceiveByte,MAX_MESSAGE_SIZE-alreadyReceiveByte,0))<0)
		{
			 printf("Error: Couldn't receive\n");
			// exit(0);
		}

		alreadyReceiveByte+=receiveByte;

		
			if(alreadyReceiveByte>=8)
			{
				// memcpy(&content,&tempBuffer,sizeof(char)*MESSAGELENGTH);
 				// strncpy(content,tempBuffer,sizeof(char)*3);
				// int d=sizeof(char);
				// printf("%d\n",d );
				// printf("tempBuffer: %s\n", tempBuffer);
				// printf("Receive degree : %s\n",tempBuffer);
				
			}
			else{continue;}
	}

	style_choice=0;

	if((tempBuffer[0]-'0')==0)
	{
	style_choice=(tempBuffer[1]-'0');
	printf("UI::style_choice is %d\n", style_choice);
	}



	hand_choice=0;

	if((tempBuffer[2]-'0')==0)
	{
	hand_choice=(tempBuffer[3]-'0');
	printf("UI::style_choice is %d\n", hand_choice);
	}

	


	roll_degree=(tempBuffer[4]-'0')*10+(tempBuffer[5]-'0');
	roll_degree-=45;
	printf("UI::roll_degree is %d\n", roll_degree);
	

	pitch_degree=(tempBuffer[6]-'0')*10+(tempBuffer[7]-'0');
	pitch_degree-=45;
	printf("UI::roll_degree is %d\n", pitch_degree);
	
}


void UI::send_comfrim_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="cp";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}




void UI::send_pattern1_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p1";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_pattern2_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p2";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_pattern3_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p3";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}


void UI::send_pattern4_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p4";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}


void UI::send_pattern5_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p5";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_pattern6_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p6";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_pattern7_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p7";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}


void UI::send_pattern8_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p8";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}


void UI::send_pattern9_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="p9";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_pattern10_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="pa";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_patterndiy_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="pd";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_patternvideo_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="vi";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_car_forward_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="Up";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_car_backward_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="Do";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}

void UI::send_car_left_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="Le";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}


void UI::send_car_right_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="Ri";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}


void UI::send_camera_forward_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="cU";
	memcpy(&msg_code,&temp,2*sizeof(char));

	send_msg();
}

void UI::send_camera_backward_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="cD";
	memcpy(&msg_code,&temp,2*sizeof(char));

	send_msg();
}

void UI::send_camera_left_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="cL";
	memcpy(&msg_code,&temp,2*sizeof(char));

	send_msg();
}


void UI::send_camera_right_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="cR";
	memcpy(&msg_code,&temp,2*sizeof(char));

	send_msg();
}

void UI::send_lift_up_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="lu";
	memcpy(&msg_code,&temp,2*sizeof(char));

	send_msg();
}


void UI::send_lift_down_ack()
{
	memset(msg_code,0,MESSAGELENGTH);
	char temp[]="ld";
	memcpy(&msg_code,&temp,2*sizeof(char));	

	send_msg();
}


int UI::read_msg()
{
	// printf("\n");
	 printf("Receiving msg from client  ");
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
				// memcpy(&content,&tempBuffer,sizeof(char)*MESSAGELENGTH);
 				// strncpy(content,tempBuffer,sizeof(char)*MESSAGELENGTH);
				// int d=sizeof(char);
				// printf("%d\n",d );
				 // printf("tempBuffer: %s\n", tempBuffer);
				// printf("Receive msg from client: %s\n",tempBuffer);
				


				if(strcmp(tempBuffer,"cr")==0)
					return 1;
				else if (strcmp(tempBuffer,"sm")==0)
					return 2;
				else if (strcmp(tempBuffer,"cp")==0)
					return 3;
				else if (strcmp(tempBuffer,"p1")==0)
					return 4;
				else if (strcmp(tempBuffer,"p2")==0)
					return 5;
				else if (strcmp(tempBuffer,"p3")==0)
					return 6;
				else if (strcmp(tempBuffer,"p4")==0)
					return 7;
				else if (strcmp(tempBuffer,"p5")==0)
					return 18;
				else if (strcmp(tempBuffer,"p6")==0)
					return 19;
				else if (strcmp(tempBuffer,"p7")==0)
					return 20;
				else if (strcmp(tempBuffer,"p8")==0)
					return 21;
				else if (strcmp(tempBuffer,"p9")==0)
					return 22;
				else if (strcmp(tempBuffer,"pa")==0)
					return 23;
				else if (strcmp(tempBuffer,"pd")==0)
					return 24;
				else if (strcmp(tempBuffer,"Up")==0)
					return 8;
				else if (strcmp(tempBuffer,"Do")==0)
					return 9;
				else if (strcmp(tempBuffer,"Le")==0)
					return 10;
				else if (strcmp(tempBuffer,"Ri")==0)
					return 11;
				else if (strcmp(tempBuffer,"cU")==0)
					return 12;
				else if (strcmp(tempBuffer,"cD")==0)
					return 13;
				else if (strcmp(tempBuffer,"cL")==0)
					return 14;
				else if (strcmp(tempBuffer,"cR")==0)
					return 15;
				else if (strcmp(tempBuffer,"lu")==0)
					return 16;
				else if (strcmp(tempBuffer,"ld")==0)
					return 17;
				else if (strcmp(tempBuffer,"vi")==0)
					return 25;
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




int UI::update_degree()
{
	// printf("\n");

	send_fetch_degree_notice();

	memset(tempBuffer,0,MAX_MESSAGE_SIZE);
	
	int receiveByte=0;
	int alreadyReceiveByte=0;

	// receive hello message



	while(alreadyReceiveByte<3){


		if((receiveByte = recv(client_sd,tempBuffer+alreadyReceiveByte,MAX_MESSAGE_SIZE-alreadyReceiveByte,0))<0)
		{
			 printf("Error: Couldn't receive\n");
			// exit(0);
		}

		alreadyReceiveByte+=receiveByte;

		
			if(alreadyReceiveByte>=3)
			{
				// memcpy(&content,&tempBuffer,sizeof(char)*MESSAGELENGTH);
 				// strncpy(content,tempBuffer,sizeof(char)*3);
				// int d=sizeof(char);
				// printf("%d\n",d );
				// printf("tempBuffer: %s\n", tempBuffer);
				// printf("Receive degree : %s\n",tempBuffer);
				
			}
			else{continue;}
	}

	int degree_temp=0;

	degree_temp=(tempBuffer[0]-'0')*100+(tempBuffer[1]-'0')*10+(tempBuffer[2]-'0');
	printf("UI::get_degree() calculated compass degree %d\n", degree_temp);

	degree=degree_temp;
	return degree;

}




int UI::update_degree_without_confirm()
{
	// printf("\n");

	send_fetch_degree_without_confirm();

	memset(tempBuffer,0,MAX_MESSAGE_SIZE);
	
	int receiveByte=0;
	int alreadyReceiveByte=0;

	// receive hello message



	while(alreadyReceiveByte<3){


		if((receiveByte = recv(client_sd,tempBuffer+alreadyReceiveByte,MAX_MESSAGE_SIZE-alreadyReceiveByte,0))<0)
		{
			 printf("Error: Couldn't receive\n");
			// exit(0);
		}

		alreadyReceiveByte+=receiveByte;

		
			if(alreadyReceiveByte>=3)
			{
				// memcpy(&content,&tempBuffer,sizeof(char)*MESSAGELENGTH);
 				// strncpy(content,tempBuffer,sizeof(char)*3);
				// int d=sizeof(char);
				// printf("%d\n",d );
				// printf("tempBuffer: %s\n", tempBuffer);
				// printf("Receive degree : %s\n",tempBuffer);
				
			}
			else{continue;}
	}

	int degree_temp=0;

	degree_temp=(tempBuffer[0]-'0')*100+(tempBuffer[1]-'0')*10+(tempBuffer[2]-'0');
	printf("UI::get_degree() calculated compass degree %d\n", degree_temp);

	degree=degree_temp;
	return degree;

}

int UI::get_degree(){
	update_degree();
	return degree;
}




int UI::init_server_socket(){

	int sd = socket(AF_INET,SOCK_STREAM,0);
	if(sd == -1)
	{
		perror("UI::init_server_socket: socket error:");
		exit(-1);
	}
	struct sockaddr_in server_addr;

	long val=1;
	if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(long))==-1){
		perror("setsockopt");
		exit(1);
	}
	memset(&server_addr,0,sizeof(server_addr));

	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port=htons(UI_PORT);
	
	if(bind(sd,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		printf("bind error: %s (Errno:%d)\n",strerror(errno),errno);
		exit(0);
	}
	
	if(listen(sd,1)<0){
		printf("listen error: %s (Errno:%d)\n",strerror(errno),errno);
		exit(0);
	}
		


	printf("**Server Standby** PORT:%d\n",UI_PORT);
	return sd;

}







 void UI::diy_data()
{
	// printf("\n");


	memset(tempBuffer,0,MAX_MESSAGE_SIZE);
	
	int receiveByte=0;
	int alreadyReceiveByte=0;

	// receive hello message



	while(alreadyReceiveByte<12){


		if((receiveByte = recv(client_sd,tempBuffer+alreadyReceiveByte,MAX_MESSAGE_SIZE-alreadyReceiveByte,0))<0)
		{
			 printf("Error: Couldn't receive\n");
			// exit(0);
		}

		alreadyReceiveByte+=receiveByte;

		
			if(alreadyReceiveByte>=12)
			{
				// memcpy(&content,&tempBuffer,sizeof(char)*MESSAGELENGTH);
 				// strncpy(content,tempBuffer,sizeof(char)*3);
				// int d=sizeof(char);
				// printf("%d\n",d );
				// printf("tempBuffer: %s\n", tempBuffer);
				// printf("Receive degree : %s\n",tempBuffer);
				
			}
			else{continue;}
	}

	int temp=0;

	temp=(tempBuffer[0]-'0')*100+(tempBuffer[1]-'0')*10+(tempBuffer[2]-'0');
	printf("calculated ratio x %d\n", temp);
	ratiox=temp;

	temp=(tempBuffer[3]-'0')*100+(tempBuffer[4]-'0')*10+(tempBuffer[5]-'0');
	printf("calculated ratio y %d\n", temp);
	ratioy=temp;

	temp=(tempBuffer[6]-'0')*100+(tempBuffer[7]-'0')*10+(tempBuffer[8]-'0');
	printf("calculated ratio width %d\n", temp);
	ratiowidth=temp;

	temp=(tempBuffer[9]-'0')*100+(tempBuffer[10]-'0')*10+(tempBuffer[11]-'0');
	printf("calculated ratio height %d\n", temp);
	ratioheight=temp;


}

unsigned long get_file_size(const char *path);

 void UI::send_finished_ack(char *file_name_parameter)
 {
 			printf("--------------------------------------------\n");
 			printf("Enter file_transfer \n");

		 	
 			/*set up server*/
			printf("set up server for file transfering\n");


			// if(file_transfer_connected==0)
			// {
					 transfer_sd = socket(AF_INET,SOCK_STREAM,0);
				
					if(transfer_sd == -1)
					{
						perror("UI::transfer_sd: socket error:");
						exit(-1);
					}

					struct sockaddr_in transfer_server_addr;

					

					memset(&transfer_server_addr,0,sizeof(transfer_server_addr));

					transfer_server_addr.sin_family=AF_INET;
					transfer_server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
					transfer_server_addr.sin_port=htons(transfer_port);
					transfer_port++;
					printf("before bind\n");
					if(bind(transfer_sd,(struct sockaddr *) &transfer_server_addr,sizeof(transfer_server_addr))<0){
						printf("bind error: %s (Errno:%d)\n",strerror(errno),errno);
						exit(0);
					}
					
					printf("before listen\n");
					if(listen(transfer_sd,1)<0){
						printf("listen error: %s (Errno:%d)\n",strerror(errno),errno);
						exit(0);
					}

					printf("file transfering Standby\n");
					
						
					

					
			// }
				// printf("before accept client\n");
			old_send_finished_ack();


			// if(file_transfer_connected==0)
			// {
				struct sockaddr_in transfer_client_addr;
				socklen_t addr_len=sizeof(struct sockaddr_in);
				printf("before accept\n");
				if((transfer_client_sd=accept(transfer_sd,(struct sockaddr *) &transfer_client_addr,&addr_len))<0)
				{
					printf("accept erro: %s (Errno:%d)\n",strerror(errno),errno);
					exit(0);
				}
				
			// 	file_transfer_connected=1;
			// }

			printf("conneted... start transfering\n");


		char file_name[MAX_SIZE + 1]; 
        memset(file_name,0,sizeof(file_name)); 
        strcpy(file_name,file_name_parameter); 
        strncpy(file_name, buffer, 
        strlen(buffer) > MAX_SIZE ? MAX_SIZE : strlen(buffer)); 

		uint64_t filesize=(uint64_t)get_file_size(file_name);

		printf("-------------------------------\n");
		printf("filesize: %llu\n", filesize);
		printf("-------------------------------\n");
		memset(buffer,0, BUFFER_SIZE); 
		memcpy(buffer,&filesize,sizeof(filesize));


		// if (send(transfer_client_sd, buffer, sizeof(filesize), 0) < 0) 
  //        { 
  //            printf("Send File:\t%s Failed!  %s (Errno:%d\n", file_name,strerror(errno),errno);  
  //        } 



  		
  
        FILE *fp = fopen(file_name, "r"); 
        if (fp == NULL) 
        { 
            printf("File:\t%s Not Found!\n", file_name); 
        } 
        else 
        {
            memset(buffer,0, BUFFER_SIZE); 
            

            int file_block_length = 0; 

			  
			

            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
            { 
                printf("file_block_length = %d\n", file_block_length); 
  
                // 发送buffer中的字符串到new_server_socket,实际上就是发送给客户端 
                if (send(transfer_client_sd, buffer, file_block_length, 0) < 0) 
                { 
                    printf("Send File:\t%s Failed!  %s (Errno:%d\n", file_name,strerror(errno),errno); 
                    break; 
                } 
  
                memset(buffer,0,sizeof(buffer)); 
            } 

            if(fclose(fp))      
            {         
            	printf("file close error\n");        
            	exit(1);      
    		} 

            printf("File:\t%s Transfer Finished!\n", file_name); 
        } 

        close(transfer_client_sd);

      

        printf("Leave file_transfer \n");
        printf("--------------------------------------------\n");

 }

unsigned long get_file_size(const char *path)  
{  
  unsigned long filesize = -1;      
  struct stat statbuff;  
	if(stat(path, &statbuff) < 0)
	{  
		return filesize;  
	}
	else
	{  
		filesize = statbuff.st_size;  
	}  
return filesize;  
}  




