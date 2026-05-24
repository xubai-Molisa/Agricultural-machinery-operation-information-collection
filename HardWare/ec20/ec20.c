#include "ec20.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "logic.h"


#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "ec20.h"



uint8_t gsm_rx_buffer[200],gsm_uart_p;
uint8_t AtRxBuffer[200],Rxcouter;
#define BUFLEN     200

char GPSRxBuffer[BUFLEN];   
int errcount = 0;
char atstr[BUFLEN];

char *strx,*extstrx,*try1,*result;
//¸´Î»4GÄ£¿é
void reset_4g()
{
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
    HAL_Delay(3000);
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
    HAL_Delay(100);
}

void gggg_IRQHandler(void)                                //´®¿Ú8ÖĞ¶Ï·şÎñ³ÌĞò
{
	 if((__HAL_UART_GET_FLAG(&huart8,UART_FLAG_RXNE)!=RESET))  //½ÓÊÕÖĞ¶Ï(½ÓÊÕµ½µÄÊı¾İ±ØĞëÊÇ0x0d 0x0a½áÎ²)
		{
				AtRxBuffer[Rxcouter] = READ_REG(huart8.Instance->DR);	
				Rxcouter++;
		}
}

void gggg_SendStr(char*SendBuf)    //´®¿Ú2´òÓ¡Êı¾İ
{
    while(*SendBuf)
    {
        while((UART8->SR&0X40)==0);//µÈ´ı·¢ËÍÍê³É
        UART8->DR = (char) *SendBuf;
        SendBuf++;
		/*
		void gggg_SendStr(char* SendBuf)
{
    // ¼ÆËã×Ö·û´®³¤¶È£¨Óöµ½ \0 Í£Ö¹£©
    uint16_t len = 0;
    while (SendBuf[len] != '\0') len++;

    // HAL ¿â×èÈû·¢ËÍ£¨UART8£¬·¢Íê²Å·µ»Ø£©
    HAL_UART_Transmit(&huart8, (uint8_t*)SendBuf, len, 0xFFFF);
}ÕâÊÇHAL¿âµÄĞ´·¨£¬¼Ä´æÆ÷¿´²»¶®µÄ»°¿ÉÒÔÓÃÕâ¸öÀ´Àí½â
		*/
    }
}

void Clear_Buffer(void)//Çå¿Õ»º´æ
{
    uint8_t i;
		//printf(AtRxBuffer);
    for(i=0;i<Rxcouter+1;i++)
    AtRxBuffer[i]=0;//»º´æ
    Rxcouter=0;
}


//»ñÈ¡GPS±±¶·Êı¾İ
void EC20Get_GPSBD_Data(void)
{
    Clear_Buffer();
    memset(GPSRxBuffer,0,BUFLEN);

    gggg_SendStr("AT+QGPSGNMEA=\042RMC\042\r\n");//¶ÁÈ¡GPS±±¶·¶¨Î»Êı¾İ
    HAL_Delay(200);
    sprintf(GPSRxBuffer,"%s",AtRxBuffer);
    strx=strstr((const char*)GPSRxBuffer,(const char*)"GPRMC");//·´À¡OK
//		if(strx==NULL)
//		Gps_State=0;
//			else 
//		Gps_State=1;
    errcount=0;
    while(strx==NULL)
    {
//			Gps_State=0;
        errcount++;
        strx=strstr((const char*)GPSRxBuffer,(const char*)"GPRMC");//·´À¡OK
        if(errcount>10)     //·ÀÖ¹ËÀÑ­»·µôÏßÖØĞÂ¿ªÊ¼
        {
            errcount = 0;
//            reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
//            NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
    }
    Clear_Buffer();
}


  //Í¸´«Ä£Ê½ÏÂ·¢ËÍÍ¼Æ¬Êı¾İ
void EC20Send_PhotoData(u8 *bufferdata,u32 len){
		u32 i;
		u8* p;
		p = (u8*)bufferdata;
		for(i=0;i<len*4;i++)		//dma´«Êä1´ÎµÈÓÚ4×Ö½Ú,ËùÒÔ³ËÒÔ4.
		{  
			while((UART8->SR&0X40)==0);//µÈ´ı·¢ËÍÍê³É
			UART8->DR = (uint8_t)p[i];
			if(p[i-1] == 0xff && p[i] == 0xd9)
				return ;
		}
}



///·¢ËÍ×Ö·ûĞÍÊı¾İ
int fail_count=0;  //¼ÇÂ¼·¢ËÍÊ§°Ü´ÎÊı
void EC20Send_StrData(char *bufferdata)
{   
	  Signal_State=0;
   // uint8_t untildata;
    gggg_SendStr("AT+QISEND=0\r\n");
    HAL_Delay(100);
    gggg_SendStr(bufferdata);
    HAL_Delay(50);
    while((UART8->SR&0X40)==0);//µÈ´ı·¢ËÍÍê³É
    UART8->DR = (uint8_t) 0x1a;
//    while((__HAL_UART_GET_FLAG(&huart8,UART_FLAG_RXNE)==RESET))
//    {
//    }
    HAL_Delay(200);
    strx=strstr((char*)AtRxBuffer,(char*)"SEND OK");//ÊÇ·ñÕıÈ··¢ËÍ
	  if(strx){          //ÅĞ¶ÏÊÇ·ñ·¢ËÍ³É¹¦
			Signal_State=1;
			fail_count=0;
			data_pointer=data_pointer+1;
		}
		else{
			fail_count++;
			if(fail_count>=e4G_fail_tim)   //Á¬ĞøÈı´Î·¢ËÍÊ§°ÜÈÏÎªÁ¬½ÓÖĞ¶Ï
			Signal_State=0;
		}
    errcount=0;
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((char*)AtRxBuffer,(char*)"SEND OK");//ÊÇ·ñÕıÈ··¢ËÍ
			
        HAL_Delay(10);
        if(errcount>=3)     //·ÀÖ¹ËÀÑ­»·µôÏßÖØĞÂ¿ªÊ¼
        {
            errcount = 0;
//            reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
//            NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
    }
		
    HAL_Delay(100);
    Clear_Buffer();

//    gggg_SendStr("AT+QISEND=0,0\r\n");
//    HAL_Delay(200);
//    strx=strstr((char*)AtRxBuffer,(char*)"+QISEND:");//·¢ËÍÊ£Óà×Ö½ÚÊı¾İ
//    while(untildata)
//    {
//        gggg_SendStr("AT+QISEND=0,0\r\n");
//        HAL_Delay(200);
//        strx=strstr((char*)AtRxBuffer,(char*)"+QISEND:");//·¢ËÍÊ£Óà×Ö½ÚÊı¾İ
//        strx=strstr((char*)strx,(char*)",");//»ñÈ¡µÚÒ»¸ö,
//        strx=strstr((char*)(strx+1),(char*)",");//»ñÈ¡µÚ¶ş¸ö,
//        untildata=*(strx+1)-0x30;
//        Clear_Buffer();
//    }

}




///Í¸´«Ä£Ê½ÏÂ½ÓÊÜÊı¾İ
void EC20Send_RecAccessMode(void)
{
    gggg_SendStr("AT+QIRD=0,7\r\n");
		HAL_Delay(200);
	  try1=strstr((const char*)AtRxBuffer,(const char*)"+QIRD:");//½ÓÊÕµ½·şÎñÆ÷ÏÂ·¢Êı¾İ
    if(try1)
    {
        //ÓÃ»§¿ÉÒÔÌí¼Ó×Ô¼ºµÄ´úÂëÀ´ÊµÏÖ
				//try1 += 8; 
			  result = strstr(try1, "ww");
			  result +=2;
			  printf("try:%s\r\n",try1);//Êı¾İ·¢»ØÈ
			  printf("result:%s\r\n",result);
			  //sprintf(result,"%s%%",result);
				OLED_ShowString(60, 53, (u8*)result,12,1);
			  OLED_ShowChinese(91, 53, 23, 12, 1); // Ä¶
			 
			  size_t length = strlen(result);
        printf("Length of the string: %zu\n", length);
        delay_ms(500);
        Clear_Buffer();
    }
    Clear_Buffer();
//		return try1;
//		
//		char *resultnew; 
//    int j = 0;
//    for(int i = 0; try1[i] != '\0'; i++)
//    {
//        if(try1[i] != ' ' && try1[i] != '\n' && try1[i] != '\r')
//        {
//            resultnew[j] = try1[i];
//            j++;
//        }
//    }
//		printf("%s",try1);
//		return try1;
}




// ·ÇÍ¸´«Ä£Ê½·¢ËÍÍ¼Æ¬Êı¾İ
void EC20Send_PData(u8 *bufferdata,u32 len)
{   
//    uint8_t untildata;
		u32 i,k;

	  for(i=0;i<len/1360+1;i++){
			gggg_SendStr("AT+QISEND=0,1360\r\n");
			HAL_Delay(50);
			k = 1360*i;
			for(int j=0;j<1360;j++){
				if(!(bufferdata[k+j-1] == 0xff && bufferdata[k+j] == 0xd9)){
           while((UART8->SR&0X40)==0);//µÈ´ı·¢ËÍÍê³É
           UART8->DR = (uint8_t)bufferdata[k+j];
				}
			}
			Clear_Buffer();
			HAL_Delay(50);
    }
}



///·¢ËÍÊ®Áù½øÖÆ
void EC20Send_HexData(char *bufferdata)
{
    u8 untildata;
    memset(atstr,0,BUFLEN);
    sprintf(atstr,"AT+QISENDEX=0,\042%s\042\r\n",bufferdata);
    gggg_SendStr(atstr);
    HAL_Delay(100);
    while((UART8->SR&0X40)==0);//µÈ´ı·¢ËÍÍê³É
    UART8->DR = (uint8_t) 0x1a;
    while((__HAL_UART_GET_FLAG(&huart8,UART_FLAG_RXNE)==RESET))
    {
    }
    HAL_Delay(100);
    strx=strstr((char*)AtRxBuffer,(char*)"OK");//ÊÇ·ñÕıÈ··¢ËÍ
    errcount=0;
    while(strx==NULL)
    {
        errcount++;
        strx=strstr((char*)AtRxBuffer,(char*)"OK");//ÊÇ·ñÕıÈ··¢ËÍ
        HAL_Delay(10);
        if(errcount>300)     //·ÀÖ¹ËÀÑ­»·µôÏßÖØĞÂ¿ªÊ¼
        {
            errcount = 0;
            //reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
            NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
    }
    HAL_Delay(100);
    Clear_Buffer();
    gggg_SendStr("AT+QISEND=0,0\r\n");
    HAL_Delay(200);
    strx=strstr((char*)AtRxBuffer,(char*)"+QISEND:");//·¢ËÍÊ£Óà×Ö½ÚÊı¾İ
    while(untildata)
    {
        gggg_SendStr("AT+QISEND=0,0\r\n");
        HAL_Delay(200);
        strx=strstr((char*)AtRxBuffer,(char*)"+QISEND:");//·¢ËÍÊ£Óà×Ö½ÚÊı¾İ
        strx=strstr((char*)strx,(char*)",");//»ñÈ¡µÚÒ»¸ö,
        strx=strstr((char*)(strx+1),(char*)",");//»ñÈ¡µÚ¶ş¸ö,
        untildata=*(strx+1)-0x30;
        Clear_Buffer();
    }

}



///ÇĞ»»¹¤×÷Ä£Ê½Í¸´«ºÍÖ¸ÁîÄ£Ê½
void EC20Send_ChangeMode(u8 data)
{
  if(data)//ÇĞ»»ÎªÃüÁîÄ£Ê½
	{
		HAL_Delay(500);
		gggg_SendStr("+++");//ÇĞ»»Í¸´«
		HAL_Delay(600);
		HAL_Delay(600);
    }
  else//ÇĞ»»ÎªÊı¾İÄ£Ê½
    {
        Clear_Buffer();
        gggg_SendStr("AT+QISWTMD=0,2\r\n");//ÇĞ»»ÎªÍ¸´«Ä£Ê½
        HAL_Delay(50);
        strx=strstr((const char*)AtRxBuffer,(const char*)"CONNECT");//ÃüÁîÇĞ»»Íê³É
//      while(strx==NULL)
//        {
//            strx=strstr((const char*)AtRxBuffer,(const char*)"CONNECT");//ÃüÁîÇĞ»»Íê³É
//        }
        Clear_Buffer();
        HAL_Delay(50);
    }
}




void  EC20_Init(void)
{

    gggg_SendStr("AT\r\n");
    HAL_Delay(500);                                                
    strx=strstr((const char*)AtRxBuffer,(const char*)"OK");//·µ»ØOK
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        printf("\r\nµ¥Æ¬»úÕıÔÚÁ¬½Óµ½Ä£¿é...\r\n");
        Clear_Buffer();
        gggg_SendStr("AT\r\n");
        HAL_Delay(500);
        strx=strstr((const char*)AtRxBuffer,(const char*)"OK");//·µ»ØOK
        if(errcount>25)     //·ÀÖ¹ËÀÑ­»·
        {
            errcount = 0;
            //reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
            //NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
    }

//    gggg_SendStr("ATE1\r\n"); //»ØÏÔ
//    HAL_Delay(500);
//    Clear_Buffer();

		gggg_SendStr("AT+QGPS?\r\n");//²éÑ¯GPSÊÇ·ñ¿ªÆô
    HAL_Delay(500);
    strx=strstr((const char*)AtRxBuffer,(const char*)"+QGPS: 1");//GPSÒÑ¾­¿ªÆôÁË
    if(strx==NULL)
    {
     Clear_Buffer();
     gggg_SendStr("AT+QGPS=1\r\n");//¿ªÆôGPS±±¶·
     HAL_Delay(500);
    }
    gggg_SendStr("AT+QGPS=1\r\n");//¿ªÆôGPS±±¶·
    Clear_Buffer();
		
    /////////////////////////////////
    gggg_SendStr("AT+CPIN?\r\n");//¼ì²éSIM¿¨ÊÇ·ñÔÚÎ»
    HAL_Delay(500);
    strx=strstr((const char*)AtRxBuffer,(const char*)"+CPIN: READY");//²é¿´ÊÇ·ñ·µ»Øready
		errcount=0;
    while(strx==NULL)
    {
			  errcount++;
        Clear_Buffer();
        gggg_SendStr("AT+CPIN?\r\n");
        HAL_Delay(500);
        strx=strstr((const char*)AtRxBuffer,(const char*)"+CPIN: READY");//¼ì²éSIM¿¨ÊÇ·ñÔÚÎ»£¬µÈ´ı¿¨ÔÚÎ»£¬Èç¹û¿¨Ê¶±ğ²»µ½£¬Ê£ÓàµÄ¹¤×÷¾ÍÃ»·¨×öÁË
			  if(errcount>20)     //·ÀÖ¹ËÀÑ­»·
        {
            errcount = 0;
            //reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
            //NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
    }
    Clear_Buffer();
    ///////////////////////////////////////
//    gggg_SendStr("AT+CSQ\r\n"); //¼ì²éCSQ
//    HAL_Delay(500);
//    Clear_Buffer();
//    gggg_SendStr("ATI\r\n"); //¼ì²éÄ£¿éµÄ°æ±¾ºÅ
//    HAL_Delay(500);
//    Clear_Buffer();


    ///////////////////////////////////
    gggg_SendStr("AT+CREG?\r\n");//²é¿´ÊÇ·ñ×¢²áGSMÍøÂç
    HAL_Delay(500);
    strx=strstr((const char*)AtRxBuffer,(const char*)"+CREG: 0,1");//·µ»ØÕı³£
    extstrx=strstr((const char*)AtRxBuffer,(const char*)"+CREG: 0,5");//·µ»ØÕı³££¬ÂşÓÎ
    while(strx==NULL&&extstrx==NULL)
    { 
			  errcount++;
        Clear_Buffer();
        gggg_SendStr("AT+CREG?\r\n");//²é¿´ÊÇ·ñ×¢²áGSMÍøÂç
        HAL_Delay(500);
        strx=strstr((const char*)AtRxBuffer,(const char*)"+CREG: 0,1");//·µ»ØÕı³£
        extstrx=strstr((const char*)AtRxBuffer,(const char*)"+CREG: 0,5");//·µ»ØÕı³££¬ÂşÓÎ
			  if(errcount>20)     //·ÀÖ¹ËÀÑ­»·
        {
            errcount = 0;
            //reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
            //NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
			  
    }
    Clear_Buffer();

//    gggg_SendStr("AT+COPS?\r\n");//²é¿´×¢²áµ½ÄÄ¸öÔËÓªÉÌ£¬Ö§³ÖÒÆ¶¯ ÁªÍ¨ µçĞÅ
//    HAL_Delay(500);
//    Clear_Buffer();
    gggg_SendStr("AT+QICLOSE=0\r\n");//¹Ø±ÕsocketÁ¬½Ó
    HAL_Delay(1000);
    Clear_Buffer();
//    gggg_SendStr("AT+CIMI\r\n");//»ñÈ¡¿¨ºÅ£¬ÀàËÆÊÇ·ñ´æÔÚ¿¨µÄÒâË¼£¬±È½ÏÖØÒª¡£
//		HAL_Delay(1000);
//    strx=strstr((const char*)AtRxBuffer,(const char*)"OK");//Ö»Òª¿¨²»´íÎó »ù±¾¾Í³É¹¦
//    if(strx)
//    {
//        printf("============\r\nÎÒµÄ¿¨ºÅÊÇ : %s \r\n===============\r\n",AtRxBuffer);
//        HAL_Delay(1000);
//        Clear_Buffer();

//    }
//    else
//    {
//        // printf("¿¨´íÎó : %s \r\n",AtRxBuffer);
//        HAL_Delay(300);
//        Clear_Buffer();
//    }
//    Clear_Buffer();

    gggg_SendStr("AT+QIDEACT=1\r\n");//È¥¼¤»î
    HAL_Delay(1000);
    Clear_Buffer();

    gggg_SendStr("AT+QIACT=1\r\n");//¼¤»î
    HAL_Delay(500);
    Clear_Buffer();

    gggg_SendStr("AT+CGATT=1\r\n");//¼¤»îÍøÂç£¬PDP
    HAL_Delay(500);
    strx=strstr((const char*)AtRxBuffer,(const char*)"OK");//·µOK
    Clear_Buffer();

    gggg_SendStr("AT+CGATT?\r\n");//²éÑ¯¼¤»î×´Ì¬
    HAL_Delay(500);
    strx=strstr((const char*)AtRxBuffer,(const char*)"+CGATT: 1");//·µ1 ±íÃ÷¼¤»î³É¹¦ »ñÈ¡µ½IPµØÖ·ÁË
    Clear_Buffer();
    errcount = 0;
    while(strx==NULL)
    {
        errcount++;
        Clear_Buffer();
        gggg_SendStr("AT+CGATT?\r\n");//»ñÈ¡¼¤»î×´Ì¬
        HAL_Delay(500);
        strx=strstr((const char*)AtRxBuffer,(const char*)"+CGATT: 1");//·µ»Ø1,±íÃ÷×¢Íø³É¹¦
        if(errcount>20)     //·ÀÖ¹ËÀÑ­»·
        {
            errcount = 0;
            //reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
            //NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
    }

    gggg_SendStr("AT+CGPADDR=1\r\n");//»ñÈ¡µ±Ç°¿¨µÄIPµØÖ·
    HAL_Delay(500);
    Clear_Buffer();

    memset(atstr,0,BUFLEN);
    sprintf(atstr,"AT+QIOPEN=1,0,\042TCP\042,\042%s\042,%d,0,1\r\n",SERVERIP,SERVERPORT);//ÉèÖÃÎ·ÇªÍ¸´«Ä£Ê½
    gggg_SendStr(atstr);//·¢ËÍ socketIPºÍ¶Ë¿ÚºóÃæ¸ú¶ÔÓ¦Êı¾İ³¤¶ÈÒÔ¼°Êı¾İ
    HAL_Delay(500);
    strx=strstr((const char*)AtRxBuffer,(const char*)"+QIOPEN: 0,0");//¼ì²éÊÇ·ñµÇÂ½³É¹¦//"+QIOPEN: 0,0"
    errcount=0;
		if(strx!=NULL)
		{
		 Signal_State=1;
		}
    while(strx==NULL)
    {
			 
        errcount++;
        strx=strstr((const char*)AtRxBuffer,(const char*)"+QIOPEN: 0,0");//¼ì²éÊÇ·ñµÇÂ½³É¹¦ //CONNECT
        HAL_Delay(500);
        if(errcount>20)     //·ÀÖ¹ËÀÑ­»·
        {
					 Signal_State=0;
            errcount = 0;
            //reset_4g();
//            __set_FAULTMASK(1); //¹Ø±Õ×ÜÖĞ¶Ï
            //NVIC_SystemReset(); //ÇëÇóµ¥Æ¬»úÖØÆô
            break;
        }
    }
    HAL_Delay(500);
    Clear_Buffer();
}


//nmea_utc_time utc; 											//GPSĞÅÏ¢
nmea_msg gpsx;
//´ÓbufÀïÃæµÃµ½µÚcx¸ö¶ººÅËùÔÚµÄÎ»ÖÃ
//·µ»ØÖµ:0~0XFE,´ú±í¶ººÅËùÔÚÎ»ÖÃµÄÆ«ÒÆ.
//       0XFF,´ú±í²»´æÔÚµÚcx¸ö¶ººÅ							  
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//Óöµ½'*'»òÕß·Ç·¨×Ö·û,Ôò²»´æÔÚµÚcx¸ö¶ººÅ
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

//m^nº¯Êı
//·µ»ØÖµ:m^n´Î·½.
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//·ÖÎöGPRMCĞÅÏ¢
//gpsx:nmeaĞÅÏ¢½á¹¹Ìå
//buf:½ÓÊÕµ½µÄGPSÊı¾İ»º³åÇøÊ×µØÖ·
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,*p2,dx;			 
	u8 posx;     
	u32 temp;	   
	double rs;  
	p1=(u8*)strstr((const char *)buf,"GPRMC");//"$GPRMC",¾­³£ÓĞ&ºÍGPRMC·Ö¿ªµÄÇé¿ö,¹ÊÖ»ÅĞ¶ÏGPRMC.
	p2=(u8*)strstr((const char *)buf,"A");
	if(p2!=NULL){
	  Gps_State=1;   //ÅĞ¶ÏGPSÊı¾İÊÇ·ñÓĞĞ§
	}
	else {
	Gps_State=0;
	}
	
		posx=NMEA_Comma_Pos(p1,9);								//µÃµ½UTCÈÕÆÚ
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//µÃµ½UTCÈÕÆÚ
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
	
	posx=NMEA_Comma_Pos(p1,1);								//µÃµ½UTCÊ±¼ä
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//µÃµ½UTCÊ±¼ä,È¥µôms
		gpsx->utc.hour=temp/10000+8;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 
		if(gpsx->utc.hour>=24){
		gpsx->utc.hour=gpsx->utc.hour-24;
		gpsx->utc.date=gpsx->utc.date+1;
		} 
	}	
	
	posx=NMEA_Comma_Pos(p1,3);								//µÃµ½Î³¶È
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);	
		gpsx->latitude=(double)temp/NMEA_Pow(10,dx+2);	//µÃµ½¡ã
		rs = gpsx->latitude - (int)gpsx->latitude;
		gpsx->latitude = (int)gpsx->latitude + rs/0.6;
		//printf("latitude=%0.8f\r\n",gpsx->latitude);
		/*rs=temp%NMEA_Pow(10,dx+2);				//µÃµ½'		 
		//gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//×ª»»Îª¡ã*/ 
	}
	posx=NMEA_Comma_Pos(p1,4);								//ÄÏÎ³»¹ÊÇ±±Î³ 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//µÃµ½¾­¶È
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	
		gpsx->longitude=(double)temp/NMEA_Pow(10,dx+2);	//µÃµ½¡ã
		rs = gpsx->longitude - (int)gpsx->longitude;
		gpsx->longitude = (int)gpsx->longitude + rs/0.6;
    //printf("longitude=%0.8f\r\n",gpsx->longitude);
		//rs=temp%NMEA_Pow(10,dx+2);				//µÃµ½'		
		//gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//×ª»»Îª¡ã */
	}
	posx=NMEA_Comma_Pos(p1,6);								//¶«¾­»¹ÊÇÎ÷¾­
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	
	posx=NMEA_Comma_Pos(p1,7);								//³µËÙ
	if(posx!=0XFF)
   temp=NMEA_Str2num(p1+posx,&dx);		
		gpsx->speed=(float)temp;
	
	posx=NMEA_Comma_Pos(p1,8);								//³µµÄ·½Ïò
	if(posx!=0XFF) 
		temp=NMEA_Str2num(p1+posx,&dx)/10;
		gpsx->carangle=(float)temp;
}



//str×ª»»ÎªÊı×Ö,ÒÔ','»òÕß'*'½áÊø
//buf:Êı×Ö´æ´¢Çø
//dx:Ğ¡ÊıµãÎ»Êı,·µ»Ø¸øµ÷ÓÃº¯Êı
//·µ»ØÖµ:×ª»»ºóµÄÊıÖµ
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //µÃµ½ÕûÊıºÍĞ¡ÊıµÄ³¤¶È
	{
		if(*p=='-'){mask|=0X02;p++;}//ÊÇ¸ºÊı
		if(*p==','||(*p=='*'))break;//Óöµ½½áÊøÁË
		if(*p=='.'){mask|=0X01;p++;}//Óöµ½Ğ¡ÊıµãÁË
		else if(*p>'9'||(*p<'0'))	//ÓĞ·Ç·¨×Ö·û
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//È¥µô¸ººÅ
	for(i=0;i<ilen;i++)	//µÃµ½ÕûÊı²¿·ÖÊı¾İ
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//×î¶àÈ¡6Î»Ğ¡Êı
	*dx=flen;	 		//Ğ¡ÊıµãÎ»Êı
	for(i=0;i<flen;i++)	//µÃµ½Ğ¡Êı²¿·ÖÊı¾İ
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 

//GPS_INFO GPS;






												






