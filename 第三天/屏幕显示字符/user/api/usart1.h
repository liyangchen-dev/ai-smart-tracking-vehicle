#ifndef _UART_H_
#define _UART_H_

#include "stm32f10x.h"
#include "stdio.h"

void USART1_Config(u32 brr);
char Get_Usart_Data(void);
#endif
