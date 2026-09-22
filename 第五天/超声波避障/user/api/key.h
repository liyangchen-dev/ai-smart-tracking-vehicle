#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"

#define key1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

void Key_Config(void);
uint8_t Key_value(void);

#endif
