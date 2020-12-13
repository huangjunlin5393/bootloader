/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of GPS functions
 * Author: Zhaoym,(modified from MBTS_DBT_BSD_860)
 * Created: 2008/07/21
 */
#ifndef GPS_M12T_H
#define GPS_M12T_H
void setGpsMode_M12T(void);
unsigned char set_time_mode_of_GPS(unsigned char command,unsigned char *msgbuf);

#endif
