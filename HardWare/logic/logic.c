#include "logic.h"
#include "bmp.h"
#include <stdlib.h>

/* 【模块初始化】 ======================================================*/
void module_init(void)
{
	
	
	HAL_Delay(500);
	//OLED初始化
	OLED_Init();
	OLED_ShowString(0, 10, "SYSTEM INIT...",12,1);

	__HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);

	
	//蓝牙初始化
  OLED_ShowString(0, 10, "BLE INIT...     ",12,1);
//	printf("BLE INIT\r\n");
	ble_start_init();
	
	//SD卡初始化
	OLED_ShowString(0, 10, "SD INIT...      ",12,1);
//	printf("SD INIT\r\n");
	ret_sd = f_mount (&SDFatFS,"0:",1);
//	ret_sd=f_mount (&SDFatFS,SDPath,1);
	printf("ret_sd1=%d\r\n",ret_sd);
	if(ret_sd==FR_NO_FILESYSTEM){  // 如果失败尝试重新挂载
//		ret_sd=f_mkfs(SDPath,FM_FAT32,512,work,sizeof(work));
//		printf("ret_sd2=%d\r\n",ret_sd);
		ret_sd=f_mount (&SDFatFS,SDPath,1);
		printf("ret_sd3=%d\r\n",ret_sd);
	    }
	if(ret_sd==0){
		Sd_State = 1;
		OLED_ShowString(0, 10, "SD INIT OK ",12,5);
	}

	
	//EC20初始化
	OLED_ShowString(0, 10, "EC20 INIT...    ",12,1);
//	printf("EC20 INIT\r\n");
	reset_4g();
	HAL_Delay(2000);
	gggg_SendStr("+++");//退出透传
  HAL_Delay(1000);
	
  EC20_Init();
		
	HAL_Delay(500);
	
  OLED_Clear();
	
}

/* 【模块自检】 ======================================================*/
u8 Signal_State = 0;
u8 Gps_State = 0;
u8 Ble_State = 1;
u8 Sd_State = 0;
u8 Photo_State = 0;
u8 Angle_State = 0;
u8 Selected_State = 0;


uint8_t host_id[2];

	void module_check(void)
{
	
	OLED_ShowString(0, 10, "MODULE CHECK...",12,1);
	
	GetAngleData();   //倾角检测1
	GetAngleData2();   //倾角检测2
	take_photo();   //照相检测
	GetVoiceData(); //超声检测
	
	I2C_EE_BufferRead(host_id, 0, 2);  //读取主机号
	
  OLED_Clear();
	HAL_Delay(500);

}

extern float depth;   //作业深度

void oled_main_show(void)   // oled主界面显示
{
	OLED_ClearB();
	OLED_ShowChinese(0, 11, 0, 12, 1);  //深
	OLED_ShowChinese(17, 11, 1, 12, 1); //度
	OLED_ShowChinese(30, 11, 11,12,1);
	OLED_ShowChinese(0, 25, 2, 12, 1);  //照
	OLED_ShowChinese(17, 25, 3, 12, 1); //相
	OLED_ShowChinese(30, 25, 11,12,1);
	OLED_ShowChinese(0, 39, 4, 12, 1);  //面
	OLED_ShowChinese(17, 39, 5, 12, 1); //积
	OLED_ShowChinese(30, 39, 11,12,1);
	OLED_ShowChinese(0, 53, 20, 12, 1);  //覆
	OLED_ShowChinese(13, 53, 21, 12, 1); //盖
	OLED_ShowChinese(26, 53, 22,12,1);//率
	OLED_ShowChinese(45, 53, 11,12,1); //:
	OLED_Refresh();
}

void bar_refresh(void)
{
  if(depth > 0)
	{
		smg1 = fabs(depth) / 10;  //数码管显示深度
		smg2 = abs((int)depth) % 10;
	}
	else 
	{
		smg1 = 0;
		smg2 = 0;
	}
	
	if(Signal_State&&Gps_State&&Sd_State&&Photo_State&&Angle_State&&Selected_State)
	{
		if(depth > 0)
		{
			smg3 = (int)(fabs(depth) * 10) % 10;
			smg4 = (int)(fabs(depth) * 100 + 0.5) % 10;
		}
		else 
		{
			smg3 = 0;
			smg4 = 0;
		}
	}
	
	if(Signal_State==1) OLED_ShowPicture(0,0,14,8,SignalIntensity4,0);
	else {
		OLED_ShowPicture(0,0,14,8,SignalIntensity0,0);
	   smg4=1;
		 smg3=10;
	}
	
	if(Gps_State==1) OLED_ShowPicture(16,0,8,8,GPS_Logo,0);
	else {
		OLED_ShowPicture(16,0,8,8,No_Logo,0);
		smg4=2;
		smg3=10;
	}
	
	if(Ble_State==1) OLED_ShowPicture(26,0,8,8,Square_B,0);
	else {
		OLED_ShowPicture(26,0,8,8,No_Logo,0);
		smg4=3;
		smg3=10;
	}
	
	if(Sd_State==1) OLED_ShowPicture(36,0,8,8,SD_Logo,0);
	else {
		OLED_ShowPicture(36,0,8,8,No_Logo,0);
		smg4=4;
		smg3=10;
	}
	
	if(Photo_State==1) OLED_ShowPicture(46,0,8,8,Square_P,0);
	else {
		OLED_ShowPicture(46,0,8,8,No_Logo,0);
		smg4=5;
		smg3=10;
	}
	
	if(Angle_State==1) OLED_ShowPicture(56,0,8,8,Angle_Logo,0);
	else {
		OLED_ShowPicture(56,0,8,8,No_Logo,0);
		smg4=6;
		smg3=10;
	}
	
	if(Selected_State==1) OLED_ShowPicture(66,0,8,8,Selected,0);
	else {
		OLED_ShowPicture(66,0,8,8,NotSelected,0);
		smg4=7;
		smg3=10;
	}
}


/* 【3、机具适配】 ======================================================*/
//uint8_t numid[1];  //存储记录数据
//uint8_t toolid[6];    //E2prom中读取数据缓存区
uint8_t all_toolid[tool_bits_width*tool_num];  //记录
uint8_t ZID[2];
uint8_t initial_angle,angle_zf;
uint16_t length,width;

int tool_check(void)
{
	int i;
	for(i=0;i<=tool_num-1;i++)  //遍历e2prom存储机具号的位置查看是否进行了机具匹配
	{ 
		//printf("id: %x,%x",id[0],id[1]);
		//k=strcmp((char *)ZID,(char *)id);
		if(ZID[0]!=0&&ZID[1]!=0&&ZID[0]==all_toolid[tool_bits_width*i]&&ZID[1]==all_toolid[tool_bits_width*i+1]&&all_toolid[tool_bits_width*i+2]!=1&&all_toolid[tool_bits_width*i+4]!=1&&all_toolid[tool_bits_width*i+5]!=1)
		{
			//height=all_toolid[tool_bits_width*i+2];
			length=all_toolid[tool_bits_width*i+2];
			angle_zf=all_toolid[tool_bits_width*i+3];
			initial_angle=all_toolid[tool_bits_width*i+4];
			width=all_toolid[tool_bits_width*i+5]*100+all_toolid[tool_bits_width*i+6];
			return 1;  //机具匹配
		}
	}
	//printf(" error !ZID:%x%x \r\n",ZID[0],ZID[1]);
	return 0; //机具未匹配
	
}



void tool_selected(void)
{
	int i;
	I2C_EE_BufferRead(all_toolid, 3, tool_bits_width*tool_num);  //读取所有的机具号存储到all_toolid
	for(i=0;i<tool_bits_width*tool_num;i++){
   printf("%d,",all_toolid[i]);
	}
	GetVoiceData();
	HAL_Delay(500);
	
	Selected_State = tool_check();  //检查E2PROM中是否有机具号
 
	// 机具未匹配则等待蓝牙发送指令进行匹配
	while(Selected_State==0)   
	{
		char str_anglex[5];
		I2C_EE_BufferRead(all_toolid,3, tool_bits_width*tool_num);  //读取所有的机具号
		for(i=0;i<tool_bits_width*tool_num;i++){
		printf("%d,",all_toolid[i]);
	}
	GetAngleData();   //获取倾角数据
	ble_receive();   //蓝牙接收
	GetVoiceData();   //获取机具号
	HAL_Delay(500);
	Selected_State = tool_check();
	 
	// 机具未匹配进行显示
	if(Selected_State==0)
	{   
		// OLED_Clear();
		//OLED_ShowString(0,10,"                 ",12,1);
		OLED_ShowChinese(0, 8, 6, 12, 1);   //请
		OLED_ShowChinese(12, 8, 7, 12, 1);  //机
		OLED_ShowChinese(24, 8, 8, 12, 1);  //具
		OLED_ShowChinese(36, 8, 9, 12, 1);  //匹
		OLED_ShowChinese(48, 8, 10, 12, 1); //配
		// OLED_ShowString(60,10,"   ",12,1);
		
		OLED_ShowChinese(0, 25, 15,12,1);    //角
		OLED_ShowChinese(17, 25, 16, 12, 1); //度
		OLED_ShowChinese(30, 25, 11, 12, 1);
		
		OLED_ShowChinese(0, 39, 7, 12, 1);  //机
		OLED_ShowChinese(17, 39, 8,12,1);   //具
		OLED_ShowChinese(30, 39, 11, 12, 1);
	
		
//		OLED_ShowString(0,10,"Please adapt the tool",12,1);
//		OLED_ShowString(0, 24, "anglex: ",12,1);
		sprintf(str_anglex,"%0.2f  ",anglex);
		OLED_ShowString(42, 24, (u8*)str_anglex,12,1);
//		OLED_ShowString(0, 38, "id: ",12,1);
		sprintf(str_anglex,"%d%d  ",ZID[0],ZID[1]);
		OLED_ShowString(42, 38, (u8*)str_anglex,12,1);
		//OLED_Refresh();
	}	 
}
	I2C_EE_BufferRead(all_toolid, 3, tool_bits_width*tool_num);  //读取所有的机具号
	for(i=0;i<tool_bits_width*tool_num;i++){
   printf("%d,",all_toolid[i]);
	}
	HAL_Delay(500);
}


/* 【定时器中断】 =====================================================*/
//smg1~4对应数码管1~4位
u8 smg1=0,smg2=0,smg3=0,smg4=0;
u8 tim4=0;
//uint32_t timei=0;
//定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//【Tim2-SMG-1ms】
  if(htim->Instance == TIM2)
  {
		//所有模块正常显示0000
		static u8 timei =0;
		timei ++;
		if(timei == 1) 
		{
			smg_onx(1,smg1);
		}
		if(timei == 2) 
		{
			smg_onx(2,smg2);
		}
		if(timei == 3) 
		{
			smg_onx(3,smg3);
		}
		if(timei == 4) 
		{
			smg_onx(4,smg4);
			timei = 0;
		}	
  }
}

