 #ifndef _SENSOR_H_
 #define _SENSOR_H_

#include "main.h"
#include "usart.h"
#include "math.h"
#include "oled.h"
#include "led.h"
#include "stdio.h"
#include "string.h"
#include "ec20.h"
#include "e2prom.h"
#include "fatfs.h"
#include "sdio.h"
#include "logic.h"

/* 【SMG刷新 Tim3】 =====================================================*/
extern u8 smg1,smg2,smg3,smg4;	////smg1~4对应数码管1~4位

/* 【倾角传感器】 ======================================================*/
typedef struct
{
	uint8_t		UartFlag;
	uint8_t 	Cnt_uart;
	uint16_t 	XorNum;
	uint16_t 	UartNum;
	uint8_t		RxBuf[1024];
	uint16_t  Len;
}USART_REC_FLAGE;

extern float anglex,angley,anglez;
extern float anglex2,angley2,anglez2;

void Angle_IRQHandler(void);
void Angle_IRQHandler2(void);
uint32_t HexToDec(uint8_t len, uint32_t hex);
void GetAngleData(void);
void GetAngleData2(void);
float GetDepthData(float ag1,float ag2,uint8_t initial_ag,uint8_t zf);

/* 【摄像传感器】 ======================================================*/
typedef struct
{
	u16 PicNum;
	u32 PicLen;
	u8 State;
	u16 PicNo;
	u16 PicNoLen;
}CAMERA;

extern  u8 PicBuf[10*1024];
extern CAMERA camer;

void Camera_IRQHandler(void);
void take_photo(void);


/* 【蓝牙】 ======================================================*/
#define BUFFER_SIZE  500
#define ble_mode0()				HAL_GPIO_WritePin(BLE_MODE_GPIO_Port,BLE_MODE_Pin,GPIO_PIN_RESET)
#define ble_mode1()				HAL_GPIO_WritePin(BLE_MODE_GPIO_Port,BLE_MODE_Pin,GPIO_PIN_SET)

extern volatile uint8_t ble_rx_len ;  //接收一帧数据的长度
extern volatile uint8_t ble_recv_end_flag ; //一帧数据接收完成标志
extern uint8_t ble_rx_buffer[BUFFER_SIZE];  //蓝牙接收数据缓存数组


void ble_rxdma_handle(void);
void ble_send(void);
void ble_receive(void);
void ble_start_init(void);

/* 【超声波传感器】 ======================================================*/

extern uint16_t Distance;   //距离
extern uint8_t ZID[2]; //主机号
extern uint8_t numid[1];  //存储记录数据

void Voice_IRQHandler(void);
void GetVoiceData(void);

/* 【sd卡存储】 ======================================================*/
void sd_saveq(char *wBuff,char *filename);
void sd_savepq(char *wBuff,char *filename);
void sd_savep(u8 *wBuff,char *filename,uint32_t num);
extern	uint64_t  data_total;   //数据总量
extern uint64_t  data_pointer;   //数据指针
extern	uint64_t  photo_total;   //数据总量
extern  uint64_t  photo_pointer;   //数据指针
	 
/*【经纬度算面积】 ======================================================*/
#define PI                      3.1415926
#define EARTH_RADIUS            6378.137        //地球近似半径
double radian(double d);
double get_distance(double lat1, double lng1, double lat2, double lng2);
	 
#endif



