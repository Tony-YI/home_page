#ifndef FILTER_H
#define FILTER_H
#include "mbed.h"
#include "Struct_Defination.h"
#include "PprzTelemetry.h"

#define MAX_ERROR 1
#define MIN_ERROR 1
#define MIN_POS_ACCURACY_CM 1000000
#define MAX_TIME_ERROR 1000
#define MIN_TIME_ERROR -MAX_TIME_ERROR

/*
extern struct GpsData Final_Gps_Data;
extern struct GpsData GPS1;
extern struct GpsData GPS2;
extern struct 

extern struct GpsData inter_GPS1;
extern struct GpsData inter_GPS2;
extern struct GpsData inter_GPS3;

extern struct GpsData prev_GPS1;
extern struct GpsData prev_GPS2;
extern struct GpsData prev_GPS3;
*/

/*this function use data from GPS1 and GPS2 to create the final GPS value to be sent. */
extern void set_Final_Gps();
extern void init_filter();

#define filter_accuracy(filtered_GPS,GPS,prev_GPS) {                   \
    if(GPS.accuracy_3D < MIN_POS_ACCURACY_CM)                          \
    {                                                                  \
        memcpy(&filtered_GPS,&GPS,sizeof(struct GpsData));             \
        memcpy(&prev_GPS,&GPS,sizeof(struct GpsData));                 \
    }                                                                  \
    else                                                               \
    {                                                                  \
        memcpy(&filtered_GPS,&prev_GPS,sizeof(struct GpsData));        \
    }                                                                  \
}

#define IsBound(a) ( a < MAX_TIME_ERROR && a > MIN_TIME_ERROR)

extern uint8_t give_smallest(uint32_t accuracy[]);

extern bool data_ready;
#endif

