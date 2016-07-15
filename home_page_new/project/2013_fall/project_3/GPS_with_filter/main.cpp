/**
 * uBlox UBX Protocol Reader - Wayne Holder
 * Ported to mbed - Michael Shimniok
 *
 * Modefied by YI and Edward
 */


#include "mbed.h"
#include "ublox_GPS.h"
#include "PprzTelemetry.h"
#include "filter.h"
#include "Port_Defination.h"


extern struct Gps_uart gps_uart1;
extern struct Gps_uart gps_uart2;
extern struct Gps_uart gps_uart3;

int main()
{
    #if USE_SCREEN
    pc.printf("USING SCREEN\r\n");
    #endif
    init_port();//Check Port_Defination.cpp for implementation.
    
    memset(&GPS1,0,sizeof(struct GpsData));
    memset(&GPS2,0,sizeof(struct GpsData));
    //memset(&GPS3,0,sizeof(struct GpsData));
    memset(&Final_GPS_Data,0,sizeof(struct GpsData));
    
    memset(&gps_uart1,0,sizeof(struct Gps_uart));
    memset(&gps_uart2,0,sizeof(struct Gps_uart));
    //memset(&gps_uart3,0,sizeof(struct Gps_uart));
    
    GPS1.ac_id = AC_ID;
    GPS2.ac_id = AC_ID;
    //GPS3.ac_id = AC_ID;
    Final_GPS_Data.ac_id = AC_ID;
    memset(pprz_buf,0,MAX_LENGTH);
    memset(pprz_data,0,MAX_LENGTH);
    
    gps_uart1.gps = &gps1;
    gps_uart1.gps_data = &GPS1;
    gps_uart1.uart_id = 1;
    
    gps_uart2.gps = &gps2;
    gps_uart2.gps_data = &GPS2;
    gps_uart2.uart_id = 2;
    
    //gps_uart3.gps = &gps3;
    //gps_uart3.gps_data = &GPS3;
    //gps_uart3.uart_id = 3;
    
    gps1.attach( &rx_handler1 );
    gps2.attach( &rx_handler2 );
    //gps3.attach( &rx_handler3 );
    
    Xbee.attach( &rx_Pprz_Xbee_Msg);

    //initiallize LEDs
    led1 = 1;
    led2 = 0;
    led3 = 0;
    led4 = 1;
    
    // Configure GPS
    uint8_t cmdbuf[40];
    for (int i=0; i < 38; i++)
        cmdbuf[i] = 0;
    cmdbuf[0] = 0x06; // NAV-CFG5
    cmdbuf[1] = 0x24; // NAV-CFG5
    cmdbuf[2] = 0x00; // X2 bitmask
    cmdbuf[3] = 0x01; //    bitmask: dynamic model
    cmdbuf[4] = 0x04; // U1 automotive dyn model
    sendCmd(38, cmdbuf,&gps_uart1);
    sendCmd(38, cmdbuf,&gps_uart2);
    //sendCmd(38, cmdbuf,&gps_uart3);
 
    // Modify these to control which messages are sent from module
    enableMsg(SOL_MSG, true, &gps_uart1);       // Enable soluton messages
    enableMsg(SOL_MSG, true, &gps_uart2);
    //enableMsg(SOL_MSG, true, &gps_uart3);
    //init_filter();//Check filter.cpp for implementation.
    
    while (1)
    {
        //pc.printf("parsing uart1\r\n");
        parse_GPS_Msg(&gps_uart1);
        //pc.printf("parsing uart2\r\n");
        parse_GPS_Msg(&gps_uart2);
        
        //parse_GPS_Msg(&gps_uart3);
        
        if(gps_uart1.infoOk && gps_uart2.infoOk)// && gps_uart3.infoOk) 
        {
            gps_uart1.infoOk = gps_uart2.infoOk = gps_uart3.infoOk = 0;
            set_Final_Gps();
            #if USE_SCREEN
            /*
            pc.printf("Final Gps ECEF X %d ,Y %d,Z %d ,ACCURACY %d \r\n",Final_GPS_Data.ecef_pos_cm_x,Final_GPS_Data.ecef_pos_cm_y, \
        Final_GPS_Data.ecef_pos_cm_z,Final_GPS_Data.accuracy_3D);
            */
            #endif
        }
        //if(data_ready)
        SendPprzXbeeMsg(&Final_GPS_Data);
        //else
        //pc.printf("data not ready\r\n");
        //parse_Pprz_Xbee_Msg();
    }
}
 


