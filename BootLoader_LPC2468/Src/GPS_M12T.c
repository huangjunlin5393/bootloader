/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of GPS functions
 * Author: Zhaoym,(modified from MBTS_DBT_BSD_860)
 * Created: 2008/07/21
 */
 #include "GPS_M12T.h"
 #include "Usart.h"
 
unsigned char send_gps_buf[30];
unsigned char recv_gps_buf[30];
void setGpsMode_M12T(void)
{
    set_time_mode_of_GPS(0,send_gps_buf);
	/*set_pps_out_of_GPS_M12T(0,send_buf);
	set_100ppsOut_of_GPS(1,send_buf);
	set_timeoffset_of_GPS_M12T(send_buf);
	set_100ppsOut_of_GPS(0xff,send_buf);*/
}

/*Aw: set the GPS time mode;
command=0xff:polling;command=0,GPS;command=1,UTC*/
unsigned char set_time_mode_of_GPS(unsigned char command,unsigned char *msgbuf)
{
	unsigned char checksum=0;
	msgbuf[0]='@';
	msgbuf[1]='@';
	msgbuf[2]='A';
	msgbuf[3]='w';
	msgbuf[4]=command;
	checksum ^=command;
	msgbuf[5]=checksum;
	msgbuf[6]=0x0d;
	msgbuf[7]=0x0a;
	SendToUart0((char *)msgbuf,8);
	GetFromUart0((char *)recv_gps_buf,8);
	return 0;
}