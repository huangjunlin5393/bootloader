/*--------------------------------------------------
 * Copyright: Beijing Xinwei Telecomm Technology Co.,Ltd
 * Function: contents of procotols.  

 * Author: Zhaoym
 * Created: 2008/03/31
 */
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include "Protocol.h"
#include <string.h>
#include <stdio.h>
//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------
unsigned char WRRU_No;// default value:0,1,2
unsigned char DbgCtrl=2;//0--no debug imformation output;1--to RNMS;2--to PC
unsigned char RFPwCtrl=0xff;//default value
unsigned char RFNum=8;//default value is 8
unsigned char SyncPwCtrl=0x01;//default value
extern unsigned char spi_send_buf[112];
extern unsigned char SSC_send_buf[112];
extern unsigned char SSC_recv_buf[192];
extern char dbgu_send_buf[100];
extern signed char temperature;
extern unsigned int CurrentAver;
extern signed char Temp_Max;
extern signed char Temp_Min;
extern unsigned int Current_Max;
extern unsigned int Current_Min;
extern unsigned char flag_recvsend;
extern char software_ver[4];
extern unsigned char hardware_ver;
extern unsigned char flag_recvsend;
extern unsigned char flag_extern_5ms;
extern unsigned char counter_1s;
//FOR CODEDOWNLOAD
static unsigned char codedata[512];

//for alarm
unsigned char flag_alarm_past=0;//whethere there is alarm last time
unsigned char flag_alarm_now=0;//whethere there is alarm this time
unsigned char flag_alarm_resumable=0;//flag of  resumable flag
unsigned char flag_alarm_unresumable=0;//flag of unresumable flag

unsigned char vswr_past=0;
unsigned char vswr_now=0;

//for calibration
Cal_Down_Data calibration_data;
Cal_MCU_Original cal_MCU_original;
Cal_MCU_Calculate cal_MCU_calculate;
Cal_WBBU cal_WBBU;
unsigned short SyncRxTab[SyncRxTabLen];//from small to large
unsigned short SyncTxTab[SyncTxTabLen];//from small to large
unsigned char counter_cal_10ms=0;

//for poll_mcu
unsigned short Frame_No_expect=0;

extern unsigned char SSC_send_msg(void);
extern unsigned char SSC_recv_msg(void);
extern unsigned char spi_FPGAsend_msg(void);
//------------------------------------------------------------------------------
//Aim: After initiating peripheral,initiate the datalink and application layer.
//
//Output:1-success;0-fail
//------------------------------------------------------------------------------
unsigned char MCU_Init(void)
{
	unsigned char i=0;
	unsigned char datalink_ack=0;// 1:receive data_link_ack ; 0:haven't received
	unsigned char retval=0;//return value
	while((i<3)&&(!datalink_ack))
	{
		//while(!(flag_extern_5ms&bit_sending_5ms));//waiting for transmitting timeslot //20080506 for debug
		Link_Init_sendmsg(Type_Data_Link_Init);
		timer0_start();
		SSC_recv_msg();
		while(counter_1s<=datalink_overtime)
		{
			if(flag_recvsend & bit_SSC_recv)
			{
				if(CRC_Check(SSC_recv_buf)&&(SSC_recv_buf[0]==msg_addr_MCU)&&
					(SSC_recv_buf[2]==Type_Data_Link_Init_Ack))
				{
					if(SSC_recv_buf[4]==0)//success
					{
						datalink_ack=1;
						timer0_stop();
						flag_recvsend &=~bit_SSC_recv;//clear the bit
						break;
					}
					else if(SSC_recv_buf[4]==1)//fail,send Data_Link_Init again!
					{
						flag_recvsend &=~bit_SSC_recv;//clear the bit
						break;
					}
				}
				else
				{
					//receive wrong message
					break;
				}
			}
		}
		i++;
	}
	
	if(datalink_ack==0)
	{
		AT91C_BASE_RSTC->RSTC_RCR = (0xA5000000 | AT91C_RSTC_PROCRST);//reset
	}
	else if(datalink_ack==1)
	{
		Link_Init_sendmsg(Type_WRRU_No_Req);
		SSC_recv_msg();
		while(!(flag_recvsend & bit_SSC_recv));
		if(CRC_Check(SSC_recv_buf)&&(SSC_recv_buf[2]==Type_WRRU_No_Req_Ack))
		{
			flag_recvsend &=~bit_SSC_recv;//clear the bit
			WRRU_No = SSC_recv_buf[4];
		}
		else
		{
			//receive wrong message
			flag_recvsend &=~bit_SSC_recv;//clear the bit
			return retval;
		}
		Link_Init_sendmsg(Type_WRRU_Para_Req);
		SSC_recv_msg();
		while(!(flag_recvsend & bit_SSC_recv));
		if (CRC_Check(SSC_recv_buf)&&(SSC_recv_buf[2]==Type_WRRU_Para_Req_Ack))
		{			
			memcpy(&calibration_data,&SSC_recv_buf[4],SSC_recv_buf[3]);
			sendFPGA_processing(Sub_Type_MCUConfig_Sync_TxGain,2);
			SSC_send_msg();
			sendFPGA_processing(Sub_Type_MCUConfig_Sync_RxGain,2);
			SSC_send_msg();			
			sendFPGA_processing(Sub_Type_MCUConfig_TxGain,2);
			SSC_send_msg();			
			sendFPGA_processing(Sub_Type_MCUConfig_RxGain,2);
			SSC_send_msg();
			//send PLL to SYNC??20080428 problem

			flag_recvsend &=~bit_SSC_recv;//clear the bit
			SSC_recv_msg();
		}
		else
		{
			//receive wrong message
			flag_recvsend &=~bit_SSC_recv;//clear the bit
			return retval;
		}
	}
	retval=1;
	return retval;
}

//------------------------------------------------------------------------------
//Aim: Send message to WBBU when datalink initiate and application layer initiate.
//
//------------------------------------------------------------------------------
void Link_Init_sendmsg(unsigned char msg_type)
{
	unsigned short crcvalue=0;
	memset(spi_send_buf,0x00,sizeof(spi_send_buf));
	spi_send_buf[0] = msg_addr_WBBU;//des
	spi_send_buf[1] = msg_addr_MCU;//src
	spi_send_buf[2] = msg_type;//type
	spi_send_buf[3] = 1;//length
	spi_send_buf[4] = 0;//RSV
	crcvalue=(unsigned short)dll_CalcCRC(spi_send_buf,(spi_send_buf[3]+4));
	u1to2(crcvalue,(char *)(spi_send_buf+4+spi_send_buf[3]));
	spi_FPGAsend_msg();
}

//------------------------------------------------------------------------------
//Aim: According to the message type, to do the code downloading.
//Input:
///     context_buf:the address of the context
///     context_size:the size of the context
//------------------------------------------------------------------------------
void code_download(const unsigned char *context_buf, unsigned char context_size)
{
    unsigned short seq_num=0;
    unsigned char result=0;
    unsigned char i=0;
    unsigned char write_result=0;//wrong--0;right--1
    static unsigned short index=0;//index of the array:codedata
	static unsigned short seq_expected=0;
	static unsigned char code_down_flag=0;//wrong for 1,ture for 0:the recent code_download is correct
	static unsigned char flag_code_baseaddr=0x03;//flag for new or old code

    switch(context_buf[0])//swtich according to the sub_type
    {
        case Sub_Type_Download_MCUCode: case Sub_Type_Download_FPGACode:
            index=0;
            seq_expected=0;
			code_down_flag=0;
			code_download_ack(context_buf[0], seq_expected, result);
			flag_recvsend |=bit_codedownload;
			break;
        
        case Sub_Type_MCUCode: case Sub_Type_FPGACode:
            seq_num = *(unsigned short *)(&context_buf[1]);
            if(code_down_flag==0)//if the recent code frame are all received correctly
            {
            	if(seq_num!=seq_expected)//if the received seq_no is different from seq_expected
            	{
		    		seq_num = seq_num%8;
		    		if(seq_num==7)
		    		{
			  			result = 1;
			  			code_download_ack(context_buf[0], seq_expected, result);
					}   
		    		code_down_flag=1;            
		    		return ;
   				}
            	else
            	{
                 	seq_expected++;
                 	for(i=0;i<64;i++,index++)
                 	{
                 		codedata[index]=context_buf[5+i];
                  	 }
				 	if( seq_num%8==7) //It's time to write flash and to do the ACK.
		    	 	{
						index=0;
						write_result=program_flash(codedata,512,flag_code_baseaddr);
		        		if(!write_result)
		    			{
		    	    		result=2;
		        	 	}
		        		code_download_ack(context_buf[0], 0, result);
					}
		    	 	return;						
            	 }
            }
            else
            {
            	seq_num = seq_num%8;
            	if (seq_num == 7)
            	{
            	    result=1;//the result is wrong
            	    code_download_ack(Sub_Type_MCUCode_Ack, seq_expected, result);
				}	
            }
            break;
        
        case Sub_Type_End_MCUCode: case Sub_Type_End_FPGACode:
            code_download_ack(context_buf[0], 0, result);
			flag_code_baseaddr = (flag_code_baseaddr+1)%2;
			if(context_buf[0] ==Sub_Type_End_FPGACode)
			{
				if(flag_code_baseaddr & bit_FPGACode)
				{
					flag_code_baseaddr &=~bit_FPGACode;//clear the bit
				}
				else
				{
					flag_code_baseaddr |=bit_FPGACode;//set the bit
				}
				//reset	
				AT91C_BASE_RSTC->RSTC_RCR = (0xA5000000 | AT91C_RSTC_PROCRST);
			}
			else if(context_buf[0] == Sub_Type_End_MCUCode)
			{
				if(flag_code_baseaddr & bit_MCUCode)
				{
					flag_code_baseaddr &=~bit_MCUCode;//clear the bit
				}
				else
				{
					flag_code_baseaddr |=bit_MCUCode;//set the bit
				}
			}
			flag_recvsend &=~bit_codedownload;
			break;
			
        default:
            break;	
    }
    
}

//------------------------------------------------------------------------------
//Aim: According to the message sub_type, to do the code downloading response.
//     Combine the entire ACK message.
//Input:
///     sub_type:
///     Seq_No_Ack:may not be used in some messages
///     result:1-succeed;0-failed
//------------------------------------------------------------------------------
void code_download_ack(unsigned char sub_type, unsigned short Seq_No_Ack, unsigned char result)
{
	unsigned short crcvalue=0;
	memset(spi_send_buf,0x00,sizeof(spi_send_buf));
	spi_send_buf[0] = msg_addr_WBBU;
	spi_send_buf[1] = msg_addr_MCU;
	spi_send_buf[2] = Type_MCUCode_Download;
	switch(sub_type)//swtich according to the sub_type
    {
    	case Sub_Type_Download_MCUCode_Ack: case Sub_Type_Download_FPGACode_Ack:
			spi_send_buf[3] = 2;
			spi_send_buf[4] = sub_type;
			spi_send_buf[5] = result;
    	    break;
    	case Sub_Type_MCUCode_Ack: case Sub_Type_FPGACode_Ack:
    		spi_send_buf[3] = 4;
			spi_send_buf[4] = sub_type;
			spi_send_buf[5] = result;
			u1to2(Seq_No_Ack,(char *)(spi_send_buf+6));
    	    break;
    	case Sub_Type_End_MCUCode_Ack:case Sub_Type_End_FPGACode_Ack:
    		spi_send_buf[3] = 4;
			spi_send_buf[4] = sub_type;
			spi_send_buf[5] = result;    	
    	    break;
    	default:
    	    return;
    }
	crcvalue=(unsigned short)dll_CalcCRC(spi_send_buf,(spi_send_buf[3]+4));
	u1to2(crcvalue,(char *)(spi_send_buf+4+spi_send_buf[3]));
	flag_recvsend |=bit_SPI_send_reply;//set the bit
}

//------------------------------------------------------------------------------
//Aim: Check the resumable alarm(including RF vswr alarm!) and combine the alarm 
//     packet.
//Input:None
//Output:1=successful;0-failure
//------------------------------------------------------------------------------
unsigned char resumable_alarm_check(void)
{
	unsigned char alarm_result;
	unsigned char retvalue=0;
	if(flag_alarm_resumable)
	{
		retvalue=1;
		if(flag_alarm_resumable&bit_alarm_temp)
		{
			if(flag_alarm_now&bit_alarm_temp)
			{
				alarm_result=1;
				flag_alarm_past |= bit_alarm_temp;//set the bit
			}
			else
			{
				alarm_result=0;
				flag_alarm_past &=~bit_alarm_temp;//clear the bit
			}
			alarm_packet(Sub_Type_Alarm_WRRU_Temp,alarm_result);
			flag_alarm_resumable &=~bit_alarm_temp;
			return retvalue;			
		}
		else if(flag_alarm_resumable&bit_alarm_current)
		{
			if(flag_alarm_now&bit_alarm_current)
			{
				alarm_result=1;
				flag_alarm_past |= bit_alarm_current;//set the bit
			}
			else
			{
				alarm_result=0;
				flag_alarm_past &=~ bit_alarm_current;//clear the bit
			}
			alarm_packet(Sub_Type_Alarm_WRRU_Current,alarm_result);
			flag_alarm_resumable &=~bit_alarm_current;
			return retvalue;
		}
		else if(flag_alarm_resumable&bit_alarm_10ms)
		{
			if(flag_alarm_now&bit_alarm_10ms)
			{
				alarm_result=1;
				flag_alarm_past |= bit_alarm_10ms;//set the bit				
			}
			else
			{
				alarm_result=0;
				flag_alarm_past &=~ bit_alarm_10ms;//clear the bit
			}
			alarm_packet(Sub_Type_Alarm_WRRU_10ms,alarm_result);
			flag_alarm_resumable &=~bit_alarm_10ms;
			return retvalue;
		}
		else if(flag_alarm_resumable&bit_alarm_LightLink)
		{
			if(flag_alarm_now&bit_alarm_LightLink)
			{
				alarm_result=1;
				flag_alarm_past |= bit_alarm_LightLink;//set the bit					
			}
			else
			{
				alarm_result=0;
				flag_alarm_past &=~ bit_alarm_LightLink;//clear the bit
			}
			alarm_packet(Sub_Type_Alarm_WRRU_LightLink,alarm_result);
			flag_alarm_resumable &=~bit_alarm_LightLink;
			return retvalue;
		}
		else if(flag_alarm_resumable&bit_alarm_SyncPLL)
		{
			if(flag_alarm_now&bit_alarm_SyncPLL)
			{
				alarm_result=1;
				flag_alarm_past |= bit_alarm_SyncPLL;//set the bit									
			}
			else
			{
				alarm_result=0;
				flag_alarm_past &=~ bit_alarm_SyncPLL;//clear the bit				
			}
			alarm_packet(Sub_Type_Alarm_Sync_PLL_Lost,alarm_result);
			flag_alarm_resumable &=~bit_alarm_SyncPLL;
			return retvalue;
		}
		else if(flag_alarm_resumable&bit_alarm_RF)
		{
			alarm_result=1;//Reserve value is 1
			vswr_past=vswr_now;
			alarm_packet(Sub_Type_Alarm_WRRU_RF,alarm_result);
			flag_alarm_resumable &=~bit_alarm_RF;
			return retvalue;
		}
		else
		{
			retvalue=0;
		}
	}
	return retvalue;
}

//------------------------------------------------------------------------------
//Aim: Check the unresumable alarm and combine the alarm packet.
//Input:None
//Output:1=successful;0-failure
//------------------------------------------------------------------------------
unsigned char unresumable_alarm_check(void)
{
	unsigned char alarm_result=1;//Alarm_Type is 1:unresumable alarm
	unsigned char retvalue=0;
	if(flag_alarm_unresumable)
	{
		retvalue=1;
		if(flag_alarm_unresumable&bit_alarm_Reset)
		{
			alarm_packet(Sub_Type_Alarm_WRRU_Reset,alarm_result);
			flag_alarm_unresumable &=~bit_alarm_Reset;		
			return retvalue;
		}
		else if(flag_alarm_unresumable&bit_alarm_FNWrong)
		{
			alarm_packet(Sub_Type_Alarm_WRRU_FNWrong,alarm_result);
			flag_alarm_resumable &=~bit_alarm_FNWrong;
			return retvalue;
		}
		else if(flag_alarm_resumable&bit_alarm_recv_nopoll)
		{
			alarm_packet(Sub_Type_Alarm_recv_nopoll,alarm_result);
			flag_alarm_resumable &=~bit_alarm_recv_nopoll;
			return retvalue;
		}
		else if(flag_alarm_resumable&bit_alarm_dbgmsg)
		{
			alarm_packet(Sub_Type_MCU_DbgMsg_Report,alarm_result);
			flag_alarm_resumable &=~bit_alarm_dbgmsg;
			return retvalue;
		}
		else
		{
			retvalue=0;
		}
	}
	return retvalue;
}

//------------------------------------------------------------------------------
//Aim: According to the message sub_type, to do resumable alarm message combining.
//     Combine the entire alarm ACK message.
//Input:
///     sub_type:
///     result:1-alarm;0-recover
//------------------------------------------------------------------------------
void alarm_packet(unsigned char sub_type,unsigned char result)
{
	unsigned short crcvalue=0;
	memset(spi_send_buf,0x00,sizeof(spi_send_buf));
	spi_send_buf[0] = msg_addr_WBBU;
	spi_send_buf[1] = msg_addr_MCU;
	spi_send_buf[2] = Type_Alarm;
	spi_send_buf[4] = sub_type;
	spi_send_buf[5] = result;
	switch(sub_type)//swtich according to the sub_type
    {
		//resumable alarm:Temp,Current,10ms,LightLink,SyncPLL_Lost
		case Sub_Type_Alarm_WRRU_Temp:
			spi_send_buf[3] = 3;
			spi_send_buf[6] = temperature;
    	    break;
    	case Sub_Type_Alarm_WRRU_Current:
    		spi_send_buf[3] = 6;
			u1to4(CurrentAver,(char *)(spi_send_buf+6));
    	    break;
    	case Sub_Type_Alarm_WRRU_10ms:
    		spi_send_buf[3] = 2;   	
    	    break;
		case Sub_Type_Alarm_WRRU_LightLink:
    		spi_send_buf[3] = 2;
			break;
		case Sub_Type_Alarm_Sync_PLL_Lost:
    		spi_send_buf[3] = 2;
			break;

		//unresumable alarm:Reset,FNWrong,recv_nopoll
		case Sub_Type_Alarm_WRRU_Reset:
    		spi_send_buf[3] = 3;
			spi_send_buf[6] = 0x01;//reset reason??20080425 problem
			break;
		case Sub_Type_Alarm_WRRU_FNWrong:
    		spi_send_buf[3] = 2;
			break;
		case Sub_Type_Alarm_recv_nopoll:
    		spi_send_buf[3] = 2;
			break;

		//particularly for RF
		case Sub_Type_Alarm_WRRU_RF:
    		spi_send_buf[3] = 2;
    		spi_send_buf[5] = vswr_now;
			break;
		//particularly for DbgMsg
		case Sub_Type_MCU_DbgMsg_Report:
			spi_send_buf[3] = strlen(dbgu_send_buf)+2;
			memcpy(&spi_send_buf[6],dbgu_send_buf,strlen(dbgu_send_buf));
			memset(dbgu_send_buf,0x00,sizeof(dbgu_send_buf));
			break;
    	default:
    	    return;
    }
	crcvalue=(unsigned short)dll_CalcCRC(spi_send_buf,(spi_send_buf[3]+4));
	u1to2(crcvalue,(char *)(spi_send_buf+4+spi_send_buf[3]));
}

//------------------------------------------------------------------------------
//Aim: According to the message type, to do the configure processing.
//Input:
///     context_buf:the address of the context
///     context_size:the size of the context
//------------------------------------------------------------------------------
void config_processing(const unsigned char *context_buf, unsigned char context_size)
{
	unsigned char result=0;
	unsigned char i=0;
	switch(context_buf[0])//swtich according to the sub_type
    {
        case Sub_Type_Config_WRRU_CircumsPara:
            Temp_Max = context_buf[1];
			Temp_Min = context_buf[2];
			Current_Max = u4to1((unsigned char *)&context_buf[3]);
			Current_Min = u4to1((unsigned char *)&context_buf[7]);			
            config_ack(Sub_Type_Config_WRRU_CircumsPara_Ack,result);
			break;
		case Sub_Type_Config_WRRU_RFPwCtrl:
			RFPwCtrl = context_buf[1];
			SyncPwCtrl = context_buf[2];
			RFNum=0;
			for(i=0;i<8;i++)
			{
				if(RFPwCtrl&(0x01<<i))
				{
					RFNum++;
				}
			}
			config_ack(Sub_Type_Config_WRRU_RFPwCtrl_Ack,result);
		    break;
		case Sub_Type_Config_WRRU_DbgSwh:
			DbgCtrl = context_buf[1];
			config_ack(Sub_Type_Config_WRRU_DbgSwh_Ack,result);
		    break;
		default:
			return;
	}
		
}

//------------------------------------------------------------------------------
//Aim: According to the message sub_type, to do the configure response.
//     Combine the entire ACK message.
//Input:
///     sub_type:
///     result:1-succeed;0-failed
//------------------------------------------------------------------------------
void config_ack(unsigned char sub_type, unsigned char result)
{
	unsigned short crcvalue=0;
	memset(spi_send_buf,0x00,sizeof(spi_send_buf));
	spi_send_buf[0] = msg_addr_WBBU;
	spi_send_buf[1] = msg_addr_MCU;
	spi_send_buf[2] = Type_Config;
	spi_send_buf[3] = 2;
	spi_send_buf[4] = sub_type;
	spi_send_buf[5] = result;
	crcvalue=(unsigned short)dll_CalcCRC(spi_send_buf,(spi_send_buf[3]+4));
	u1to2(crcvalue,(char *)(spi_send_buf+4+spi_send_buf[3]));
	flag_recvsend |=bit_SPI_send_reply;//set the bit
}

//------------------------------------------------------------------------------
//Aim: According to the message type, to do the maintain processing.
//Input:
///     context_buf:the address of the context
///     context_size:the size of the context
//------------------------------------------------------------------------------
void maintain_processing(const unsigned char *context_buf, unsigned char context_size)
{
	switch(context_buf[0])//swtich according to the sub_type
    {
        case Sub_Type_Reset_WRRU:
			AT91C_BASE_RSTC->RSTC_RCR = (0xA5000000 | AT91C_RSTC_PROCRST);//reset
			break;
		case Sub_Type_Query_WRRU_CircumsPara:
			monitor_current();
			monitor_temperature();
			maintain_ack(Sub_Type_Query_WRRU_CircumsPara_Ack);
		    break;
		case Sub_Type_Query_WRRU_Version:
			maintain_ack(Sub_Type_Query_WRRU_Ver_Ack);
		    break;
		case Sub_Type_Query_WRRU_DbgCtrl:
			maintain_ack(Sub_Type_Query_WRRU_DbgCtrl_Ack);
			break;
		case Sub_Type_Query_WRRU_RFPwCtrl:
			maintain_ack(Sub_Type_Query_WRRU_RFPwCtrl_Ack);
			break;			
		default:
			return;
	}
		
}

//------------------------------------------------------------------------------
//Aim: According to the message sub_type, to do maintenance reply message combining.
//     Combine the entire alarm ACK message.
//Input:
///     sub_type;
//------------------------------------------------------------------------------
void maintain_ack(unsigned char sub_type)
{
	unsigned short crcvalue=0;
	memset(spi_send_buf,0x00,sizeof(spi_send_buf));
	spi_send_buf[0] = msg_addr_WBBU;
	spi_send_buf[1] = msg_addr_MCU;
	spi_send_buf[2] = Type_maintenance;
	spi_send_buf[4] = sub_type;

	switch(sub_type)//swtich according to the sub_type
    {
    	case Sub_Type_Query_WRRU_CircumsPara_Ack:
			spi_send_buf[3] = 6;
			spi_send_buf[5] = temperature;
			spi_send_buf[6] = CurrentAver;
			u1to4(CurrentAver,(char *)(spi_send_buf+6));
    	    break;
    	case Sub_Type_Query_WRRU_Ver_Ack:
    		//??20080425 problem:FPGA software version
    		//??20080425 problem:Sync software version
    		memcpy(&spi_send_buf[5],software_ver,4);
			spi_send_buf[17] = hardware_ver;
    	    break;
    	case Sub_Type_Query_WRRU_DbgCtrl_Ack:
    		spi_send_buf[5] = DbgCtrl;   	
    	    break;
		case Sub_Type_Query_WRRU_RFPwCtrl_Ack:
    		spi_send_buf[5] = RFPwCtrl;
			spi_send_buf[6] = SyncPwCtrl;
			break;
    	default:
    	    return;
    }
	crcvalue=(unsigned short)dll_CalcCRC(spi_send_buf,(spi_send_buf[3]+4));
	u1to2(crcvalue,(char *)(spi_send_buf+4+spi_send_buf[3]));
	flag_recvsend |=bit_SPI_send_reply;//set the bit
}

//------------------------------------------------------------------------------
//Aim: After receiveing message from FPGA(throughSSC),to do the processing.
//Input:
///     context_buf:the address of the context
///     context_size:the size of the context
//------------------------------------------------------------------------------
void recvFGPA_processing(const unsigned char * context_buf, unsigned char context_size)
{
	switch(context_buf[0])
	{
		case Sub_Type_Sync_Tx_adcV:
			calculate_Sync_TxGain((context_buf+1), context_size-1);
			sendFPGA_processing(Sub_Type_MCUConfig_Sync_TxGain,1);
			SSC_send_msg();		
			break;
		case Sub_Type_Sync_Rx_adcV:
			calculate_TxGain((context_buf+1), context_size-1);
			sendFPGA_processing(Sub_Type_MCUConfig_TxGain,1);
			SSC_send_msg();
			calibration_ack(Sub_Type_Sync_RxPower_Report,0);
			spi_FPGAsend_msg();
			//especially in TX+0 timeslot
			if(counter_cal_10ms==3)
			{
				calibration_ack(Sub_Type_PreCalibration_PreUplink_Report,0);
				spi_FPGAsend_msg();
			}
			break;
		case Sub_Type_Sync_PLL_Report:
			flag_alarm_resumable |=bit_alarm_SyncPLL;//assume alarm at first
			if(!context_buf[1])
			{
				flag_alarm_now &=~bit_alarm_SyncPLL;//clear the bit
				// no alarm this time and no alarm last time
				if(!(flag_alarm_past & bit_alarm_SyncPLL))
				{
					flag_alarm_resumable &=~bit_alarm_SyncPLL;//clear the bit
				}		
			}
			else
			{
				flag_alarm_now |= bit_alarm_SyncPLL;//set the bit
			}
			break;
		case Sub_Type_Sync_RF_Report:
			vswr_now = context_buf[1];
			if(vswr_now|vswr_past)
			{
				flag_alarm_resumable |=bit_alarm_RF;//RF alarm
			}
			else
			{
				flag_alarm_resumable &=~bit_alarm_RF;//no RF alarm
			}
			break;
		default:
			return;
	}
}

//------------------------------------------------------------------------------
//Aim: According to the message type, to do the calibration processing.
//Input:
///     context_buf:the address of the context
///     context_size:the size of the context
//------------------------------------------------------------------------------
void calibration_processing(const unsigned char *context_buf, unsigned char context_size)
{
	unsigned char result=0;
	unsigned char cal_no=0;
	switch(context_buf[0])
	{
		case Sub_Type_CalibrationData_Download:
			memcpy(&calibration_data,&context_buf[1],context_size-1);
			cal_no=2;
			sendFPGA_processing(Sub_Type_MCUConfig_Sync_TxGain,cal_no);
			SSC_send_msg();
			sendFPGA_processing(Sub_Type_MCUConfig_Sync_RxGain,cal_no);
			SSC_send_msg();			
			sendFPGA_processing(Sub_Type_MCUConfig_TxGain,cal_no);
			SSC_send_msg();			
			sendFPGA_processing(Sub_Type_MCUConfig_RxGain,cal_no);
			SSC_send_msg();
			//send PLL to SYNC??20080428 problem

			//calculate the original calibration data
			calculate_MCU_original();
			calibration_ack(Sub_Type_CalibrationData_Download_Ack,result);
			break;
		case Sub_Type_Calibration_Start:
			flag_recvsend |=bit_calibration;
			counter_cal_10ms=0;//start counter 10ms
			cal_no=0;
			sendFPGA_processing(Sub_Type_MCUConfig_Sync_TxGain,cal_no);
			SSC_send_msg();

			break;
		case Sub_Type_Config_Sync_RxGain:
			cal_no=0;
			cal_WBBU.Sync_RxGain[0]=context_buf[1];
			cal_WBBU.Sync_RxGain[1]=context_buf[2];			
			sendFPGA_processing(Sub_Type_MCUConfig_Sync_RxGain,cal_no);
			SSC_send_msg();
			break;
		case Sub_Type_Config_RxGain:
			cal_no=0;
			memcpy(cal_WBBU.Rx_Gain,&context_buf[1],8);
			sendFPGA_processing(Sub_Type_MCUConfig_RxGain,cal_no);
			SSC_send_msg();
			break;
		default:
			return;
	}
}

//------------------------------------------------------------------------------
//Aim: During calibration,there are time driving message as well as event driving
//     This function focuses on transmitting 5ms timeslot message processing.
//------------------------------------------------------------------------------
void calibration_sendtime_pro(void)
{
	switch(counter_cal_10ms)
	{
	 	case 1:
			sendFPGA_processing(Sub_Type_Query_Sync_Tx_adcV,0);
			SSC_send_msg();
			break;
		case 2:
			sendFPGA_processing(Sub_Type_MCUConfig_TxGain,0);
			SSC_send_msg();
			sendFPGA_processing(Sub_Type_Query_Sync_Tx_adcV,0);
			SSC_send_msg();
		    break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
	 }
}

//------------------------------------------------------------------------------
//Aim: During calibration,there are time driving message as well as event driving
//	   This function focuses on receiving 5ms timeslot message processing.
//------------------------------------------------------------------------------
void calibration_recvtime_pro(void)
{
	switch(counter_cal_10ms)
	{
	 	case 1:
			break;
		case 2:
			sendFPGA_processing(Sub_Type_Sync_Entry_Open,0);
			SSC_send_msg();
			sendFPGA_processing(Sub_Type_Query_Sync_Rx_adcV,0);
			SSC_send_msg();
		    break;
		case 3:
			sendFPGA_processing(Sub_Type_Query_Sync_Rx_adcV,0);
			SSC_send_msg();
			break;
		case 4:
			calibration_ack(Sub_Type_CalibrationData_Download_Ack,0);
			SSC_send_msg();
			break;
		case 5:
			sendFPGA_processing(Sub_Type_Sync_Entry_Close,0);
			SSC_send_msg();
			calibration_ack(Sub_Type_Calibration_End,0);
			SSC_send_msg();
			flag_recvsend &=~bit_calibration;//calibration end
			break;
	 }	    
}
//------------------------------------------------------------------------------
//Aim: According to the message sub_type, to do the calibration response.
//     Combine the entire ACK message.
//Input:
///     sub_type:
///     result:1-succeed;0-failed
//------------------------------------------------------------------------------
void calibration_ack(unsigned char sub_type, unsigned char result)
{
	unsigned short crcvalue=0;
	while(AT91C_BASE_SPI->SPI_TCR);//wait untill last transmit complete
	spi_send_buf[0] = msg_addr_WBBU;//des
	spi_send_buf[1] = msg_addr_MCU;//src
	spi_send_buf[2] = Type_Calibration;//type
	spi_send_buf[4] = sub_type;//sub_type
	spi_send_buf[5] = result;//result
	switch(sub_type)
	{
		case Sub_Type_CalibrationData_Download_Ack:
			spi_send_buf[3] = 2;//length
			break;
		case Sub_Type_PreCalibration_PreUplink_Report:
			spi_send_buf[3] = 5;//length	
			//Sync_TxPower
			spi_send_buf[6] = (unsigned char)cal_MCU_calculate.Sync_TxPower;
			memcpy(&spi_send_buf[7],cal_MCU_calculate.Sync_TxGain,2);
			break;
		case Sub_Type_Sync_RxPower_Report:
			spi_send_buf[3] = 50;//length	
			//Sync_RxPower,48 samples have been evaluated in calculate_TxGain function
			break;
		case Sub_Type_Calibration_Dwnlink_Report:
			spi_send_buf[3] = 18;//length			
			memcpy(&spi_send_buf[6],cal_MCU_calculate.Tx_Gain,8);
			break;
		case Sub_Type_Calibration_End:
			spi_send_buf[3] = 2;//length
			break;
		default:
			return;
	}
	crcvalue=(unsigned short)dll_CalcCRC(spi_send_buf,(spi_send_buf[3]+4));
	u1to2(crcvalue,(char *)(spi_send_buf+4+spi_send_buf[3]));
	flag_recvsend |=bit_SPI_send_reply;//set the bit
}

//------------------------------------------------------------------------------
//Aim: According to the message sub_type, combine the message sending to FPGA
//     (through SSC).
//Input:
///     sub_type;cal_no:0--original data,1-calculate data,2-calibration download data
//------------------------------------------------------------------------------
void sendFPGA_processing(unsigned char sub_type,unsigned char cal_no)
{
	while(AT91C_BASE_SSC->SSC_TCR);//wait untill last transmit complete
	memset(SSC_send_buf,0x00,sizeof(SSC_send_buf));
	SSC_send_buf[0] = msg_addr_FPGA;//des
	SSC_send_buf[1] = msg_addr_MCU;//src
	SSC_send_buf[2] = Type_FPGA_MCU;//type
	SSC_send_buf[4] = sub_type;//sub_type

	switch(sub_type)
	{
		case Sub_Type_RFPwCtrl_Setting:
			SSC_send_buf[3] = 3;//length
			SSC_send_buf[4] = RFPwCtrl;
			SSC_send_buf[5] = SyncPwCtrl;
			break;
		case Sub_Type_MCUConfig_Sync_TxGain:
			SSC_send_buf[3] = 3;//length
			if(cal_no==2)
			{
				SSC_send_buf[5] = calibration_data.Sync_TxGain[0];
				SSC_send_buf[6] = calibration_data.Sync_TxGain[1];
			}
			else if(cal_no==1)
			{
				SSC_send_buf[5] = cal_MCU_calculate.Sync_TxGain[0];
				SSC_send_buf[6] = cal_MCU_calculate.Sync_TxGain[1];				
			}
			else if(cal_no==0)
			{
				SSC_send_buf[5] = cal_MCU_original.Sync_TxGain[0];
				SSC_send_buf[6] = cal_MCU_original.Sync_TxGain[1];				
			}			
			break;
		case Sub_Type_MCUConfig_Sync_RxGain:
			SSC_send_buf[3] = 3;//length
			if(cal_no==2)
			{
				SSC_send_buf[5] = calibration_data.Sync_RxGain[0];
				SSC_send_buf[6] = calibration_data.Sync_RxGain[1];
			}
			else if((cal_no==1)||(cal_no==0))
			{
				SSC_send_buf[5] = cal_WBBU.Sync_RxGain[0];
				SSC_send_buf[6] = cal_WBBU.Sync_RxGain[1];				
			}			
			break;
		case Sub_Type_MCUConfig_TxGain:
			SSC_send_buf[3] = 9;//length
			if(cal_no==2)
			{
				memcpy(&SSC_send_buf[5],&calibration_data.Tx_Gain[0],8);
			}
			else if(cal_no==1)
			{
				memcpy(&SSC_send_buf[5],&cal_MCU_calculate.Tx_Gain[0],8);				
			}
			else if(cal_no==0)
			{
				memcpy(&SSC_send_buf[5],&cal_MCU_original.Tx_Gain[0],8);
			}			
			break;
		case Sub_Type_MCUConfig_RxGain:
			SSC_send_buf[3] = 9;//length
			if(cal_no==2)
			{
				memcpy(&SSC_send_buf[5],&calibration_data.Rx_Gain[0],8);
			}
			else if((cal_no==1)||(cal_no==0))
			{
				memcpy(&SSC_send_buf[5],&cal_WBBU.Rx_Gain[0],8);
			}
			break;
		case Sub_Type_Sync_Entry_Open:
			SSC_send_buf[3] = 1;//length
			break;
		case Sub_Type_Sync_Entry_Close:
			SSC_send_buf[3] = 1;//length
			break;
		case Sub_Type_Query_Sync_Tx_adcV:
			SSC_send_buf[3] = 1;//length
			break;
		case Sub_Type_Query_Sync_Rx_adcV:
			SSC_send_buf[3] = 1;//length
			break;
		default:
			return;
	}
}

//------------------------------------------------------------------------------
//Aim: Sync_Tx_adcV->search Sync_TxPower in EEPROM(SYNC)->calculate Sync_TxGain
//	   Calculate twice during calibration.
//Input:
///     *context_buf:pointer to the adcV sample;adcV_size:length of total sample
//------------------------------------------------------------------------------
void calculate_Sync_TxGain(const unsigned char *context_buf, unsigned char adcV_size)
{
	unsigned char i;
	unsigned short syncTxV[9];
	signed char syncTxPower[9];
	unsigned char expect_power;
	unsigned char Sync_TxGain,Sync_TxGain_original;
	//search in EEPROM
	for(i=0;i<adcV_size/2;i++)
	{	
		syncTxV[i] = *(unsigned short *)(context_buf+i*2);
		syncTxPower[i] = SyncTxVToSyncTxP(syncTxV[i]);
	}
	//deal with samples to get Sync_TxPower in cal_MCU_calculate struct
	
	//calculate Sync_TxGain
	expect_power = -52+calibration_data.Cable_Loss+calibration_data.Delta+calibration_data.PS_Loss;
	expect_power = expect_power*2;
	if(cal_MCU_original.Sync_TxGain[1]==0)
	{
		Sync_TxGain_original = cal_MCU_original.Sync_TxGain[0];
	}
	else if(cal_MCU_original.Sync_TxGain[0]==63)
	{
		Sync_TxGain_original = 63+cal_MCU_original.Sync_TxGain[1];		
	}
	Sync_TxGain = expect_power-cal_MCU_calculate.Sync_TxPower+Sync_TxGain_original;
	if(Sync_TxGain<=63)
	{
		cal_MCU_calculate.Sync_TxGain[0]=Sync_TxGain;
		cal_MCU_calculate.Sync_TxGain[1]=0;
	}
	else if(Sync_TxGain<63*2)
	{
		cal_MCU_calculate.Sync_TxGain[0]=63;
		
		cal_MCU_calculate.Sync_TxGain[1]=Sync_TxGain-63;
	}
	else
	{
		//wrong
		cal_MCU_calculate.Sync_TxGain[0]=0;
		cal_MCU_calculate.Sync_TxGain[1]=0;		
	}
}

//------------------------------------------------------------------------------
//Aim: Sync_Rx_adcV->search Sync_RxPower in EEPROM(SYNC)->calculate TxGain
//	   Calculate twice during calibration.
//Input:
///     *context_buf:pointer to the adcV sample;adcV_size:length of total sample
//------------------------------------------------------------------------------
void calculate_TxGain(const unsigned char *context_buf, unsigned char adcV_size)
{
	unsigned char i,j;
	unsigned short syncRxV[8][6];
	signed char syncRxPower[8][6];
	unsigned char expect_power;

	//search in EEPROM
	for(i=0;i<8;i++)
	{	
		
		for(j=0;j<adcV_size/2/8;j++)
		{
			syncRxV[i][j] = *(unsigned short *)(context_buf+i*2*8+j*2);
			syncRxPower[i][j] = SyncRxVToSyncRxP(syncRxV[i][j]);
			//send Sync_RxPower_Report to WBBU immediately
			*(spi_send_buf+6+i*8+j) = (unsigned char)syncRxPower[i][j];
		}
	}
	//deal with samples to get Sync_RxPower[8] in cal_MCU_calculate struct

	//calculate TxGain
	expect_power = calibration_data.Tx_Power-calibration_data.Cable_Loss-calibration_data.PS_Loss;
	expect_power = expect_power*2;
	for(i=0;i<8;i++)
	{	
		cal_MCU_calculate.Tx_Gain[i]=expect_power-cal_MCU_calculate.Sync_RxPower[i]+cal_MCU_original.Tx_Gain[i];
	}	
}

//------------------------------------------------------------------------------
//Aim: Calculate MCU original calibration data:Sync_TxGain and Tx_Gain.
//Input:None.
//------------------------------------------------------------------------------
void calculate_MCU_original(void)
{
	unsigned char sync_txgain,txgain;
	//default value:when tx_power=33,PS_Loss=28,Cable_loss=0,sync_txgain=16*2
	sync_txgain=2*(16+(calibration_data.Delta+calibration_data.Cable_Loss+calibration_data.PS_Loss-0-0-28));
	if(sync_txgain<=63)
	{
		cal_MCU_original.Sync_TxGain[0]=sync_txgain;
		cal_MCU_original.Sync_TxGain[1]=0;
	}
	else if(sync_txgain<63*2)
	{
		cal_MCU_original.Sync_TxGain[0]=63;
		
		cal_MCU_original.Sync_TxGain[1]=sync_txgain-63;
	}
	else
	{
		//wrong
		cal_MCU_original.Sync_TxGain[0]=0;
		cal_MCU_original.Sync_TxGain[1]=0;		
	}

	//default value:when tx_power=33,txgain=16*2
	txgain = 2*(16+calibration_data.Tx_Power-33);
	if(txgain<63)
	{
		memset(cal_MCU_original.Tx_Gain,txgain,8);	
	}
	else
	{
		//wrong
		memset(cal_MCU_original.Tx_Gain,0x00,8);			
	}
}

//------------------------------------------------------------------------------
//Aim: search Sync_RxPower in EEPROM(SYNC)
//Input:Sync_Rx_adcV
//Output:Sync_RxPower(2 times of the true value!!!)
//------------------------------------------------------------------------------
signed char SyncRxVToSyncRxP(unsigned short SyncRxV)
{
	unsigned char i=0;
	signed char SyncRxPower;
	if(SyncRxV<SyncRxTab[0]||SyncRxV>SyncRxTab[SyncRxTabLen-1])
	{
		//wrong value
		SyncRxPower = -127;
	}
	else
	{
		for( i=0; i<SyncRxTabLen-1; i++ )
		{
		   if( SyncRxV<=SyncRxTab[i+1]&&SyncRxV>=SyncRxTab[i])
			{
			   SyncRxPower = SyncRxTab[i];
			   break;	
			} 	
		}
		SyncRxPower = -40+i;//dBm
	}	
	return SyncRxPower;
}

//------------------------------------------------------------------------------
//Aim: search Sync_TxPower in EEPROM(SYNC)
//Input:Sync_Tx_adcV
//Output:Sync_TxPower(2 times of the true value!!!)
//------------------------------------------------------------------------------
signed char SyncTxVToSyncTxP(unsigned short SyncTxV)
{
	unsigned char i=0;
	signed char SyncTxPower;
	if(SyncTxV<SyncTxTab[0]||SyncTxV>SyncTxTab[SyncTxTabLen-1])
	{
		//wrong value
		SyncTxPower = -127;
	}
	else
	{
		for( i=0; i<SyncRxTabLen-1; i++ )
		{
		   if( SyncTxV<=SyncTxTab[i+1]&&SyncTxV>=SyncTxTab[i])
		   {
			   SyncTxPower = SyncTxTab[i];
			   break;	
		   } 	
		}
		SyncTxPower = -42+i;//dBm
	}	
	return SyncTxPower;
}

//------------------------------------------------------------------------------
//Aim: reply poll_mcu message:stop timer0->check whethere frame_no is consecutive
//                            ->reply poll_mcu_ack->start timer0
//Input:pointer to frame_no
//Output:None
//------------------------------------------------------------------------------
void Poll_MCU_processing(unsigned char *frame_no)
{
	unsigned short no_now;
	unsigned short crcvalue=0;
	memset(spi_send_buf,0x00,sizeof(spi_send_buf));
	timer0_stop();
	no_now = *(unsigned short *)frame_no;
	if(no_now == (Frame_No_expect+1)%0xffff)
	{
		spi_send_buf[4] = 0;//result
		memset(&spi_send_buf[5],0,2);//Frame_No_Expect=0
		Frame_No_expect = (Frame_No_expect+1)%0xffff;
	}
	else
	{
		spi_send_buf[4] = 1;//result
		u1to2((Frame_No_expect+1)%0xffff,(char *)(spi_send_buf+5));//Frame_No_Expect
		flag_alarm_unresumable |= bit_alarm_FNWrong;//FNWrong alarm set bit
	}
	spi_send_buf[0] = msg_addr_WBBU;
	spi_send_buf[1] = msg_addr_MCU;
	spi_send_buf[2] = Type_Poll_Mcu_Ack;
	spi_send_buf[3] = 2;//length
	crcvalue=(unsigned short)dll_CalcCRC(spi_send_buf,(spi_send_buf[3]+4));
	u1to2(crcvalue,(char *)(spi_send_buf+4+spi_send_buf[3]));
	flag_recvsend |=bit_SPI_send_reply;//set the bit
	timer0_start();
}

//-----------------------------------------------------------------------------
//Aim: Output debug message to PC and and to RNMS.
//     1.If the total length is larger than 100,discard the message.
//     2.If there is dbgmsg already,strcat;otherwise, set the flag.
//Input:p_msg - pointer to the message;len - length of message
//Output:None
//-----------------------------------------------------------------------------
void dbgmsg_output(char *p_msg, unsigned char len)
{
	if(DbgCtrl&bit_output_PC)
	{
		SendMsgToDbgu(p_msg,len);
	}
	if(DbgCtrl&bit_output_RNMS)
	{
		if((len+strlen(dbgu_send_buf))<100)
		{
			if(!(flag_alarm_unresumable&bit_alarm_dbgmsg))
			{
				//there is no dbgmsg now
				strcpy(dbgu_send_buf,p_msg);
				flag_alarm_unresumable |=bit_alarm_dbgmsg;
			}
			else
			{
				strcat(dbgu_send_buf,p_msg);
			}
		}
	}
}

//-----------------------------------------------------------------------------
//Aim: Output debug data to PC and and to RNMS.
//     1.If the total length is larger than 100,discard the message.
//     2.If there is no dbgmsg now, set the flag.
//     3.Transfer hex to string.
//Input:p_msg - pointer to the message;len - length of message
//Output:None
//-----------------------------------------------------------------------------
void dbgdata_output(char *p_msg, unsigned char len)
{
	unsigned char i=0;
	if(DbgCtrl&bit_output_PC)
	{
		SendDataToDbgu(p_msg,len);
	}
	if(DbgCtrl&bit_output_RNMS)
	{
		if((len+strlen(dbgu_send_buf))<50)
		{
			sprintf(dbgu_send_buf,":");
			for(i=0;i<len;i++)
			{
				sprintf(&dbgu_send_buf[strlen(dbgu_send_buf)],"%x ",p_msg[i]);
			}
			sprintf(&dbgu_send_buf[strlen(dbgu_send_buf)],"\r\n");
			if(!(flag_alarm_unresumable&bit_alarm_dbgmsg))
			{
				//there is no dbgdata now
				flag_alarm_unresumable |=bit_alarm_dbgmsg;
			}
		}
	}
}
