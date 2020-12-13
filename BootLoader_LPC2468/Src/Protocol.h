/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Aim: Definition of the protocols
 * Author: Zhaoym
 * Created: 2008/03/31
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "board.h"
#include "aic.h"
#include "config.h"
#include "Pit.h"
#include "NorFlash.h"
#include "Twi.h"
#include "Dbgu.h"
#include "../include/math.h"
//------------------------------------------------------------------------------
//        Definitions of messages via SPI
//------------------------------------------------------------------------------
//Type of MCUCode_Download
#define Type_MCUCode_Download	0x13
//Sub_Type about MCUCode_Donload
#define Sub_Type_Download_MCUCode 0x01
#define Sub_Type_Download_MCUCode_Ack 0x01
#define Sub_Type_MCUCode 0x02
#define Sub_Type_MCUCode_Ack 0x02
#define Sub_Type_End_MCUCode 0x03
#define Sub_Type_End_MCUCode_Ack 0x03
//Sub_Type about FPGACode_Donload
#define Sub_Type_Download_FPGACode 0x04
#define Sub_Type_Download_FPGACode_Ack 0x04
#define Sub_Type_FPGACode 0x05
#define Sub_Type_FPGACode_Ack 0x05
#define Sub_Type_End_FPGACode 0x06
#define Sub_Type_End_FPGACode_Ack 0x06
//------------------------------------------------------------------------------
//Type of RNMS Configuration
#define Type_Config	0x10
//Sub_Type about RNMS Configuration
#define Sub_Type_Config_WRRU_CircumsPara	0x01
#define Sub_Type_Config_WRRU_CircumsPara_Ack 0x01
#define Sub_Type_Config_WRRU_RFPwCtrl 		0x02
#define Sub_Type_Config_WRRU_RFPwCtrl_Ack	0x02
#define Sub_Type_Config_WRRU_DbgSwh		0x03
#define Sub_Type_Config_WRRU_DbgSwh_Ack	0x03
//-------------------------------------------------------------------------------
//Type of RNMS maintenance
#define Type_maintenance	0x11
//Sub_Type about RNMS maintenance
#define Sub_Type_Reset_WRRU	0x01
#define Sub_Type_Query_WRRU_CircumsPara	0x02
#define Sub_Type_Query_WRRU_CircumsPara_Ack	0x02
#define Sub_Type_Query_WRRU_Version 0x03
#define Sub_Type_Query_WRRU_Ver_Ack 0x03
#define Sub_Type_Query_WRRU_DbgCtrl 0x04
#define Sub_Type_Query_WRRU_DbgCtrl_Ack 0x04
#define Sub_Type_Query_WRRU_RFPwCtrl 0x05
#define Sub_Type_Query_WRRU_RFPwCtrl_Ack 0x05
//------------------------------------------------------------------------------
//Type of RNMS Alarm
#define Type_Alarm 0x12
//Sub_Type about RNMS Alarm
#define Sub_Type_Alarm_WRRU_Reset 	0x01
#define Sub_Type_Alarm_WRRU_Temp		0x02
#define Sub_Type_Alarm_WRRU_Current	0x03
#define Sub_Type_Alarm_WRRU_RF		0x04
#define Sub_Type_Alarm_WRRU_10ms 		0x05
#define Sub_Type_Alarm_WRRU_LightLink 0x06
#define Sub_Type_Alarm_WRRU_FNWrong 0x07
#define Sub_Type_Alarm_recv_nopoll 0x08
#define Sub_Type_Alarm_Sync_PLL_Lost	  0x09
#define Sub_Type_MCU_DbgMsg_Report 0x10

//-------------------------------------------------------------------------------
//Type of Calibration
#define Type_Calibration 0x50
//Sub_Type about Calibration
#define Sub_Type_CalibrationData_Download 0x01
#define Sub_Type_CalibrationData_Download_Ack 0x01
#define Sub_Type_Calibration_Start 0x02
#define Sub_Type_PreCalibration_PreUplink_Report 0x03
#define Sub_Type_Sync_RxPower_Report	0x04
#define Sub_Type_Config_Sync_RxGain 0x05
#define Sub_Type_Config_RxGain 0x06
#define Sub_Type_Calibration_Dwnlink_Report 0x07
#define Sub_Type_Calibration_End 0x08

//------------------------------------------------------------------------------
//Type of DataLink Maintenance
#define Type_Data_Link_Init 0x01
#define Type_Data_Link_Init_Ack 0x01
#define Type_Poll_Mcu 0x02
#define Type_Poll_Mcu_Ack 0x02
#define Type_WRRU_No_Req 0x03
#define Type_WRRU_No_Req_Ack 0x03
#define Type_WRRU_Para_Req 0x04
#define Type_WRRU_Para_Req_Ack 0x04

//------------------------------------------------------------------------------
//Type of messages between FPGA and MCU
#define Type_FPGA_MCU 0x60
//Sub_Type about Calibration
#define Sub_Type_RFPwCtrl_Setting 0x01
#define Sub_Type_MCUConfig_Sync_TxGain 0x02
#define Sub_Type_MCUConfig_Sync_RxGain 0x03
#define Sub_Type_MCUConfig_TxGain 0x04
#define Sub_Type_MCUConfig_RxGain	0x05
#define Sub_Type_Sync_Tx_adcV 0x06
#define Sub_Type_Sync_Rx_adcV 0x07
#define Sub_Type_Sync_Entry_Open 0x08
#define Sub_Type_Sync_Entry_Close 0x09
#define Sub_Type_Sync_PLL_Report	0x10
#define Sub_Type_Sync_RF_Report	0x11
#define Sub_Type_Query_Sync_Tx_adcV	0x12
#define Sub_Type_Query_Sync_Rx_adcV	0x13

#define msg_addr_WBBU	0x02
#define msg_addr_MCU	0x01
#define msg_addr_FPGA	0x03
//for calibration download data
#define SyncTxTabLen	71//Sync_TxPower:-20~15 step 0.5
#define SyncRxTabLen	19//Sync_RxPower:-21~-12 step 0.5
typedef struct 
{

	unsigned char Freqency_No;
	unsigned char Freqency_Para[24];
	unsigned char Sync_TxGain[2];
	unsigned char Sync_RxGain[2];	
	unsigned char Tx_Gain[8];//8 channels
	unsigned char Rx_Gain[8];//8 channels
	unsigned char Delta;
	unsigned char Cable_Loss;
	unsigned char PS_Loss;
	signed char Tx_Power;
}Cal_Down_Data;
//for the original calibration data for MCU
typedef struct
{
	unsigned char Tx_Gain[8];//8 channels
	unsigned char Sync_TxGain[2];	
}Cal_MCU_Original;
//for the calculated calibration data for MCU
typedef struct
{
	unsigned char Tx_Gain[8];//8 channels
	unsigned char Sync_TxGain[2];
	signed char Sync_TxPower;
	signed char Sync_RxPower[8];//8 channels
}Cal_MCU_Calculate;
//for the calibration data MCU receive
typedef struct
{
	unsigned char Rx_Gain[8];//8 channels
	unsigned char Sync_RxGain[2];	
}Cal_WBBU;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//        Definitions of functions
//------------------------------------------------------------------------------
unsigned char MCU_Init(void);
void Link_Init_sendmsg(unsigned char msg_type);
void code_download(const unsigned char *context_buf, unsigned char context_size);
void code_download_ack(unsigned char sub_type, unsigned short Seq_No_Ack, unsigned char result);
unsigned char unresumable_alarm_check(void);
unsigned char resumable_alarm_check(void);
void alarm_packet(unsigned char sub_type,unsigned char result);
void config_processing(const unsigned char *context_buf, unsigned char context_size);
void config_ack(unsigned char sub_type, unsigned char result);
void maintain_processing(const unsigned char *context_buf, unsigned char context_size);
void maintain_ack(unsigned char sub_type);
void recvFGPA_processing(const unsigned char *context_buf, unsigned char context_size);
void sendFPGA_processing(unsigned char sub_type,unsigned char cal_no);
void calibration_processing(const unsigned char *context_buf, unsigned char context_size);
void calibration_sendtime_pro(void);
void calibration_recvtime_pro(void);
void calibration_ack(unsigned char sub_type, unsigned char result);
void calculate_Sync_TxGain(const unsigned char *context_buf, unsigned char adcV_size);
void calculate_TxGain(const unsigned char *context_buf, unsigned char adcV_size);
void calculate_MCU_original(void);
signed char SyncRxVToSyncRxP(unsigned short);
signed char SyncTxVToSyncTxP(unsigned short);
void Poll_MCU_processing(unsigned char *frame_no);
void dbgmsg_output(char *p_msg, unsigned char len);
void dbgdata_output(char *p_msg, unsigned char len);
#endif
