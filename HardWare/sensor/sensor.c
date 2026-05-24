#include "sensor.h"


/* ¡¾Type-C UART1´®¿Ú¡¿ ======================================================*/
//ÖØ¶¨Òåfputcº¯Êý 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
	USART1->DR = (uint8_t) ch;      
	return ch;
}

u8 res_sd;
extern u8 Sd_State;
extern u8 Photo_State;
uint64_t num=0;
char str[50];


//========================Çã½Ç´«¸ÐÆ÷1ºÍÇã½Ç´«¸ÐÆ÷2======================================//
uint8_t QueryAngleBuf[5]={0x68,0x04,0x00,0x04,0x08};
float anglex,angley,anglez;
float anglex2,angley2,anglez2;
USART_REC_FLAGE  port2;
USART_REC_FLAGE  port4;	

uint8_t angle_rx_flag=0;
void Angle_IRQHandler(void)    //Çã½ÇÄ£¿éÖÐ¶Ïº¯Êý1
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


//len : DecÎ»Êý
uint32_t HexToDec(uint8_t len, uint32_t hex)   //Çã½ÇÊý¾Ý×ª»»
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
	if(zf)  //³õÊ¼½Ç¶ÈÎª¸ºÊý
	{
		deep = length*(sin((ag1-ag2)*0.0174)+sin((initial_ag-ag2)*0.0174));
	}
	else           //³õÊ¼½Ç¶ÈÎªÕýÊý
	{
		deep = length*(sin((ag1-ag2)*0.0174)-sin((initial_ag-ag2)*0.0174));
	}
	return deep;
}

void GetAngleData(void)   //»ñµÃÇã½ÇÊý¾Ý1
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


//========Çã½Ç´«¸ÐÆ÷2========//
void Angle_IRQHandler2(void)    //Çã½ÇÄ£¿éÖÐ¶Ïº¯Êý2
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



void GetAngleData2(void)   //»ñµÃÇã½ÇÊý¾Ý2
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



/* ¡¾ÉãÏñ´«¸ÐÆ÷¡¿ ======================================================*/
u8 ID=0x00;
CAMERA camer;
USART_REC_FLAGE  port3;
u8 TakeBuf[7]={0x55,0x48,0x00,0x31,0x00,0x02,0x23};
u8 TakeBuf2[6]={0x55,0x45,0x00,0x01,0x00,0x23};
__align(32) u8 PicBuf[10*1024];

void Camera_IRQHandler(void)    //ÉãÏñÍ·Ä£¿éÖÐ¶Ïº¯Êý
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
			if(temp==0x48)//ÅÄÕÕÓ¦´ð
			{
				port3.Cnt_uart=0;
				port3.UartFlag=2;
				port3.UartNum=0;
				port3.XorNum=0;
			}
			else if(temp==0x52&&camer.State==1)//ÕÕÆ¬ÐÅÏ¢
			{
				port3.Cnt_uart=0;
				port3.UartFlag=3;
				port3.UartNum=0;
				port3.XorNum=0;						
			}
			else if(temp==0x45&&camer.State==2)//È¡°üÓ¦´ð
			{
				port3.Cnt_uart=0;
				port3.UartFlag=4;
				port3.UartNum=0;
				port3.XorNum=0;						
			}
			else if(temp==0x46&&camer.State==2)//È¡°üÄÚÈÝ
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
//					printf("ÅÄÕÕÓ¦´ð\n");
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
//					printf("È¡°üÓ¦´ð\n");
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



void take_photo(void)    //ÕÕÏà
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




/* ¡¾À¶ÑÀ¡¿ ======================================================*/
volatile uint8_t ble_rx_len = 0;  //½ÓÊÕÒ»Ö¡Êý¾ÝµÄ³¤¶È
volatile uint8_t ble_recv_end_flag = 0; //Ò»Ö¡Êý¾Ý½ÓÊÕÍê³É±êÖ¾
uint8_t ble_rx_buffer[BUFFER_SIZE]={0};  //À¶ÑÀ½ÓÊÕÊý¾Ý»º´æÊý×é

void ble_rxdma_handle(void)
{
  uint32_t tmp_flag = 0;
	uint32_t temp;
	tmp_flag =__HAL_UART_GET_FLAG(&huart7,UART_FLAG_IDLE); //»ñÈ¡IDLE±êÖ¾Î»
	if((tmp_flag != RESET))//idle±êÖ¾±»ÖÃÎ»
	{ 
		__HAL_UART_CLEAR_IDLEFLAG(&huart7);//Çå³ý±êÖ¾Î»
		//temp = huart1.Instance->SR;  //Çå³ý×´Ì¬¼Ä´æÆ÷SR,¶ÁÈ¡SR¼Ä´æÆ÷¿ÉÒÔÊµÏÖÇå³ýSR¼Ä´æÆ÷µÄ¹¦ÄÜ
		//temp = huart1.Instance->DR; //¶ÁÈ¡Êý¾Ý¼Ä´æÆ÷ÖÐµÄÊý¾Ý
		//ÕâÁ½¾äºÍÉÏÃæÄÇ¾äµÈÐ§
		HAL_UART_DMAStop(&huart7); //
		temp  =  __HAL_DMA_GET_COUNTER(&hdma_uart7_rx);// »ñÈ¡DMAÖÐÎ´´«ÊäµÄÊý¾Ý¸öÊý   
		//temp  = hdma_usart1_rx.Instance->NDTR;//¶ÁÈ¡NDTR¼Ä´æÆ÷ »ñÈ¡DMAÖÐÎ´´«ÊäµÄÊý¾Ý¸öÊý£¬
		//Õâ¾äºÍÉÏÃæÄÇ¾äµÈÐ§
		ble_rx_len =  BUFFER_SIZE - temp; //×Ü¼ÆÊý¼õÈ¥Î´´«ÊäµÄÊý¾Ý¸öÊý£¬µÃµ½ÒÑ¾­½ÓÊÕµÄÊý¾Ý¸öÊý
		ble_recv_end_flag = 1;	// ½ÓÊÜÍê³É±êÖ¾Î»ÖÃ1	
	}
}

void ble_start_init(void)
{
	ble_mode1();	
	__HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);        //Ê¹ÄÜidleÖÐ¶Ï
	HAL_UART_Receive_DMA(&huart7,ble_rx_buffer,BUFFER_SIZE);  //´ò¿ªDMA½ÓÊÕ£¬Êý¾Ý´æÈërx_bufferÊý×éÖÐ¡£	
	ble_rx_len = 0;  //½ÓÊÕÒ»Ö¡Êý¾ÝµÄ³¤¶È
	ble_recv_end_flag = 0; //Ò»Ö¡Êý¾Ý½ÓÊÕÍê³É±êÖ¾
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
	if(ble_recv_end_flag==1)		//½ÓÊÕµ½Ò»´ÎÊý¾ÝÁË
	{
		uint8_t wnumid[1]; //¼ÇÂ¼e2promÖÐÖ÷»ú¾ßºÅ´æ´¢µ½µÚ¼¸¸öÊý¾Ý
		OLED_ShowString(0, 52, "ble:    ",12,1);
		OLED_ShowString(30, 52, ble_rx_buffer,12,1);
		//printf("len:%d\r\n",ble_rx_len);
		//printf("ble:%s\r\n",ble_rx_buffer);
		
		// (1) ·¢ËÍÖ¸ÁîSt+³¤¶È(3Î»,×ÜÊýÐ¡ÓÚ128)+½Ç¶È±íÊ¾(0ÎªÕý½Ç¶È,1Îª¸º½Ç¶È)+½Ç¶È(2Î»)+¿í¶È(3Î»)+E¡£Íê³É»ú¾ßÆ¥Åä
    if(ble_rx_buffer[0]=='S'&&ble_rx_buffer[1]=='t'&&ble_rx_buffer[11]=='E'&&ZID[0]!=0&&ZID[1]!=0){
			I2C_EE_BufferRead(wnumid, 2, 1);  //¶ÁÈ¡e2promµÄµÚÒ»¸öÊý¾Ý£¬Ò²¾ÍÊÇ»ú¾ßºÅ´æ´¢µ½µÚ¼¸¸öÊý¾Ý
			printf("wnumid====%d\r\n",wnumid[0]);
			I2C_EE_BufferWrite(ZID, tool_bits_width*(wnumid[0])+3, 2);   //½«»ú¾ßºÅÐ´µ½¶ÔÓ¦µÄe2promµÄÎ»ÖÃ
			//ble_e2prom[0]=(ble_rx_buffer[2]-'0')*10+(ble_rx_buffer[3]-'0');   //¸ß¶È
			ble_e2prom[0]=(ble_rx_buffer[2]-'0')*100+(ble_rx_buffer[3]-'0')*10+(ble_rx_buffer[4]-'0');   //³¤¶È
			ble_e2prom[1]=(ble_rx_buffer[5]-'0');    //½Ç¶ÈÕý¸º±êÊ¶£¬1Îª¸º½Ç¶È£¬0ÎªÕý½Ç¶È¡£ 
			ble_e2prom[2]=(ble_rx_buffer[6]-'0')*10+(ble_rx_buffer[7]-'0');                 //³õÊ¼½Ç¶È
			ble_e2prom[3]=(ble_rx_buffer[8]-'0');   //¿í¶ÈµÄ°Ù·ÖÎ»
			ble_e2prom[4]=(ble_rx_buffer[9]-'0')*10+(ble_rx_buffer[10]-'0');    //¿í¶ÈµÄÊ®·ÖÎ»ºÍ¸ö·ÖÎ»
			I2C_EE_BufferWrite(ble_e2prom, tool_bits_width*(wnumid[0])+5, tool_bits_width-2);   //½«»ú¾ßºÅÐ´µ½¶ÔÓ¦µÄe2promµÄÎ»ÖÃ
			
			//printf("%d,%d,%d,%d,%d\r\n",ble_e2prom[0],ble_e2prom[1],ble_e2prom[2],ble_e2prom[3]);
			wnumid[0]=wnumid[0]+1;
			if(wnumid[0]>tool_num-1){
			  wnumid[0]=0;
			}
		 I2C_EE_ByteWrite(wnumid, 2);
		}
		 
		// (2) ·¢ËÍÖ¸ÁîCl+»ú¾ßºÅ(2¸ö3Î»Êý×Ö)+E£¬Çå³ýÄ³Ò»¸ö»ú¾ßµÄÈ«²¿ÐÅÏ¢
		if(ble_rx_buffer[0]=='C'&&ble_rx_buffer[1]=='l'&&ble_rx_buffer[8]=='E')
		{
			I2C_EE_BufferRead(wnumid, 2, 1);
			printf("wnumid====%d\r\n",wnumid[0]);
			printf("\r\n%d,%d,%d,%d,%d,%d\r\n",ble_rx_buffer[2],ble_rx_buffer[3],ble_rx_buffer[4],ble_rx_buffer[5],ble_rx_buffer[6],ble_rx_buffer[7]);
			clear_idD[0]=(ble_rx_buffer[2]-'0')*100+(ble_rx_buffer[3]-'0')*10+(ble_rx_buffer[4]-'0');
			clear_idD[1]=(ble_rx_buffer[5]-'0')*100+(ble_rx_buffer[6]-'0')*10+ble_rx_buffer[7]-'0';
			printf("\r\n%X,%X\r\n",clear_idD[0],clear_idD[1]);
			for(i=0;i<tool_num;i++){
				I2C_EE_BufferRead(clear_idX, tool_bits_width*i+3, 2);   //Çå³ý»ú¾ßºÅ
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
		
		// (3) ·¢ËÍÖ¸ÁîClear£¬Çå³ýÈ«²¿µÄ»ú¾ßÐÅÏ¢
		if(ble_rx_buffer[0]=='C'&&ble_rx_buffer[1]=='l'&&ble_rx_buffer[2]=='e'&&ble_rx_buffer[3]=='a'&&ble_rx_buffer[4]=='r')
		{
			for(i=0;i<tool_bits_width*tool_num;i++){
				clear_all_id[i]=1;
			}
     I2C_EE_BufferWrite(clear_all_id,3, tool_bits_width*tool_num);   //²Á³ý»ú¾ßºÅËùÕ¼µÄ¿Õ¼ä
		 wnumid[0]=0;
		 I2C_EE_ByteWrite(wnumid, 2);
		}
		
		for(uint8_t i=0;i<ble_rx_len;i++)
		{
			ble_rx_buffer[i]=0;//Çå½ÓÊÕ»º´æ
		}
		ble_rx_len = 0;//Çå³ý¼ÆÊý
		ble_recv_end_flag = 0;//Çå³ý½ÓÊÕ½áÊø±êÖ¾Î»
		HAL_UART_Receive_DMA(&huart7,ble_rx_buffer,BUFFER_SIZE);//ÖØÐÂ´ò¿ªDMA½ÓÊÕ		
	}							
}


/* ¡¾³¬Éù²¨´«¸ÐÆ÷¡¿ ======================================================*/
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
					ZID[0]=port6.RxBuf[0];     //»ú¾ßºÅ£¨Á½¸öÊ®Áù½øÖÆÊý¾Ý£©
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


/*¡¾SD¿¨´æ´¢¡¿ ======================================================*/
	uint64_t  data_total=0;   //Êý¾Ý×ÜÁ¿
  uint64_t  data_pointer=0;   //Êý¾ÝÖ¸Õë
	uint64_t  photo_total=0;   //Êý¾Ý×ÜÁ¿
  uint64_t  photo_pointer=0;   //Êý¾ÝÖ¸Õë

  void sd_saveq(char *wBuff,char *filename){    //´æ´¢Çã½ÇÊý¾Ý
	int res_sd;
	f_open (&SDFile,filename,FA_OPEN_ALWAYS|FA_WRITE );
  f_lseek(&SDFile,data_total*(data_frame_num+2));	
  res_sd=f_write(&SDFile,wBuff,strlen((char*)wBuff),&numRW);
		
	if(res_sd!=0){  //ÅÐ¶Ïsd¿¨´æ´¢ÊÇ·ñÕý³£
		Sd_State=0;
	}
		else {
	Sd_State=1;
	data_total=data_total+1;
	}
	f_close (&SDFile);
	}


	void sd_savepq(char *wBuff,char *filename){    //´æ´¢Í¼Æ¬Ãû³Æ¼ÍÂ¼
	int res_sd;
	f_open (&SDFile,filename,FA_OPEN_ALWAYS|FA_WRITE );
  f_lseek(&SDFile,photo_total*(photo_name_num+2));	
  res_sd=f_write(&SDFile,wBuff,strlen((char*)wBuff),&numRW);
		
	if(res_sd!=0){  //ÅÐ¶Ïsd¿¨´æ´¢ÊÇ·ñÕý³£
		Sd_State=0;
	}
		else {
	Sd_State=1;
	//photo_total=photo_total+1;
	}
	f_close (&SDFile);
	}
	
  void sd_savep(u8 *wBuff,char *filename,uint32_t num){    //´æ´¢Í¼Æ¬Êý¾Ý
	int res_sd;
	f_open (&SDFile,filename,FA_CREATE_ALWAYS|FA_WRITE );
		
//  f_lseek(&SDFile,f_size (&SDFile));
		
  res_sd=f_write(&SDFile,wBuff,num,&numRW);
	if(res_sd!=0){     //ÅÐ¶Ïsd¿¨´æ´¢ÊÇ·ñÕý³£
		Sd_State=0;
	}
	else {
		Sd_State=1;
	}
	f_close (&SDFile);

	}
	
	
	/*¡¾¾­Î³¶ÈËãÃæ»ý1¡¿ ======================================================*/
	double radian(double d)
	{
			return d * PI / 180.0;   //??1° = p / 180
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
			return dst;	//µ¥Î»km
	}
	
	/*¡¾¾­Î³¶ÈËãÃæ»ý2¡¿ ======================================================*/



