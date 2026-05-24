/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

	
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;  
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define data_frame_num 75  //����֡����
#define photo_name_num 19	 //ͼƬ���Ƴ���
//#define SERVERIP  "192.168.244.1"
//#define SERVERIP "101.200.212.234"   //IP��ַ
//#define SERVERPORT 2233              //�˿ں�

//#define SERVERIP "113.6.252.222"   //IP��ַ
//#define SERVERPORT 8087             //�˿ں�

//#define SERVERIP "43.143.130.64"   //IP��ַ
//#define SERVERPORT 8087             //�˿ں�

#define SERVERIP "43.143.198.120"   //IP��ַ
#define SERVERPORT 8087             //�˿ں�

#define Photo_interval  20   //���������
#define reconnect_tim   20   //��������ʱ����
#define e4G_fail_tim   3    //4G����ʧ�ܴ������ƣ��������ж�Ϊ�����ж�
#define tool_num 10     //���ߴ洢�������
#define tool_bits_width  7   //һ�������Ϣ��e2prom��ռ�ݵ��ֽ���
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_SDA_Pin GPIO_PIN_0
#define OLED_SDA_GPIO_Port GPIOF
#define OLED_SCL_Pin GPIO_PIN_1
#define OLED_SCL_GPIO_Port GPIOF
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SMG_COM1_Pin GPIO_PIN_13
#define SMG_COM1_GPIO_Port GPIOF
#define SMG_COM2_Pin GPIO_PIN_14
#define SMG_COM2_GPIO_Port GPIOF
#define SMG_COM3_Pin GPIO_PIN_15
#define SMG_COM3_GPIO_Port GPIOF
#define SMG_COM4_Pin GPIO_PIN_0
#define SMG_COM4_GPIO_Port GPIOG
#define SEG1_Pin GPIO_PIN_1
#define SEG1_GPIO_Port GPIOG
#define SEG2_Pin GPIO_PIN_7
#define SEG2_GPIO_Port GPIOE
#define SEG3_Pin GPIO_PIN_8
#define SEG3_GPIO_Port GPIOE
#define SEG4_Pin GPIO_PIN_9
#define SEG4_GPIO_Port GPIOE
#define SEG5_Pin GPIO_PIN_10
#define SEG5_GPIO_Port GPIOE
#define SEG6_Pin GPIO_PIN_11
#define SEG6_GPIO_Port GPIOE
#define SEG7_Pin GPIO_PIN_12
#define SEG7_GPIO_Port GPIOE
#define BLE_MODE_Pin GPIO_PIN_14
#define BLE_MODE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
