#ifndef __EC20_H
#define __EC20_H
#include "usart.h"
#include "main.h"



//UTC时间信息
__packed typedef struct  
{										    
 	u16 year;	//年份
	u8 month;	//月份
	u8 date;	//日期
	u8 hour; 	//小时
	u8 min; 	//分钟
	u8 sec; 	//秒钟
}nmea_utc_time; 

__packed typedef struct  
{										    
	nmea_utc_time utc;			//UTC时间
	double latitude;				//纬度 分扩大100000倍,实际要除以100000
	u8 nshemi;					//北纬/南纬,N:北纬;S:南纬				  
	double longitude;			    //经度 分扩大100000倍,实际要除以100000
	u8 ewhemi;					//东经/西经,E:东经;W:西经
	u8 gpssta;					//GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.				   
	float speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
	float carangle;      //车的方向角
}nmea_msg; 

extern char GPSRxBuffer[200];
extern nmea_utc_time utc;  											//GPS信息
extern uint8_t AtRxBuffer[200];       
extern char *strx;
extern nmea_msg gpsx;


void reset_4g(void);
void EC20_Init(void);
void EC20Send_HexData(char *bufferdata);		
void Clear_Buffer(void);
void gggg_SendStr(char*SendBuf);
void gggg_IRQHandler(void);
void EC20Send_StrData(char *bufferdata);
void EC20Send_PData(u8 *bufferdata,u32 len);
void EC20Get_GPSBD_Data(void);
int NMEA_Str2num(u8 *buf,u8*dx);
//void NMEA_GPRMC_Analysis(nmea_utc_time *utc,u8 *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf);
void EC20Send_RecAccessMode(void);

void EC20Send_ChangeMode(u8 data);
void EC20Send_PhotoData(u8 *bufferdata,u32 len);
	


#endif
