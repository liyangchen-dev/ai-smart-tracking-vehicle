#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "stm32f10x.h"
#include "delay.h"

#define MAX_SPEED	900

//方向定义
typedef enum{
	CAR_DIR_STOP = 0,
	CAR_DIR_FRONT = 1,
	CAR_DIR_LEFT = 2,
	CAR_DIR_RIGHT = 3,
	CAR_DIR_BACK = 4,
}CarDIR_Typedef;

//小车控制结构体
typedef struct {
	uint16_t speed;
	CarDIR_Typedef dir;
}CarControl_Typedef;

extern CarControl_Typedef car;

/*
												A					B
	右前		IA1  IB1		TIM4CH3		TIM4CH4		
	右后		IA2  IB2		TIM3CH3		TIM3CH4		
	左后		IA3  IB3		TIM4CH1		TIM4CH2		
	左前		IA4  IB4		TIM3CH1		TIM3CH2		
*/

//右前轮-前进														A												B																		
#define Motor_Front_Right_F(speed) 	(TIM_SetCompare3(TIM4, speed), TIM_SetCompare4(TIM4, 0))
//右前轮-后退														A												B																	
#define Motor_Front_Right_B(speed) 	(TIM_SetCompare3(TIM4, 0), TIM_SetCompare4(TIM4, speed))
//右后轮-前进														A												B																		
#define Motor_Back_Right_F(speed) 	(TIM_SetCompare3(TIM3, speed), TIM_SetCompare4(TIM3, 0))
//右后轮-后退														A												B																	
#define Motor_Back_Right_B(speed) 	(TIM_SetCompare3(TIM3, 0), TIM_SetCompare4(TIM3, speed))
																						
//左前轮-前进														A												B																		
#define Motor_Front_Left_F(speed) 	(TIM_SetCompare1(TIM3, speed), TIM_SetCompare2(TIM3, 0))
//左前轮-后退														A												B																	
#define Motor_Front_Left_B(speed) 	(TIM_SetCompare1(TIM3, 0), TIM_SetCompare2(TIM3, speed))
//左后轮-前进														A												B																		
#define Motor_Back_Left_F(speed) 	(TIM_SetCompare1(TIM4, speed), TIM_SetCompare2(TIM4, 0))
//左后轮-后退														A												B																	
#define Motor_Back_Left_B(speed) 	(TIM_SetCompare1(TIM4, 0), TIM_SetCompare2(TIM4, speed))


//轮子停止
#define Motor_Stop()		(Motor_Front_Right_F(0), Motor_Back_Right_F(0),\
												 Motor_Front_Left_F(0), Motor_Back_Left_F(0))
												 
#define Motor_Front(speed)	(Motor_Front_Right_F(speed), Motor_Back_Right_F(speed),\
														 Motor_Back_Left_F(speed), Motor_Front_Left_F(speed))

#define Motor_Back(speed)		(Motor_Front_Right_B(speed), Motor_Back_Right_B(speed),\
														 Motor_Back_Left_B(speed), Motor_Front_Left_B(speed))
														 
#define Motor_Right(speed)	(Motor_Front_Right_B(speed), Motor_Back_Right_B(speed),\
														 Motor_Back_Left_F(speed), Motor_Front_Left_F(speed))

#define Motor_Left(speed)		(Motor_Front_Right_F(speed), Motor_Back_Right_F(speed),\
														 Motor_Back_Left_B(speed), Motor_Front_Left_B(speed))

void MOTOR_Config(void);
void Car_MotorControl(CarControl_Typedef c);


void Motor_RightFrontControl(int speed);
void Motor_RightBackControl(int speed);
void Motor_LeftFrontControl(int speed);
void Motor_LeftBackControl(int speed);


#endif

