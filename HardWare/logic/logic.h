#ifndef _LOGIC_H_
#define _LOGIC_H_

#include "sensor.h"


extern uint8_t id[2];    //E2prom中读取数据缓存区

//模块初始化
void module_init(void);

//模块自检
extern u8 work_State;
extern u8 Signal_State;
extern u8 Gps_State;
extern u8 Ble_State;
extern u8 Sd_State;
extern u8 Photo_State;
extern u8 Angle_State;
extern u8 Selected_State;
void module_check(void);
void bar_refresh(void);
void oled_main_show(void);
extern  char file_name[50];
extern uint8_t host_id[2];

//机具适配
void tool_selected(void);
int tool_check(void);
//extern uint8_t height;
extern uint8_t initial_angle,angle_zf;
extern uint16_t length,width;
extern uint8_t all_toolid[tool_bits_width*tool_num];  //记录


#endif
