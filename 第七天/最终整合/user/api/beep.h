#ifndef _BEEP_H_
#define _BEEP_H_

#include "stm32f10x.h"

#define beep(x) (x)?(GPIO_SetBits(GPIOA,GPIO_Pin_15)):(GPIO_ResetBits(GPIOA,GPIO_Pin_15))

void Beep_Config(void);
void JTAG_SW_Config(void);

#endif
