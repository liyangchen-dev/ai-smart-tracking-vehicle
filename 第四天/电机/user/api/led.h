#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"

#define led1(x) (x)?(GPIO_ResetBits(GPIOC,GPIO_Pin_0)):(GPIO_SetBits(GPIOC,GPIO_Pin_0))
#define led2(x) (x)?(GPIO_ResetBits(GPIOC,GPIO_Pin_1)):(GPIO_SetBits(GPIOC,GPIO_Pin_1))
#define led3(x) (x)?(GPIO_ResetBits(GPIOC,GPIO_Pin_2)):(GPIO_SetBits(GPIOC,GPIO_Pin_2))

void Led_Config(void);
void led_water(void);

#endif
