
#include "filter.h"
#include "Struct_Defination.h"

//CHECK PprzTelemetry.h for decalration//
extern struct GpsData Final_Gps_Data;
extern struct GpsData GPS1;
extern struct GpsData GPS2;
extern struct GpsData GPS3;
//------------------------------------//

struct GpsData inter_GPS1;
struct GpsData inter_GPS2;
struct GpsData inter_GPS3;

struct GpsData prev_GPS1;
struct GpsData prev_GPS2;
struct GpsData prev_GPS3;

bool data_ready = 0;
//call this function to iniliase the filter.
void init_filter()
{
    memset(&prev_GPS1,0,sizeof(struct GpsData));
    memset(&prev_GPS2,0,sizeof(struct GpsData));
    memset(&prev_GPS3,0,sizeof(struct GpsData));
    
    memset(&inter_GPS1,0,sizeof(struct GpsData));
    memset(&inter_GPS2,0,sizeof(struct GpsData));
    memset(&inter_GPS3,0,sizeof(struct GpsData));
}

void set_Final_Gps()
{
    filter_accuracy(inter_GPS1,GPS1,prev_GPS1);
    filter_accuracy(inter_GPS2,GPS2,prev_GPS2);
    //filter_accuracy(inter_GPS3,GPS3,prev_GPS3);
    //Checking the time lag, to prevent GPS module from giving lagging coordinate.. 
    bool GPS1_GPS2 = IsBound((int32_t)(inter_GPS1.iTOW - inter_GPS2.iTOW)) && (inter_GPS1.iTOW != 0) && (inter_GPS2.iTOW != 0);
    //bool GPS1_GPS3 = IsBound((int32_t)(inter_GPS1.iTOW - inter_GPS3.iTOW));
    //bool GPS2_GPS3 = IsBound((int32_t)(inter_GPS2.iTOW - inter_GPS3.iTOW));
    /*if( GPS1_GPS2 && GPS1_GPS3 && GPS2_GPS3)
    {
            //case for using three GPS values;
            uint32_t accuracy[NB_GPS] = {inter_GPS1.accuracy_3D,inter_GPS2.accuracy_3D,inter_GPS3.accuracy_3D};
            switch(give_smallest(accuracy))
            {
                case 0:
                        memcpy(&Final_GPS_Data,&inter_GPS1.accuracy_3D,sizeof(struct GpsData));
                        break;
                case 1:
                        memcpy(&Final_GPS_Data,&inter_GPS2.accuracy_3D,sizeof(struct GpsData));
                        break;
                case 2:
                        memcpy(&Final_GPS_Data,&inter_GPS3.accuracy_3D,sizeof(struct GpsData));
                        break;
                default:
                        break;
            }
    }
    else*/ if(GPS1_GPS2)
    {   //case for using GPS1 and GPS2
        if(inter_GPS1.accuracy_3D < inter_GPS2.accuracy_3D)
        {
            #if USE_SCREEN
            pc.printf("Using UART1 \r\n");
            #endif
            memcpy(&Final_GPS_Data,&inter_GPS1,sizeof(struct GpsData));
        }
        else
        {
            #if USE_SCREEN
            pc.printf("Using UART2 \r\n");
            #endif
            memcpy(&Final_GPS_Data,&inter_GPS2,sizeof(struct GpsData));
        }
        data_ready = 1;
    }/*
    else if(GPS1_GPS3)
    {   //case for using GPS1 and GPS3
         if(inter_GPS1.accuracy_3D < inter_GPS3.accuracy_3D)
            memcpy(&Final_GPS_Data,&inter_GPS1,sizeof(struct GpsData));
        else
            memcpy(&Final_GPS_Data,&inter_GPS3,sizeof(struct GpsData));
    }
    else if(GPS2_GPS3)
    {
        //case for using GPS2 and GPS3
         if(inter_GPS2.accuracy_3D < inter_GPS3.accuracy_3D)
            memcpy(&Final_GPS_Data,&inter_GPS2,sizeof(struct GpsData));
        else
            memcpy(&Final_GPS_Data,&inter_GPS3,sizeof(struct GpsData));
    }*/
    else 
    {
        #if USE_SCREEN
        pc.printf("TIME VARIES %d IsBound %02x,%02x, UART1 %d UART2 %d\r\n",inter_GPS1.iTOW - inter_GPS2.iTOW,\
        ((int32_t)inter_GPS1.iTOW - (int32_t)inter_GPS2.iTOW) < MAX_TIME_ERROR,((int32_t)inter_GPS1.iTOW - (int32_t)inter_GPS2.iTOW) > MIN_TIME_ERROR\
        ,inter_GPS1.iTOW,inter_GPS2.iTOW);
        #endif
        data_ready = 0;
    }
}

//this function give the smallest one among the array.
uint8_t give_smallest(uint32_t accuracy[])
{
    uint8_t index = 0;
    uint32_t count = 0, temp = accuracy[0];
    for(count = 0;count < NB_GPS;count++)
    {
        if(accuracy[count] < temp)
        {
            temp = accuracy[count];
            index = count;
        }
    }
    return index;
}