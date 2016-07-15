#include "PprzTelemetry.h"

struct GpsData Final_GPS_Data;
struct GpsData GPS1;
struct GpsData GPS2;
struct GpsData GPS3;

uint8_t pprz_buf[MAX_LENGTH];
uint8_t pprz_data[MAX_LENGTH];
int pprz_in = 0;
int pprz_out = 0;
unsigned char pprz_state = 0;
unsigned char pprz_recv_ck1 = 0 ,pprz_recv_ck2 = 0,pprz_check_ok;
unsigned char recv_msg_id = 0;
uint8_t pprz_ck1,pprz_ck2;
uint8_t pprz_length = 0,pprz_index = 0;



/*---------------PPRZ IMPLEMENTATION-----------------*/
#define Put1byte(data){\
    Xbee.putc(data);\
}

#define PutUint8(data){\
    pprz_ck1+=data;\
    pprz_ck2+=pprz_ck1;\
    Put1byte(data);\
}

#define Put2byteByAddr(data){\
    PutUint8(*(const uint8_t *) data);\
    PutUint8(*(((const uint8_t *) data) + 1));\
}

#define Put4byteByAddr(data){\
    Put2byteByAddr(((const uint16_t *) data));\
    Put2byteByAddr((((const uint16_t *) data) + 1));\
}

#define PutStartMessage() {\
    Put1byte(STX);\
    Put1byte(MSG_LENGTH);\
    pprz_ck1 = MSG_LENGTH;\
    pprz_ck2 = MSG_LENGTH;\
    PutUint8(AC_ID);\
    PutUint8(MSG_ID);\
}

#define PutTrailer(){Put1byte(pprz_ck1);Put1byte(pprz_ck2);}

void SendPprzXbeeMsg(struct GpsData *gps)
{   
    //pc.printf("Sending Pprz Msg\r\n");
    __disable_irq();//disable interupt when sending data
    PutStartMessage();
    //Message content Start//
    PutUint8(gps->ac_id);
    PutUint8(gps->GPS_FIX);
    Put4byteByAddr(&gps->ecef_pos_cm_x);
    Put4byteByAddr(&gps->ecef_pos_cm_y);
    Put4byteByAddr(&gps->ecef_pos_cm_z);
    //Message content End//
    PutTrailer();
    __enable_irq();
    //pc.printf("Sending Pprz Msg Done\r\n");
    return ;
}

void rx_Pprz_Xbee_Msg()
{
    //__disable_irq();//disable interupt when receiving data
    pprz_buf[pprz_in++] = Xbee.getc();
    pprz_in &= (MAX_LENGTH-1);
    led4 = !led4;
    //__enable_irq();
}

void parse_Pprz_Xbee_Msg()
{
    if(pprz_in != pprz_out)
    {
        uint8_t temp = pprz_buf[pprz_out++];
        pprz_out &= (MAX_LENGTH - 1);
        
        switch(pprz_state)
        {
            case 0: //check for PPRZ_STX
            {
                //pc.printf("case 0, temp = %02x\r\n", temp);
                
                pprz_recv_ck1 = pprz_recv_ck2 = 0;
                pprz_check_ok = false;
                if(temp == STX)
                    pprz_state++;
                break;
            }
            case 1: //Check for length, should be smaller than MAX_LENGTH;
            {
                //pc.printf("case 1, temp = %02x\r\n", temp);
                
                pprz_length = temp;
                pprz_recv_ck1 += temp;
                pprz_recv_ck2 += pprz_recv_ck1;
                if(pprz_length > MAX_LENGTH)
                {
                    pc.printf("Error:Pprz Message length doesn't match %d\r\n",pprz_length);
                    pprz_state = 0;
                }
                else
                    pprz_state++;
                    
                break;
            }
            case 2://Check for AC_ID.
            {
                //pc.printf("case 2, temp = %02x\r\n", temp);
                
                if(temp != AC_ID)
                {
                    pc.printf("Error:AC_ID doesn't match %x\r\n",temp);
                    pprz_state = 0;
                }
                else
                {
                    pprz_recv_ck1 += temp;
                    pprz_recv_ck2 += pprz_recv_ck1;
                    pprz_state++;
                }
                break;
            }
            case 3://Check for Message ID.
            {
                //pc.printf("case 3, temp = %02x\r\n", temp);
                
                if(temp != MSG_RECV_TARGET_ID && temp != MSG_UART_ERROR)
                {
                    pc.printf("Error:MSG_ID doesn't match %x\r\n",temp);
                    pprz_state = 0;
                }
                else
                {
                    recv_msg_id = temp;
                    pprz_recv_ck1 += temp;
                    pprz_recv_ck2 += pprz_recv_ck1;
                    pprz_index = 0;
                    //pc.printf("MSG_LENGTH %d\r\n",pprz_length);
                    pprz_state++;
                }
                break;
            }
            case 4://Receving payload and computing check sum;
            {
                if(pprz_index < pprz_length - 6)
                {
                       // pc.printf("case 4, temp = %02x\r\n", temp);
                    pprz_data[pprz_index++] = temp;
                    pprz_recv_ck1 += temp;
                    pprz_recv_ck2 += pprz_recv_ck1;
                }
                else
                {
                    pprz_out--;//When checking pprz_index < pprz_length -6, pprz_out adds one more time so it needs to be rectracted.
                    pprz_state++;
                }
                break;
            }
            case 5://Receving check sum1
            {
                //pc.printf("case 5, temp = %02x\r\n", temp);
                   
                if(temp != pprz_recv_ck1)
                {
                    //pc.printf("!!!!!!!\r\n|");
                    //for(int count = 0;count < pprz_length;count++)
                    //{
                    //    pc.printf("%02x ",pprz_buf[count]);
                    //}
                    //pc.printf("|\r\n");
                    pprz_state = 0;
                    pc.printf("Error:Check sum 1 incorrect: expected %x, computed %x\r\n", temp, pprz_recv_ck1);
                }
                else
                    pprz_state++;
                break;
            }
            case 6://Receving check sum2
            {
                if(temp != pprz_recv_ck2)
                {
                    pprz_state = 0;
                    pc.printf("Error:Check sum 2 incorrec: expected %x,computed %x\r\n", temp, pprz_recv_ck2);
                }
                else
                    pprz_state++;
                break;
            }
            case 7://Printting out Message Content;
            {
                switch(recv_msg_id)
                {
                    case MSG_RECV_TARGET_ID:
                    {
                        pc.printf("RECV_MSG:AC_ID %x\r\n",*(uint8_t *) &pprz_data[0]);
                        pc.printf("RECV_MSG:GPS_FIX %x\r\n",*(uint8_t *) &pprz_data[1]);
                        pc.printf("RECV_MSG:ECEF_POS_X %d\r\n",*(int32_t *) &pprz_data[2]);
                        pc.printf("RECV_MSG:ECEF_POS_Y %d\r\n",*(int32_t *) &pprz_data[6]);
                        pc.printf("RECV_MSG:ECEF_POS_Z %d\r\n\r\n",*(int32_t *) &pprz_data[10]);
                        break;
                    }
                    case MSG_UART_ERROR:
                    {
                        pc.printf("UART_ERROR:\r\n");
                        pc.printf("UART_ERROR_FRAME_CNT:%d\r\n",*(uint16_t *) &pprz_data[4]);
                        break;
                    }
                    default:
                    {
                        pc.printf("Unkonw MSG\r\n");
                    }
                }
                pprz_state = 0;
                break;
            }
            default:
            {
                pc.printf("Error:Entering Default, please check the State Machine\r\n");
                break;
            }
        } 
    }
}
/*-----------------------------------------*/