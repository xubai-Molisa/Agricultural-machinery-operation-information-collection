#include "led.h"

LEDSTATE ledstate;


//=====================================数码管配置==============================================

void smg_off_all(void)
{
	com1_off();com2_off();com3_off();com4_off();
}

/*
亮一位数码管
输入 num   数码管的序号（1-4）
		 data  亮的值（0-9）
*/
void smg_onx(uint8_t num , uint8_t data)
{
	com1_off();com2_off();com3_off();com4_off();
	smga_off();smgb_off();smgc_off();smgd_off();smge_off();smgf_off();smgg_off();
	if(num>=1&&num<=4&&data<=10)
	{
		switch(data)
		{
			case 0:
				smga_on();smgb_on();smgc_on();smgd_on();smge_on();smgf_on();
				break;
			case 1:
				smgb_on();smgc_on();
				break;
			case 2:
				smga_on();smgb_on();smgd_on();smge_on();smgg_on();
				break;
			case 3:
				smga_on();smgb_on();smgc_on();smgd_on();smgg_on();
				break;
			case 4:
				smgf_on();smgb_on();smgg_on();smgc_on();
				break;
			case 5:
				smga_on();smgf_on();smgg_on();smgc_on();smgd_on();
				break;
			case 6:
				smgd_on();smgc_on();smgg_on();smge_on();smgf_on();
				break;
			case 7:
				smga_on();smgb_on();smgc_on();
				break;
			case 8:
				smga_on();smgb_on();smgg_on();smgf_on();smgc_on();smgd_on();smge_on();
				break;
			case 9:
				smga_on();smgb_on();smgg_on();smgf_on();smgc_on();smgd_on();
				break;
			case 10:
				smga_on();smgd_on();smge_on();smgf_on();smgg_on();
				break;
			default:
				break;
		}
		switch(num)
		{
			case 1:
				com1_on();
				break;
			case 2:
				com2_on();
				break;
			case 3:
				com3_on();
				break;
			case 4:
				com4_on();
				break;
			default:
				break;
		}

	}
}

/*
数码管显示数字
输入 num   数字  
范围 0-9999
不支持小数
需要将函数放在超过1ms的刷新中，利用余辉效应视觉延时
*/
void smg_num(uint16_t num)
{
	uint8_t numbuf[4]={0};
	static uint8_t i=0;
	if(num<=9999)
	{
		numbuf[3]=num%10;
		numbuf[2]=num/10%10;
		numbuf[1]=num/100%10;
		numbuf[0]=num/1000%10;
		smg_onx(i+1,numbuf[i]);
		if(i++>=4)
			i=0;
	}
}

 void smg_E(uint16_t num){
  com1_off();com2_off();com3_off();com4_off();
	smga_off();smgb_off();smgc_off();smgd_off();smge_off();smgf_off();smgg_off();
  smga_on();smgd_on();smge_on();smgf_on();smgg_on();
   switch(num)
		{
			case 1:
				com1_on();
				break;
			case 2:
				com2_on();
				break;
			case 3:
				com3_on();
				break;
			case 4:
				com4_on();
				break;
			default:
				break;
		}
}








