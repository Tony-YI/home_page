#ifndef PPRZ_TELEMETRY_H
#define PPRZ_TELEMETRY_H

#include "Struct_Defination.h"
#include "Port_Defination.h" 


/*----------------PPRZ DEFINATION-------------------*/
#define STX  0x99
#define AC_ID 1
#define MSG_ID 14 //message.xml class = datalink <message HOVER_POINT_CHANGE/>.
#define MSG_RECV_TARGET_ID 14
#define MSG_UART_ERROR 208
#define MAX_LENGTH 512

//MSG_LENGTH = STX + LENGTH + AC_ID + MSG_ID + AC_ID + GPS_FIX + x + y + z 
#define MSG_LENGTH 1 + 1 + 1 + 1 + 1 + 1 + 4 + 4 + 4
extern uint8_t pprz_ck1,pprz_ck2;

extern struct GpsData Final_GPS_Data;
extern struct GpsData GPS1;
extern struct GpsData GPS2;
extern struct GpsData GPS3;
void SendPprzXbeeMsg(struct GpsData *gps);//CALL THIS FUNCTION if you think it's right time to send the GPS data to quadcopter

extern uint8_t pprz_buf[MAX_LENGTH];
extern uint8_t pprz_data[MAX_LENGTH];
extern int pprz_in;
extern int pprz_out;
extern unsigned char pprz_state;
extern unsigned char pprz_recv_ck1,pprz_recv_ck2,pprz_check_ok;
extern unsigned char recv_msg_id;
extern uint8_t pprz_length,pprz_index;
extern void rx_Pprz_Xbee_Msg();
extern void parse_Pprz_Xbee_Msg();
/*----------------PPRZ DEFINATION END-------------------*/


#endif //#ifndef Pprz_Telemetry_H