#ifndef __SR04_H
#define __SR04_H

#include "stm32f10x.h"

#define SR04_TRIG_CLK		RCC_APB2Periph_GPIOB
#define SR04_TRIG_PORT	GPIOB
#define SR04_TRIG_PIN		GPIO_Pin_14

#define SR04_ECHO_CLK		RCC_APB2Periph_GPIOC
#define SR04_ECHO_PORT	GPIOC
#define SR04_ECHO_PIN		GPIO_Pin_6

#define SR04_ECHO_PinSourcePort		GPIO_PortSourceGPIOC
#define SR04_ECHO_PinSource				GPIO_PinSource6
#define SR04_ECHO_EXTI_Line				EXTI_Line6

#define SR04_ECHO_IRQn						EXTI9_5_IRQn
#define SR04_ECHO_IRQHandler			EXTI9_5_IRQHandler




typedef struct{
	uint32_t sendCount;
	uint8_t recvCountFlag;
	uint32_t recvCount;
	float leng;	
}__SR04_TypeDef;

extern __SR04_TypeDef sr04;
extern uint8_t SteerDir;

void Sr04_Init(void);
void Sr04_Config(void);
void Sr04_SendTTL(void);
void Sr04_CountInc(void);
float Sr04_GetLength(void);
void Tim1Sr04_Config(void);
void Obstacle_Avoidance(void);


/*
 **********************************************
 以下程序是舵机控制程序
 ***********************************************/

typedef enum {
	SteerStop = 5,
	SteerLeft = 6,
	SteerRight = 7,
	SteerUp = 8,
	SteerDown = 9,
}__STEER_DIR;

void Steer_Init(void);
void Tim1_SteerInit(u16 psc, u16 arr, u16 ccr);
void Steer_SetPWM(uint8_t steer, uint16_t pwm) ;
void Steer_SetAngle(uint8_t steer, int angle) ;
void Steer_Control(void);

#endif
