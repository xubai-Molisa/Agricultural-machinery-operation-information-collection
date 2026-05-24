#ifndef _LED_H_
#define _LED_H_

#include "main.h"

//=====================================流水灯配置==============================================
#define led1_on()				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_RESET)
#define led2_on()				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_RESET)


#define led1_off()				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_SET)
#define led2_off()				HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_SET)


#define led1_toggle()				HAL_GPIO_TogglePin(GPIOH,GPIO_PIN_2)
#define led2_toggle()				HAL_GPIO_TogglePin(GPIOH,GPIO_PIN_3)



typedef enum
{
	LED_ALL_TOGGLE,
	LED_WATER
}LEDSTATE;




//=====================================数码管配置==============================================


#define com1_on()           HAL_GPIO_WritePin(SMG_COM1_GPIO_Port,SMG_COM1_Pin,GPIO_PIN_SET)
#define com2_on()           HAL_GPIO_WritePin(SMG_COM2_GPIO_Port,SMG_COM2_Pin,GPIO_PIN_SET)
#define com3_on()           HAL_GPIO_WritePin(SMG_COM3_GPIO_Port,SMG_COM3_Pin,GPIO_PIN_SET)
#define com4_on()           HAL_GPIO_WritePin(SMG_COM4_GPIO_Port,SMG_COM4_Pin,GPIO_PIN_SET)

#define com1_off()           HAL_GPIO_WritePin(SMG_COM1_GPIO_Port,SMG_COM1_Pin,GPIO_PIN_RESET)
#define com2_off()           HAL_GPIO_WritePin(SMG_COM2_GPIO_Port,SMG_COM2_Pin,GPIO_PIN_RESET)
#define com3_off()           HAL_GPIO_WritePin(SMG_COM3_GPIO_Port,SMG_COM3_Pin,GPIO_PIN_RESET)
#define com4_off()           HAL_GPIO_WritePin(SMG_COM4_GPIO_Port,SMG_COM4_Pin,GPIO_PIN_RESET)

#define smga_on()           HAL_GPIO_WritePin(SEG1_GPIO_Port,SEG1_Pin,GPIO_PIN_SET)
#define smgb_on()           HAL_GPIO_WritePin(SEG2_GPIO_Port,SEG2_Pin,GPIO_PIN_SET)
#define smgc_on()           HAL_GPIO_WritePin(SEG3_GPIO_Port,SEG3_Pin,GPIO_PIN_SET)
#define smgd_on()           HAL_GPIO_WritePin(SEG4_GPIO_Port,SEG4_Pin,GPIO_PIN_SET)
#define smge_on()           HAL_GPIO_WritePin(SEG5_GPIO_Port,SEG5_Pin,GPIO_PIN_SET)
#define smgf_on()           HAL_GPIO_WritePin(SEG6_GPIO_Port,SEG6_Pin,GPIO_PIN_SET)
#define smgg_on()           HAL_GPIO_WritePin(SEG7_GPIO_Port,SEG7_Pin,GPIO_PIN_SET)

#define smga_off()           HAL_GPIO_WritePin(SEG1_GPIO_Port,SEG1_Pin,GPIO_PIN_RESET)
#define smgb_off()           HAL_GPIO_WritePin(SEG2_GPIO_Port,SEG2_Pin,GPIO_PIN_RESET)
#define smgc_off()           HAL_GPIO_WritePin(SEG3_GPIO_Port,SEG3_Pin,GPIO_PIN_RESET)
#define smgd_off()           HAL_GPIO_WritePin(SEG4_GPIO_Port,SEG4_Pin,GPIO_PIN_RESET)
#define smge_off()           HAL_GPIO_WritePin(SEG5_GPIO_Port,SEG5_Pin,GPIO_PIN_RESET)
#define smgf_off()           HAL_GPIO_WritePin(SEG6_GPIO_Port,SEG6_Pin,GPIO_PIN_RESET)
#define smgg_off()           HAL_GPIO_WritePin(SEG7_GPIO_Port,SEG7_Pin,GPIO_PIN_RESET)


//=====================================流水灯配置==============================================
void led_off_all(void);
void led_on_all(void);
void led_toggle_all(void);
void led_onx(uint8_t num);
//=====================================数码管配置==============================================
void smg_onx(uint8_t num , uint8_t data);
void smg_num(uint16_t num);
void smg_off_all(void);
void smg_E(uint16_t num);
#endif

