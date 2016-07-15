#ifndef STRUCT_DEFINATION_H
#define STRUCT_DEFINATION_H
#include "mbed.h"



#define USE_SCREEN 1
#define USE_USB !USE_SCREEN
#define CHECK_SYN1 0
#define CHECK_SYN2 0 
#define CHECK_RX 0


#ifndef MAX_LENGTH
#define MAX_LENGTH 512
#endif

#define NB_GPS 3

struct GpsData 
{
    uint8_t ac_id;
    uint8_t GPS_FIX;
    uint32_t iTOW;
    uint32_t ecef_pos_cm_x;
    uint32_t ecef_pos_cm_y;
    uint32_t ecef_pos_cm_z;
    uint32_t accuracy_3D;
}__attribute__ ((packed));//this is very important

struct Gps_uart
{
    uint8_t uart_id;
    Serial *gps;
    struct GpsData * gps_data;
    unsigned char buf[MAX_LENGTH];
    unsigned char data[MAX_LENGTH];
    int in;
    int out;
    unsigned char state, lstate, code, id, chk1, chk2, ck1, ck2;
    unsigned int length, idx, cnt;
    bool gpsReady;
    bool checkOk;
    bool infoOk;
};

#endif