#include "ublox_GPS.h"
#include "PprzTelemetry.h"


struct Gps_uart gps_uart1;
struct Gps_uart gps_uart2;
struct Gps_uart gps_uart3;


void sendCmd (unsigned char len, uint8_t data[], struct Gps_uart * gps_uart)
{
    unsigned char chk1 = 0, chk2 = 0;
 
    gps_uart->gps->putc(SYNC1);
    gps_uart->gps->putc(SYNC2);
    for (unsigned char ii = 0; ii < len; ii++) {
        gps_uart->gps->putc(data[ii]);
        gps_uart->chk1 += data[ii];
        gps_uart->chk2 += chk1;
    }
    gps_uart->gps->putc(chk1);
    gps_uart->gps->putc(chk2);
}

void enableMsg(unsigned char id, bool enable,struct Gps_uart * gps_uart, int rate)
{
    if (!enable) rate = 0;
    uint8_t cmdBuf[] = {
        0x06,   // class CFG
        0x01,   // id MSG -> CFG-MSG
        8,      // length, for config message rates
        0x01,   // class,
        id,     // id,
        0x0,    // target 0 rate (DDC/I2C)
        rate,   // target 1 rate (UART1)
        0x0,    // target 2 rate (UART2)
        0x0,    // target 3 rate (USB)
        0x0,    // target 4 rate (SPI)
        0x0,    // target 5 rate (reserved)
    };
    sendCmd(sizeof(cmdBuf), cmdBuf, gps_uart);
}


void parse_GPS_Msg(struct Gps_uart* gps_uart)
{
    if (gps_uart->in != gps_uart->out)
        {
            unsigned char cc = gps_uart->buf[gps_uart->out++];
            gps_uart->out &= (MAX_LENGTH-1);
 
            switch (gps_uart->state)
            {
                case 0:    // wait for sync 1 (0xB5)
                    gps_uart->ck1 = gps_uart->ck2 = 0;
                    gps_uart->checkOk = false;
                    if (cc == SYNC1)
                        gps_uart->state++;
                    else
                    {   
                       #if CHECK_SYN1
                       pc.printf("UART %d NOT SYNC1\r\n",gps_uart->uart_id);
                       #endif
                    }
                    break;
                case 1:    // wait for sync 2 (0x62)
                    if (cc == SYNC2)
                        gps_uart->state++;
                    else
                    {
                        gps_uart->state = 0;
                        #if CHECK_SYN2
                        pc.printf("UART %d NOT SYNC2\r\n",gps_uart->uart_id);
                        #endif
                    }
                    break;
                case 2:    // wait for class code
                    gps_uart->code = cc;
                    gps_uart->ck1 += cc;
                    gps_uart->ck2 += gps_uart->ck1;
                    gps_uart->state++;
                    break;
                case 3:    // wait for Id
                    gps_uart->id = cc;
                    gps_uart->ck1 += cc;
                    gps_uart->ck2 += gps_uart->ck1;
                    gps_uart->state++;
                    break;
                case 4:    // wait for length uint8_t 1
                    gps_uart->length = cc;
                    gps_uart->ck1 += cc;
                    gps_uart->ck2 += gps_uart->ck1;
                    gps_uart->state++;
                    break;
                case 5:    // wait for length uint8_t 2
                    gps_uart->length |= (unsigned int) cc << 8;
                    gps_uart->ck1 += cc;
                    gps_uart->ck2 += gps_uart->ck1;
                    gps_uart->idx = 0;
                    gps_uart->state++;
                    if (gps_uart->length > MAX_LENGTH)
                    {
                        gps_uart->state= 0;
                        #if USE_SCREEN
                        pc.printf("Message too long\r\n");
                        #endif
                    }
                    break;
                case 6:    // wait for <length> payload uint8_ts
                    gps_uart->data[gps_uart->idx++] = cc;
                    gps_uart->ck1 += cc;
                    gps_uart->ck2 += gps_uart->ck1;
                    if (gps_uart->idx >= gps_uart->length)
                    {
                        gps_uart->state++;
                    }
                    break;
                case 7:    // wait for checksum 1
                    gps_uart->chk1 = cc;
                    gps_uart->state++;
                    break;
                case 8: {  // wait for checksum 2
                    gps_uart->chk2 = cc;
                    gps_uart->checkOk = gps_uart->ck1 == gps_uart->chk1  &&  gps_uart->ck2 == gps_uart->chk2;
                    if (!gps_uart->checkOk)
                    {
                        //led2 = 0;//led2 off when get a invalid message from GPS
                        //pc.printf("Uart %d Check sum incorrect\r\n",gps_uart->uart_id);
                    }
                    else//get a valid message from GPS
                    {
                        //led2 = 1;//led2 on when get a valid message from GPS

                        switch (gps_uart->code)
                        {
                            case 0x01:      // NAV-
                                switch (gps_uart->id)
                                {
                                    case SOL_MSG:  // NAV-SOL // we need this, ecef_pos_x, ecef_pos_y, ecef_pos_z, GPS_FIX
                                        led3 = (UINT_8(10,gps_uart) == 0x03);//get a GPS 3D-FIX
                                        #if USE_SCREEN
                                        pc.printf("GPS%d GPS_FIX %d X:%d Y:%d Z:%d ACCUR:%d\r\n",gps_uart->uart_id,UINT_32(10,gps_uart),UINT_32(12,gps_uart),UINT_32(16,gps_uart),UINT_32(20,gps_uart),UINT_32(24,gps_uart));
                                        #endif
                                        
                                        #if USE_USB
                                        pc.printf("GPS%d %d %d %d %d %d\r\n",gps_uart->uart_id,UINT_32(10,gps_uart),UINT_32(12,gps_uart),UINT_32(16,gps_uart),UINT_32(20,gps_uart),UINT_32(24,gps_uart));
                                        #endif
                                        /*
                                        pc.printf("UART %d GPS_FIX = %x\r\n", gps_uart->uart_id ,UINT_8(10,gps_uart) );
                                        pc.printf("UART %d ECEF_POS_X = %d\r\n", gps_uart->uart_id ,INT_32(12,gps_uart) );//in cm
                                        pc.printf("UART %d ECEF_POS_Y = %d\r\n", gps_uart->uart_id ,INT_32(16,gps_uart) );//in cm
                                        pc.printf("UART %d ECEF_POS_Z = %d\r\n", gps_uart->uart_id ,INT_32(20,gps_uart) );//in cm
                                        pc.printf("UART %d 3D_POS_ACCURACY = %d\r\n\r\n",gps_uart->uart_id,(uint32_t)INT_32(24,gps_uart));//Accuray
                                        */
                                        
                                        gps_uart->gps_data->iTOW = UINT_32(0,gps_uart);
                                        gps_uart->gps_data->GPS_FIX = UINT_8(10,gps_uart);
                                        gps_uart->gps_data->ecef_pos_cm_x = INT_32(12,gps_uart);
                                        gps_uart->gps_data->ecef_pos_cm_y = INT_32(16,gps_uart);
                                        gps_uart->gps_data->ecef_pos_cm_z = INT_32(20,gps_uart);
                                        gps_uart->gps_data->accuracy_3D = INT_32(24,gps_uart);
                                        gps_uart->infoOk = 1;
                                        break;
                                    default:
                                        break;
                                }
                                break;

                            default:
                                break;
                        }
                    }

                    gps_uart->state = 0;
                    break;
                }

                default:
                    break;
            }
        }
}




void rx_handler1()
{   
    //pc.printf("ENTER RX_HANDLER\r\n");
    __disable_irq();//disable interupt when receiving data from GPS1
    gps_uart1.buf[gps_uart1.in++] = gps_uart1.gps->getc();
    gps_uart1.in &= (MAX_LENGTH-1);
    __enable_irq();
    //pc.printf("EXIT RX_HANDLER\r\n");
    //pc.printf("GPS1 GOT SOMETHING\r\n");
    //led1 = !led1;
}

void rx_handler2()
{
    __disable_irq();//disable interupt when receiving data from GPS2
    gps_uart2.buf[gps_uart2.in++] = gps_uart2.gps->getc();
    gps_uart2.in &= (MAX_LENGTH-1);
    __enable_irq();
    //pc.printf("GPS2.GOT SOMETHING\r\n");
}

void rx_handler3()
{
    __disable_irq();//disable interupt when receiving data from GPS3
    gps_uart3.buf[gps_uart3.in++] = gps_uart3.gps->getc();
    gps_uart3.in &= (MAX_LENGTH - 1);
    __enable_irq();
}