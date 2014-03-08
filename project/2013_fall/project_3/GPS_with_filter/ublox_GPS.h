#ifndef UBLOX_GPS_H
#define UBLOX_GPS_H

#include "Struct_Defination.h"
#include "Port_Defination.h"



extern struct GpsData GPS1;
extern struct GpsData GPS2;
#ifndef MAX_LENGTH
#define MAX_LENGTH 512
#endif

/*
extern unsigned char buf[MAX_LENGTH];
extern int in;
extern int out;
*/

#define  SYNC1       0xB5
#define  SYNC2       0x62
#define  SOL_MSG     0x06

#define INT_32(X,gps_uart)    *(int32_t *)(&(gps_uart->data[X]))
#define UINT_32(X,gps_uart)   *(uint32_t *)(&(gps_uart->data[X]))
#define INT_8(X,gps_uart)     *(int8_t *)(&(gps_uart->data[X]))
#define UINT_8(X,gps_uart)    *(uint8_t *)(&(gps_uart->data[X]))

/* 
extern unsigned char  state, lstate, code, id, chk1, chk2, ck1, ck2;
extern unsigned int  length, idx, cnt;
extern bool gpsReady;
extern bool checkOk;
 
extern unsigned char data[MAX_LENGTH];
*/
extern struct Gps_uart gps_uart1;
extern struct Gps_uart gps_uart2;

extern void sendCmd(unsigned char len, uint8_t data[], struct Gps_uart * gps_uart);

extern void enableMsg(unsigned char id, bool enable, struct Gps_uart *gps_uart, int rate = 1);

//extern void parse_GPS_Msg(struct Gps * GPS_UART,struct GpsData *GPS)
extern void parse_GPS_Msg(struct Gps_uart *gps_uart);

extern void rx_handler1();
extern void rx_handler2();
extern void rx_handler3();
#endif // #ifdef ublox_GPS.h