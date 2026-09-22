#ifndef _TCRT5000_H_
#define _TCRT5000_H_

#include "stm32f10x.h"

#define OUT1 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
#define OUT2 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
#define OUT3 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)
#define OUT4 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)
#define OUT5 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)

#define TCRT_VALUE ((OUT5<<4)|(OUT4<<3)|(OUT3<<2)|(OUT2<<1)|(OUT1<<0))      //例如:000*1 1111*, 用了5位
void TCRT5000_Config(void);
void Find_Line(void);
#endif
