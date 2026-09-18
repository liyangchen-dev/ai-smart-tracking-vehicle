#ifndef __BLU_H
#define __BLU_H

#include "stm32f10x.h"


extern uint8_t BluValue;

void Blue_Config(void);
void Blue_SendCMD(char cmd[]);
void Blue_SetName(char name[]);
void Blue_Analysis(void);
void Usart2_SendString(char str[]);
void BLE_SendStr(char *p);
#endif
