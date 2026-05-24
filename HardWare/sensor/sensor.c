#include "sensor.h"


/* 【Type-C UART1串口】 ======================================================*/
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (uint8_t) ch;      
	return ch;
}

u8 res_sd;
extern u8 Sd_State;
extern u8 Photo_State;
uint64_t num=0;
char str[50];


//========================倾角传感器1和倾角传感器2======================================//
uint8_t QueryAngleBuf[5]={0x68,0x04,0x00,0x04,0x08};
float anglex,angley,anglez;
float anglex2,angley2,anglez2;
USART_REC_FLAGE  port2;
USART_REC_FLAGE  port4;	

uint8_t angle_rx_flag=0;
void Angle_IRQHandler(void)    //倾角模块中断函数1
{
	  uint8_t temp;
    if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE) != RESET)
    {
		temp =READ_REG(huart2.Instance->DR);
		if(port2.UartFlag==0)
		{
			if(temp==0x68) 
			{
				port2.Cnt_uart=0;
				port2.UartFlag=1;
				port2.UartNum=0;
				port2.XorNum=0;
			}
		}
		else if(port2.UartFlag==1)
		{
			port2.RxBuf[port2.UartNum++]=temp;
			port2.Len=port2.RxBuf[0];
			if(port2.UartNum<port2.Len)
				port2.XorNum+=temp;
			if(port2.UartNum==port2.Len)
			{
				if(port2.RxBuf[port2.Len-1]==(uint8_t)port2.XorNum)
				{
					angle_rx_flag=1;
					port2.UartFlag=0;
//						OLED_ShowString(20, 12, (u8*)"angle rx success!\n",12,1);
//					printf("angle rx success!\n");
					Angle_State = 1;
				}
				else
				{
					Angle_State = 0;
//					printf("angle rx erro! %x  %x\n",port2.XorNum,port2.RxBuf[port2.Len-1]);
					port2.UartFlag=0;
				}
			}
				
		}
		else 
		{
			port2.UartFlag=0;
		}
	}
}


//len : Dec位数
uint32_t HexToDec(uint8_t len, uint32_t hex)   //倾角数据转换
{
	uint32_t temp=0;
	for(uint8_t i=0;i<len;i++)
	{
		temp+=((hex>>(4*(len-1-i)))&0x0f)*(uint32_t)pow(10,len-1-i);
	}
	return temp;
}

float GetDepthData(float ag1,float ag2,uint8_t initial_ag,uint8_t zf)
{
	float deep;
	if(zf)  //初始角度为负数
	{
		deep = length*(sin((ag1-ag2)*0.0174)+sin((initial_ag-ag2)*0.0174));
	}
	else           //初始角度为正数
	{
		deep = length*(sin((ag1-ag2)*0.0174)-sin((initial_ag-ag2)*0.0174));
	}
	return deep;
}

void GetAngleData(void)   //获得倾角数据1
{
	uint32_t temp;
	angle_rx_flag=0;
	HAL_UART_Transmit(&huart2,QueryAngleBuf,5,0xff);
	HAL_Delay(200);
	if(angle_rx_flag==1)
	{
		temp=port2.RxBuf[3]<<16|port2.RxBuf[4]<<8|port2.RxBuf[5];
		if((temp&0x100000)==0x100000)
			anglex=-(float)HexToDec(5,temp&0x0fffff)/100;
		else 
			anglex=(float)HexToDec(5,temp)/100;
//		temp=port2.RxBuf[6]<<16|port2.RxBuf[7]<<8|port2.RxBuf[8];
//		if((temp&0x100000)==0x100000)
//			angley=-(float)HexToDec(5,temp&0x0fffff)/100;
//		else 
//			angley=(float)HexToDec(5,temp)/100;
//		temp=port2.RxBuf[9]<<16|port2.RxBuf[10]<<8|port2.RxBuf[11];
//		if((temp&0x100000)==0x100000)
//			anglez=-(float)HexToDec(5,temp&0x0fffff)/100;
//		else 
//			anglez=(float)HexToDec(5,temp)/100;
	}
}


//========倾角传感器2========//
void Angle_IRQHandler2(void)    //倾角模块中断函数2
{
	  uint8_t temp;
    if(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_RXNE) != RESET)
    {
		temp =READ_REG(huart4.Instance->DR);
		if(port4.UartFlag==0)
		{
			if(temp==0x68) 
			{
				port4.Cnt_uart=0;
				port4.UartFlag=1;
				port4.UartNum=0;
				port4.XorNum=0;
			}
		}
		else if(port4.UartFlag==1)
		{
			port4.RxBuf[port4.UartNum++]=temp;
			port4.Len=port4.RxBuf[0];
			if(port4.UartNum<port4.Len)
				port4.XorNum+=temp;
			if(port4.UartNum==port4.Len)
			{
				if(port4.RxBuf[port4.Len-1]==(uint8_t)port4.XorNum)
				{
					angle_rx_flag=1;
					port4.UartFlag=0;
//						OLED_ShowString(20, 12, (u8*)"angle rx success!\n",12,1);
//					printf("angle rx success!\n");
					Angle_State = 1;
				}
				else
				{
					Angle_State = 0;
//					printf("angle rx erro! %x  %x\n",port2.XorNum,port2.RxBuf[port2.Len-1]);
					port4.UartFlag=0;
				}
			}
				
		}
		else 
		{
			port4.UartFlag=0;
		}
	}
}



void GetAngleData2(void)   //获得倾角数据2
{
	uint32_t temp;
	angle_rx_flag=0;
	HAL_UART_Transmit(&huart4,QueryAngleBuf,5,0xff);
	HAL_Delay(200);
	if(angle_rx_flag==1)
	{
		temp=port4.RxBuf[3]<<16|port4.RxBuf[4]<<8|port4.RxBuf[5];
		if((temp&0x100000)==0x100000)
			anglex2=-(float)HexToDec(5,temp&0x0fffff)/100;
		else 
			anglex2=(float)HexToDec(5,temp)/100;
//		temp=port2.RxBuf[6]<<16|port2.RxBuf[7]<<8|port2.RxBuf[8];
//		if((temp&0x100000)==0x100000)
//			angley2=-(float)HexToDec(5,temp&0x0fffff)/100;
//		else 
//			angley2=(float)HexToDec(5,temp)/100;
//		temp=port2.RxBuf[9]<<16|port2.RxBuf[10]<<8|port2.RxBuf[11];
//		if((temp&0x100000)==0x100000)
//			anglez2=-(float)HexToDec(5,temp&0x0fffff)/100;
//		else 
//			anglez2=(float)HexToDec(5,temp)/100;
	}
}



/* 【摄像传感器】 ======================================================*/
u8 ID=0x00;
CAMERA camer;
USART_REC_FLAGE  port3;
u8 TakeBuf[7]={0x55,0x48,0x00,0x31,0x00,0x02,0x23};
u8 TakeBuf2[6]={0x55,0x45,0x00,0x01,0x00,0x23};
__ALIGNED(32) u8 PicBuf[10*1024];

void Camera_IRQHandler(void)    //摄像头模块中断函数
{
	static uint16_t xorm=0;
	uint8_t temp;
	if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_RXNE)!=RESET)
	{
		temp =READ_REG(huart3.Instance->DR);
		
//		WRITE_REG(huart1.Instance->DR,temp);
		if(port3.UartFlag==0)
		{
			if(temp==0x55) 
			{
				port3.Cnt_uart=0;
				port3.UartFlag=1;
				port3.UartNum=0;
				port3.XorNum=0;
			}
		}
		else if(port3.UartFlag==1)
		{
			if(temp==0x48)//拍照应答
			{
				port3.Cnt_uart=0;
				port3.UartFlag=2;
				port3.UartNum=0;
				port3.XorNum=0;
			}
			else if(temp==0x52&&camer.State==1)//照片信息
			{
				port3.Cnt_uart=0;
				port3.UartFlag=3;
				port3.UartNum=0;
				port3.XorNum=0;						
			}
			else if(temp==0x45&&camer.State==2)//取包应答
			{
				port3.Cnt_uart=0;
				port3.UartFlag=4;
				port3.UartNum=0;
				port3.XorNum=0;						
			}
			else if(temp==0x46&&camer.State==2)//取包内容
			{
				port3.Cnt_uart=0;
				port3.UartFlag=5;
				port3.UartNum=0;
				port3.XorNum=0;	
				port3.XorNum+=0x55;
				port3.XorNum+=0x46;				
			}
		}
		else if(port3.UartFlag==2)
		{
			port3.RxBuf[port3.UartNum++]=temp;
			if(port3.UartNum==2)
			{
				if(port3.RxBuf[0]==ID&&port3.RxBuf[1]==0x23)
				{
					camer.State=1;
//					printf("拍照应答\n");
					port3.UartFlag=0;
				}
				else
				{
					port3.UartFlag=0;
				}
			}			
		}
		else if(port3.UartFlag==3)
		{
			port3.RxBuf[port3.UartNum++]=temp;
			if(port3.UartNum==8)
			{
				if(port3.RxBuf[0]==ID&&port3.RxBuf[7]==0x23)
				{
					camer.State=2;
					camer.PicLen=port3.RxBuf[4]<<24|port3.RxBuf[3]<<16|port3.RxBuf[2]<<8|port3.RxBuf[1];
					camer.PicNum=port3.RxBuf[6]<<8|port3.RxBuf[5];
					printf("PicLen:%d PicNum:%d\n",camer.PicLen,camer.PicNum);
					port3.UartFlag=0;
				}
				else
				{
					port3.UartFlag=0;
				}
			}			
		}
		else if(port3.UartFlag==4)
		{
			port3.RxBuf[port3.UartNum++]=temp;
			if(port3.UartNum==2)
			{
				if(port3.RxBuf[0]==ID&&port3.RxBuf[1]==0x23)
				{
//					printf("取包应答\n");
					port3.UartFlag=0;
				}
				else
				{
					port3.UartFlag=0;
				}
			}				
		}
		else if(port3.UartFlag==5)
		{
			port3.RxBuf[port3.UartNum++]=temp;
			port3.XorNum+=temp;
				
			if(port3.UartNum==5&&port3.RxBuf[0]==ID)
			{
				camer.PicNo=port3.RxBuf[2]<<8|port3.RxBuf[1];
				camer.PicNoLen=port3.RxBuf[4]<<8|port3.RxBuf[3];
//			printf("PicNo:%d PicNoLen:%d\n",camer.PicNo,camer.PicNoLen);
			}
			if(port3.UartNum>5)
			{
				if(port3.UartNum==(5+camer.PicNoLen))
				{
					xorm=port3.XorNum;
				}
				if(port3.UartNum==(7+camer.PicNoLen))
				{
					if(xorm==(port3.RxBuf[6+camer.PicNoLen]<<8|port3.RxBuf[5+camer.PicNoLen]))
					{
						Photo_State=1;
						printf("PicNo:%d PicNoLen:%d\n",camer.PicNo,camer.PicNoLen);
						port3.UartFlag=0;
					}
					else {
						
						port3.UartFlag=0;
					}
				}
			}
		}

		else 
		{
			port3.UartFlag=0;
		}
	}
}



void take_photo(void)    //照相
{
	camer.State=0;
	HAL_UART_Transmit(&huart3,TakeBuf,7,0xff);
	
//	while(camer.State!=0x02)
//	{
////		printf("state: %d",camer.State);
//		HAL_Delay(10);
//	}
	HAL_Delay(600);
	for(uint16_t i=0;i<camer.PicNum;i++)
	{
		TakeBuf2[3]=i+1;
		HAL_UART_Transmit(&huart3,TakeBuf2,6,0xff);
		HAL_Delay(400);
		memmove(PicBuf+i*512,port3.RxBuf+5,camer.PicNoLen);
	}
	
}




/* 【蓝牙】 ======================================================*/
volatile uint8_t ble_rx_len = 0;  //接收一帧数据的长度
volatile uint8_t ble_recv_end_flag = 0; //一帧数据接收完成标志
uint8_t ble_rx_buffer[BUFFER_SIZE]={0};  //蓝牙接收数据缓存数组

void ble_rxdma_handle(void)
{
  uint32_t tmp_flag = 0;
	uint32_t temp;
	tmp_flag =__HAL_UART_GET_FLAG(&huart7,UART_FLAG_IDLE); //获取IDLE标志位
	if((tmp_flag != RESET))//idle标志被置位
	{ 
		__HAL_UART_CLEAR_IDLEFLAG(&huart7);//清除标志位
		//temp = huart1.Instance->SR;  //清除状态寄存器SR,读取SR寄存器可以实现清除SR寄存器的功能
		//temp = huart1.Instance->DR; //读取数据寄存器中的数据
		//这两句和上面那句等效
		HAL_UART_DMAStop(&huart7); //
		temp  =  __HAL_DMA_GET_COUNTER(&hdma_uart7_rx);// 获取DMA中未传输的数据个数   
		//temp  = hdma_usart1_rx.Instance->NDTR;//读取NDTR寄存器 获取DMA中未传输的数据个数，
		//这句和上面那句等效
		ble_rx_len =  BUFFER_SIZE - temp; //总计数减去未传输的数据个数，得到已经接收的数据个数
		ble_recv_end_flag = 1;	// 接受完成标志位置1	
	}
}

void ble_start_init(void)
{
	ble_mode1();	
	__HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);        //使能idle中断
	HAL_UART_Receive_DMA(&huart7,ble_rx_buffer,BUFFER_SIZE);  //打开DMA接收，数据存入rx_buffer数组中。	
	ble_rx_len = 0;  //接收一帧数据的长度
	ble_recv_end_flag = 0; //一帧数据接收完成标志
}


void ble_send(void)
{
	u8 ble_str[] = "ble:hello!";
	HAL_UART_Transmit(&huart7,ble_str,sizeof(ble_str),1000);
}


uint8_t ble_e2prom[5];
uint8_t clear_idD[2];
uint8_t clear_idX[2];
uint8_t temp[1]={0};
uint8_t clear_all_id[tool_bits_width*tool_num]={1,1,1,1,1,1,1};

void ble_receive(void)
{
	int i;
	if(ble_recv_end_flag==1)		//接收到一次数据了
	{
		uint8_t wnumid[1]; //记录e2prom中主机具号存储到第几个数据
		OLED_ShowString(0, 52, "ble:    ",12,1);
		OLED_ShowString(30, 52, ble_rx_buffer,12,1);
		//printf("len:%d\r\n",ble_rx_len);
		//printf("ble:%s\r\n",ble_rx_buffer);
		
		// (1) 发送指令St+长度(3位,总数小于128)+角度表示(0为正角度,1为负角度)+角度(2位)+宽度(3位)+E。完成机具匹配
    if(ble_rx_buffer[0]=='S'&&ble_rx_buffer[1]=='t'&&ble_rx_buffer[11]=='E'&&ZID[0]!=0&&ZID[1]!=0){
			I2C_EE_BufferRead(wnumid, 2, 1);  //读取e2prom的第一个数据，也就是机具号存储到第几个数据
			printf("wnumid====%d\r\n",wnumid[0]);
			I2C_EE_BufferWrite(ZID, tool_bits_width*(wnumid[0])+3, 2);   //将机具号写到对应的e2prom的位置
			//ble_e2prom[0]=(ble_rx_buffer[2]-'0')*10+(ble_rx_buffer[3]-'0');   //高度
			ble_e2prom[0]=(ble_rx_buffer[2]-'0')*100+(ble_rx_buffer[3]-'0')*10+(ble_rx_buffer[4]-'0');   //长度
			ble_e2prom[1]=(ble_rx_buffer[5]-'0');    //角度正负标识，1为负角度，0为正角度。 
			ble_e2prom[2]=(ble_rx_buffer[6]-'0')*10+(ble_rx_buffer[7]-'0');                 //初始角度
			ble_e2prom[3]=(ble_rx_buffer[8]-'0');   //宽度的百分位
			ble_e2prom[4]=(ble_rx_buffer[9]-'0')*10+(ble_rx_buffer[10]-'0');    //宽度的十分位和个分位
			I2C_EE_BufferWrite(ble_e2prom, tool_bits_width*(wnumid[0])+5, tool_bits_width-2);   //将机具号写到对应的e2prom的位置
			
			//printf("%d,%d,%d,%d,%d\r\n",ble_e2prom[0],ble_e2prom[1],ble_e2prom[2],ble_e2prom[3]);
			wnumid[0]=wnumid[0]+1;
			if(wnumid[0]>tool_num-1){
			  wnumid[0]=0;
			}
		 I2C_EE_ByteWrite(wnumid, 2);
		}
		 
		// (2) 发送指令Cl+机具号(2个3位数字)+E，清除某一个机具的全部信息
		if(ble_rx_buffer[0]=='C'&&ble_rx_buffer[1]=='l'&&ble_rx_buffer[8]=='E')
		{
			I2C_EE_BufferRead(wnumid, 2, 1);
			printf("wnumid====%d\r\n",wnumid[0]);
			printf("\r\n%d,%d,%d,%d,%d,%d\r\n",ble_rx_buffer[2],ble_rx_buffer[3],ble_rx_buffer[4],ble_rx_buffer[5],ble_rx_buffer[6],ble_rx_buffer[7]);
			clear_idD[0]=(ble_rx_buffer[2]-'0')*100+(ble_rx_buffer[3]-'0')*10+(ble_rx_buffer[4]-'0');
			clear_idD[1]=(ble_rx_buffer[5]-'0')*100+(ble_rx_buffer[6]-'0')*10+ble_rx_buffer[7]-'0';
			printf("\r\n%X,%X\r\n",clear_idD[0],clear_idD[1]);
			for(i=0;i<tool_num;i++){
				I2C_EE_BufferRead(clear_idX, tool_bits_width*i+3, 2);   //清除机具号
				printf("\r\n%x,%x\r\n",clear_idX[1],clear_idX[1]);
				if(clear_idD[0]==clear_idX[0]&&clear_idD[1]==clear_idX[1])
				{
					I2C_EE_BufferWrite(clear_all_id,tool_bits_width*i+3,tool_bits_width); 
				}
			}
			if(wnumid[0]>0){
				wnumid[0]=wnumid[0]-1;
			}
			else {
				wnumid[0] = 0;
			}
			I2C_EE_ByteWrite(wnumid, 2);
		}
		
		// (3) 发送指令Clear，清除全部的机具信息
		if(ble_rx_buffer[0]=='C'&&ble_rx_buffer[1]=='l'&&ble_rx_buffer[2]=='e'&&ble_rx_buffer[3]=='a'&&ble_rx_buffer[4]=='r')
		{
			for(i=0;i<tool_bits_width*tool_num;i++){
				clear_all_id[i]=1;
			}
     I2C_EE_BufferWrite(clear_all_id,3, tool_bits_width*tool_num);   //擦除机具号所占的空间
		 wnumid[0]=0;
		 I2C_EE_ByteWrite(wnumid, 2);
		}
		
		for(uint8_t i=0;i<ble_rx_len;i++)
		{
			ble_rx_buffer[i]=0;//清接收缓存
		}
		ble_rx_len = 0;//清除计数
		ble_recv_end_flag = 0;//清除接收结束标志位
		HAL_UART_Receive_DMA(&huart7,ble_rx_buffer,BUFFER_SIZE);//重新打开DMA接收		
	}							
}


/* 【超声波传感器】 ======================================================*/
uint8_t QueryVoiceBuf[2]={0x68,0x0D};
USART_REC_FLAGE  port6;
uint16_t Distance=0xFF;

void Voice_IRQHandler(void)
{
	
	uint8_t temp;
	if(__HAL_UART_GET_FLAG(&huart6,UART_FLAG_RXNE)!=RESET)
	{
		temp =READ_REG(huart6.Instance->DR);
//		WRITE_REG(huart1.Instance->DR,temp);

		if(port6.UartFlag==0)
		{
			if(temp==0x68) 
			{
				port6.Cnt_uart=0;
				port6.UartFlag=1;
				port6.UartNum=0;
			}	
		}
		else if(port6.UartFlag==1)
		{
			port6.RxBuf[port6.UartNum++]=temp;
			if(port6.UartNum==6)
			{
				if(port6.RxBuf[5]==0x0D)
				{
					Distance=port6.RxBuf[3]*10;
//					printf("get voice ok\r\n");
					//printf("data:%x %x %x %x %x\n",port6.RxBuf[0],port6.RxBuf[1],port6.RxBuf[2],port6.RxBuf[3],port6.RxBuf[4]);
					port6.UartFlag=0; 
					ZID[0]=port6.RxBuf[0];     //机具号（两个十六进制数据）
					ZID[1]=port6.RxBuf[1];
					
//					ZID_ready = 1;
				}
				else 
				{
					port6.UartFlag=0;
				}
			}
		}
		else
		{port6.UartFlag=0;
		}
	}
}

void GetVoiceData(void)
{
	ZID[0]=0;
	ZID[1]=0;
	HAL_UART_Transmit(&huart6,QueryVoiceBuf,2,0xff);
	HAL_Delay(200);
	//printf("distance:%d\r\n",Distance);
	//printf("ZID: %x%x",ZID[0],ZID[1]);
}


/*【SD卡存储】 ======================================================*/
	uint64_t  data_total=0;   //数据总量
  uint64_t  data_pointer=0;   //数据指针
	uint64_t  photo_total=0;   //数据总量
  uint64_t  photo_pointer=0;   //数据指针

  void sd_saveq(char *wBuff,char *filename){    //存储倾角数据
	int res_sd;
	f_open (&SDFile,filename,FA_OPEN_ALWAYS|FA_WRITE );
  f_lseek(&SDFile,data_total*(data_frame_num+2));	
  res_sd=f_write(&SDFile,wBuff,strlen((char*)wBuff),&numRW);
		
	if(res_sd!=0){  //判断sd卡存储是否正常
		Sd_State=0;
	}
		else {
	Sd_State=1;
	data_total=data_total+1;
	}
	f_close (&SDFile);
	}


	void sd_savepq(char *wBuff,char *filename){    //存储图片名称纪录
	int res_sd;
	f_open (&SDFile,filename,FA_OPEN_ALWAYS|FA_WRITE );
  f_lseek(&SDFile,photo_total*(photo_name_num+2));	
  res_sd=f_write(&SDFile,wBuff,strlen((char*)wBuff),&numRW);
		
	if(res_sd!=0){  //判断sd卡存储是否正常
		Sd_State=0;
	}
		else {
	Sd_State=1;
	//photo_total=photo_total+1;
	}
	f_close (&SDFile);
	}
	
  void sd_savep(u8 *wBuff,char *filename,uint32_t num){    //存储图片数据
	int res_sd;
	f_open (&SDFile,filename,FA_CREATE_ALWAYS|FA_WRITE );
		
//  f_lseek(&SDFile,f_size (&SDFile));
		
  res_sd=f_write(&SDFile,wBuff,num,&numRW);
	if(res_sd!=0){     //判断sd卡存储是否正常
		Sd_State=0;
	}
	else {
		Sd_State=1;
	}
	f_close (&SDFile);

	}
	
	
	/*【经纬度算面积1】 ======================================================*/
	double radian(double d)
	{
			return d * PI / 180.0;   //??1?= p / 180
	}

	double get_distance(double lat1, double lng1, double lat2, double lng2)
	{
			double radLat1 = radian(lat1);
			double radLat2 = radian(lat2);
			double a = radLat1 - radLat2;
			double b = radian(lng1) - radian(lng2);
			
			double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));
			
			dst = dst * EARTH_RADIUS;
			dst= round(dst * 10000) / 10000;
			return dst;	//单位km
	}
	
	/*【经纬度算面积2】 ======================================================*/



