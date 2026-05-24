/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  *************ge*****************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "logic.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float depth = 0.0;   //��ҵ���
	
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
 
	int photo_tim = Photo_interval + 1;  //��¼����ʱ����
	uint8_t e4g_tim;  //��¼4Gģ��������ʱ����
  char data_total_str[10];    //���������ַ��洢
  char data_pointer_str[10];   //����ָ���ַ��洢
	char photo_total_str[10];		//���ϴ�ͼƬ�����ַ��洢
	char photo_pointer_str[10];		//���ϴ�ͼƬָ���ַ��洢
  char str[50];   
  extern char *try1;
	double	latitude_before = 0;	//gpsx.latitude ά��
	double	latitude = 0;	//gpsx.latitude ά��
	double longitude_before = 0; //gpsx.longitude	����
	double longitude = 0; //gpsx.longitude	����
	double area = 0;      // ���m2		
	double areamu = 0;      // ��� Ķ
 	
	char picture_name[40];  //��Ƭ����·������
	char file_name[20] = "20230811";      // �洢����֡�ļ�����
	char file_name_date[40];  // �洢����֡�ļ�����
	char file_record[40];          // ��¼�ļ�����
	char file_failname[40];        // ���ϴ�ͼƬ����
	char s_picture_name[40];   //δ�ϴ���Ƭ���ֻ���
	char gpsrmc[]="$GPRMC,122417.00,A,4544.752328,N,12637.607085,E,2.3,317.8,110823,8.4,W,A*20";  //GPSģ��ɼ���Ϣ����
	char data_str[100];    //����֡
	char* result="00000";



  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_UART7_Init();
  MX_UART8_Init();
  MX_USART6_UART_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */


 	//1��ģ���ʼ��
	module_init();
	
	//2��ģ���Լ�
	module_check();

	uint8_t www[2]={31,41};  //3141Ϊ������
	uint8_t numid[1];
  I2C_EE_BufferWrite(www, 0, 2); 
//	I2C_EE_BufferRead(www, 0, 5); 
//	printf("www=%d %d %x %x %x\r\n",www[0],www[1],www[2],www[3],www[4]);
//  I2C_EE_BufferRead(numid, 2, 1);   //��e2prom�ж���һ���ж��ٻ����Ѿ�ƥ��
//	printf("numid=%d \r\n",numid[0]);

	//3����������
	tool_selected();
	
	//4����������
	bar_refresh();   //  ����ܺ�״̬����ʾ����
	oled_main_show();  // oled��������ʾ
	
	//OLED_ShowString(0, 10, " depth :",12,1);
//	OLED_ShowString(0, 24, " Photo :",12,1);
//	OLED_ShowString(0, 38, " area :",12,1);
  //OLED_ShowString(0, 52, " ble   :",12,1);
//	OLED_Refresh();
	
  HAL_TIM_Base_Start_IT(&htim2);
	
  // EC20Send_StrData("start\r\n");
	
	// 5�������ļ���
	// �ȴ�GPSģ���⵽GPS�ź�
	while(!Gps_State)
	{
//		EC20Get_GPSBD_Data();
//    strncpy(gpsrmc,&GPSRxBuffer[32],80); 
	  NMEA_GPRMC_Analysis(&gpsx,(u8 *)gpsrmc);  //����gps����
		sprintf(file_name,"%02d%02d%02d",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);
		sprintf(file_name_date,"0:%s/%2d%2d_%s.txt",file_name,host_id[0],host_id[1],file_name);
		sprintf(file_record,"%s/%s_record.txt",file_name,file_name);
		sprintf(file_failname,"%s/%s_Fail.txt",file_name,file_name);
	}
//	  // ��һ�δ�����¼�ļ���
//	 sprintf(file_name_date,"%s.txt",file_name);
//	 sprintf(file_record,"%s_record.txt",file_name);
//	 sprintf(file_failname,"%s_Fail.txt",file_name);
	
    // ��һ�δ�����¼�ļ���
		sprintf(file_name_date,"0:%s/%2d%2d_%s.txt",file_name,host_id[0],host_id[1],file_name);  // �����ı��ļ�·��
		sprintf(file_record,"%s/%s_record.txt",file_name,file_name);     // �����¼�ļ�·��
		sprintf(file_failname,"%s/%s_Fail.txt",file_name,file_name);     // �����ϴ�ʧ��ͼƬ����·��
	
	 if(f_mkdir(file_name) != FR_EXIST && f_mkdir(file_name) != FR_OK )
	 {
		Sd_State = 0;   // SD���洢ʧ��
	 }
	 printf("\r\nf_mkdir(file_name) = %d\r\n",f_mkdir(file_name));	 
	 f_open (&SDFile,file_record,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
	 if(f_size(&SDFile) == 0)
	 {
			f_lseek(&SDFile,0);
			f_write(&SDFile,0,10,&br);
			f_lseek(&SDFile,12);
			f_write(&SDFile,0,10,&br);
			f_lseek(&SDFile,24);
			f_read(&SDFile,0,10,&br);
			f_lseek(&SDFile,36);
			f_read(&SDFile,0,10,&br);
	 }
	 else 
	 {
		 // ��ȡ�ı��ļ�ָ��
			f_lseek(&SDFile,0);
			f_read(&SDFile,data_pointer_str,10,&br);
			f_lseek(&SDFile,12);
			f_read(&SDFile,data_total_str,10,&br);
			f_close(&SDFile);
			data_pointer=atoll(data_pointer_str);
			data_total=atoll(data_total_str);
		 
		 // ��ȡͼƬ�ļ�ָ��
			f_lseek(&SDFile,24);
			f_read(&SDFile,photo_pointer_str,10,&br);
			f_lseek(&SDFile,36);
			f_read(&SDFile,photo_total_str,10,&br);
			f_close(&SDFile);
			photo_pointer=atoll(photo_pointer_str);
			photo_total=atoll(photo_total_str);
	 }
	 f_close(&SDFile);
	
	
	
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		
//    // ��ȡ��¼�ļ��е�ָ������
//		f_open (&SDFile,file_record,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
//  	f_lseek(&SDFile,0);
//		f_read(&SDFile,data_pointer_str,10,&br);
//		f_lseek(&SDFile,12);
//		f_read(&SDFile,data_total_str,10,&br);
//		f_close(&SDFile);
//		data_pointer=atoll(data_pointer_str);
//		data_total=atoll(data_total_str);
//    printf("data_pointer=%10lld\r\n",data_pointer);
//    printf("data_total=%10lld\r\n",data_total);

				
   //��1����ȡGPS���ݲ���������
//		EC20Get_GPSBD_Data();
//    strncpy(gpsrmc,&GPSRxBuffer[32],80); 
	  NMEA_GPRMC_Analysis(&gpsx,(u8 *)gpsrmc);  //����gps����
		

		
		// ��2����ȡ��ǲ����������Ϣ
    Angle_State = 0;
    GetAngleData();  //��ȡ���1����
//		GetAngleData2();  //��ȡ���2����
		depth = GetDepthData(anglex,anglex2,initial_angle,angle_zf);  // ��ȡ�����Ϣ
		sprintf(str,"%0.2fcm  ",depth);
    OLED_ShowString(42, 11, (u8*)str,12,1);   //oled��ʾ���
		// printf("\r\n%d,%d,%f,%3d\r\n",length,initial_angle,anglex2,width);
		


		// ��3��������ҵ���
		if(depth >= 5)   // ɸѡ�켣��
		{
			latitude_before = latitude;
			longitude_before = longitude;
			latitude = gpsx.latitude;
			longitude = gpsx.longitude;
		}
		else
		{
			latitude_before = 0;
			longitude_before = 0;
			latitude = 0;
			longitude = 0;
		}
		
		
		if((latitude_before != 0) && (longitude_before != 0) && (latitude != 0) && (longitude != 0))
		{
			double dst;
			dst = get_distance(latitude_before, longitude_before, latitude, longitude)*1000;	//����m
			printf("dst:%f",dst);
//			double h = 3; //���m
			if(dst <= 50)
			{
				area = area + (width/100.0)*dst;  //width��λcm
				printf("width:%f",width/100.0);
				//areamu = area/666.67; // ƽ���׻����Ķ
		  }
			else 
			{
				area = area;
				//areamu = area/666.67;
			}
			sprintf(str,"%05.2fm2",area);
			printf("area:%f",area);
			OLED_ShowString(42, 39, (u8*)str,12,1);	
			//OLED_ShowChinese(79, 39, 17, 12, 1); // Ķ
		}
		else
		{
			OLED_ShowChinese(42, 39, 12, 12, 1);  // ��
//			OLED_ShowChinese(54, 40, 13, 12, 1);
//			OLED_ShowChinese(66, 40, 14, 12, 1);
			sprintf(str,"GPS!");
			OLED_ShowString(56, 39, (u8*)str,12,1);	
			//��ʾ�ַ���
      //x,y:�������  
      //size1:�����С 
      //*chr:�ַ�����ʼ��ַ 
      //mode:0,��ɫ��ʾ;1,������ʾ
		}
		
		
	  // ��4����ȡ�������ߺŲ��������Ƿ�ƥ��
    GetVoiceData();
		Selected_State = tool_check();  

	
   // ��5������֡�洢���ϴ�
// 	 sprintf(data_str,"hd+%2d%2d+%2x%2x+%06.2f+%4d%02d%02d%02d%02d%02d+%1c+%011.7f+%1c+%011.7f+%04.1f+%04.1f+ed\r\n",host_id[0],host_id[1],ZID[0],ZID[1],depth,gpsx.utc.year,gpsx.utc.month,gpsx.utc.date,gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec,gpsx.nshemi,gpsx.latitude,gpsx.ewhemi,gpsx.longitude,gpsx.carangle,gpsx.speed);
	 sprintf(data_str,"hd+%2d%2d+%2x%2x+%06.2f+%4d%02d%02d+%1c+%011.7f+%1c+%011.7f+%04.1f+%04.1f+%05.2f+ed\r\n",host_id[0],host_id[1],ZID[0],ZID[1],depth,gpsx.utc.year,gpsx.utc.month,gpsx.utc.date,gpsx.nshemi,gpsx.latitude,gpsx.ewhemi,gpsx.longitude,gpsx.carangle,gpsx.speed,area);
	 sd_saveq(data_str,file_name_date);
	 
	  // �ϴ�����֡
		if(data_total-data_pointer==1&&Signal_State)
		{
			f_open (&SDFile,file_name_date,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
			f_lseek(&SDFile,data_pointer*(data_frame_num+2));
			f_read(&SDFile,data_str,data_frame_num,&br);
			f_close(&SDFile);
			EC20Send_StrData(data_str);		
			
			//EC20Send_RecAccessMode();
		}
		else if(data_total-data_pointer>=2&&Signal_State)
		{
			f_open (&SDFile,file_name_date,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
			f_lseek(&SDFile,data_pointer*(data_frame_num+2));
			f_read(&SDFile,data_str,data_frame_num,&br);
			EC20Send_StrData(data_str);
			f_lseek(&SDFile,data_pointer*(data_frame_num+2));
			f_read(&SDFile,data_str,data_frame_num,&br);	
			f_close(&SDFile);
			EC20Send_StrData(data_str);	
      
      //EC20Send_RecAccessMode();			
		}
		else if(Signal_State==0)   //��������
		{
			e4g_tim++;
			if(e4g_tim >= reconnect_tim)   //��������ʱ����
				{
					e4g_tim=0;
//				  reset_4g();  //��λ4Gģ��
					EC20_Init();
					// ���Է�������
					f_open (&SDFile,file_name_date,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
					f_lseek(&SDFile,data_pointer*(data_frame_num+2));
					f_read(&SDFile,data_str,data_frame_num,&br);
					f_close(&SDFile);
					EC20Send_StrData(data_str);
					
					//EC20Send_RecAccessMode();

				}
			
		}
		
		//���仯����ָ�����ݴ洢��sd��
		sprintf(data_pointer_str,"%010lld",data_pointer);
		sprintf(data_total_str,"%010lld",data_total);
		f_open (&SDFile,file_record,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
		f_lseek(&SDFile,0);
		f_write(&SDFile,data_pointer_str,10,&numRW);
		f_lseek(&SDFile,12);
		f_write(&SDFile,data_total_str,10,&numRW);
		f_close(&SDFile);
		//printf("write_data_pointer=%10lld\r\n",data_pointer);
		//printf("write_data_total=%10lld\r\n",data_total);
		

		 // ��6��ͼƬ�洢���ϴ�
    OLED_ShowString(42, 25, (u8*)"  ",12,1);
		if(photo_tim >= Photo_interval)
		{
			photo_tim=0;
			Photo_State=0;   //�����־λ
			take_photo();
			sprintf(picture_name,"%s/%4d%02d%02d_%02d%02d%02d.jpg\r\n",file_name,gpsx.utc.year,gpsx.utc.month,gpsx.utc.date,gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);
			sd_savep(PicBuf,picture_name,camer.PicLen);
			OLED_ShowString(42, 25, (u8*)"OK ",12,1);
			
			//EC20Send_RecAccessMode();
			if(Signal_State)
			{
				EC20Send_ChangeMode(0);   //͸��ģʽ
  			EC20Send_PhotoData(PicBuf,camer.PicLen);
				//HAL_Delay(400);
//		  EC20Send_RecAccessMode();	
//			HAL_Delay(200);
//		  printf("1111:%s",try1);

//				f_open (&SDFile,file_record,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
//				f_lseek(&SDFile,24);
//				f_read(&SDFile,photo_pointer_str,10,&br);
//				f_lseek(&SDFile,36);
//				f_read(&SDFile,photo_total_str,10,&br);
//				f_close(&SDFile);
//				photo_pointer=atoll(photo_pointer_str);
//				photo_total=atoll(photo_total_str);
			
				
				//���������ϴ�δ�ϴ�ͼƬ
				if(photo_pointer < photo_total)
				{
					// �� ��ͼƬ����
					f_open (&SDFile,file_failname,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
					f_lseek(&SDFile,photo_pointer*(photo_name_num+2));
					f_read(&SDFile,s_picture_name,photo_name_num,&br);
					f_close(&SDFile);
					
					// �� �������ƶ�ͼƬ�ϴ�
					f_open (&SDFile,s_picture_name,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
					f_read(&SDFile,PicBuf,camer.PicLen,&br);
//					EC20Send_PData(PicBuf,camer.PicLen);
					
				  EC20Send_PhotoData(PicBuf,camer.PicLen);
					//HAL_Delay(200);
//				  EC20Send_RecAccessMode();	
//			    HAL_Delay(200);
//		      printf("1111:%s",try1);
					
					
					f_close(&SDFile);
					
					// �� ����Ƭָ���һ
					photo_pointer = photo_pointer + 1;
					//printf("\r\n��PHOTO����photo_pointer=%10lld\r\n",photo_pointer);
					//printf("\r\n��PHOTO��photo_total=%10lld\r\n",photo_total);
					
					sprintf(photo_pointer_str,"%010lld",photo_pointer);
					sprintf(photo_total_str,"%010lld",photo_total);
					f_open (&SDFile,file_record,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
					f_lseek(&SDFile,24);
					f_write(&SDFile,photo_pointer_str,10,&numRW);
					f_lseek(&SDFile,36);
					f_write(&SDFile,photo_total_str,10,&numRW);
					f_close(&SDFile);
				}
				
				HAL_Delay(400);  // �������ʱ������ģʽ�л���ʧ��
				EC20Send_ChangeMode(1);	//����ģʽ
				
			}
			else
			{
				//��ͼƬ���ƴ���SD��
				if(gpsx.utc.year != 2000)//��GPS�ź�ʱ
				{
					sd_savepq(picture_name,file_failname);
					photo_total = photo_total + 1;
					
					sprintf(photo_pointer_str,"%010lld",photo_pointer);
					sprintf(photo_total_str,"%010lld",photo_total);
					f_open (&SDFile,file_record,FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
					f_lseek(&SDFile,24);
					f_write(&SDFile,photo_pointer_str,10,&numRW);
					f_lseek(&SDFile,36);
					f_write(&SDFile,photo_total_str,10,&numRW);
					f_close(&SDFile);

					//printf("\r\n��PHOTO����photo_total=%10lld\r\n",photo_total);
					//printf("\r\n��PHOTO��photo_pointer=%10lld\r\n",photo_pointer);
				}
			}
//			result = EC20Send_RecAccessMode();	
//			HAL_Delay(200);
//		  printf("1111:%s",try1);
			EC20Send_RecAccessMode();	
			HAL_Delay(400);
		  printf("1111:%s",try1);
		}
		 // HAL_Delay(100);
		
		// ����

//		if (result!= NULL) {
//        OLED_ShowString(64, 53, (u8 *)try1,12,1);
//    }
//		
		//  ��7��ˢ����ʾ
		bar_refresh();  
		photo_tim++;  // ��¼����ʱ��
  }
  
	
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
