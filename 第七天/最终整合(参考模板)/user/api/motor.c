#include "motor.h"



//IA4 IB4		TIM3 CH1	2		PA6	7
//IA2 IB2		TIM3 CH3	4		PB0	1
//IA3 IB3		TIM4 CH1	2		PB6	7
//IA1 IB1		TIM4 CH3	4		PB8	9
//定时器周期1ms		占空比0
void MOTOR_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure = {0};
	TIM_OCInitTypeDef TIM_OCInitStructure = {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//分频因子：决定数字滤波器采样频率的参数，当用到输入捕获功能的时候，会用到这个参数
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_BaseInitStructure.TIM_Period = 999;	//重装载值
	TIM_BaseInitStructure.TIM_Prescaler = 71;	//预分频器
	TIM_TimeBaseInit(TIM3, &TIM_BaseInitStructure);
	TIM_ARRPreloadConfig(TIM3, ENABLE);//ARR预装载使能

	TIM_TimeBaseInit(TIM4, &TIM_BaseInitStructure);
	TIM_ARRPreloadConfig(TIM4, ENABLE);//ARR预装载使能
	
	//输出比较配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//PWM1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//高电平有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出到对应引脚
	TIM_OCInitStructure.TIM_Pulse = 0;				//比较值
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, ENABLE);//CCR预装载使能
	TIM_OC2PreloadConfig(TIM3, ENABLE);//CCR预装载使能
	TIM_OC3PreloadConfig(TIM3, ENABLE);//CCR预装载使能
	TIM_OC4PreloadConfig(TIM3, ENABLE);//CCR预装载使能


	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM4, ENABLE);//CCR预装载使能
	TIM_OC2PreloadConfig(TIM4, ENABLE);//CCR预装载使能
	TIM_OC3PreloadConfig(TIM4, ENABLE);//CCR预装载使能
	TIM_OC4PreloadConfig(TIM4, ENABLE);//CCR预装载使能

	TIM_Cmd(TIM3, ENABLE);	//使能定时器
	TIM_Cmd(TIM4, ENABLE);	//使能定时器
}


CarControl_Typedef car = {MAX_SPEED, CAR_DIR_STOP};



//小车切换运行状态耗电过高会复位
void Car_MotorControl(CarControl_Typedef c)
{
	if(c.speed > MAX_SPEED)	c.speed = MAX_SPEED;
	if(c.dir == CAR_DIR_STOP) {
		Motor_Stop();
	}
	else if(c.dir == CAR_DIR_FRONT) {
		Motor_Front(c.speed);
	}
	else if(c.dir == CAR_DIR_BACK) {
		Motor_Back(c.speed);
	}
	else if(c.dir == CAR_DIR_LEFT) {
		Motor_Left(c.speed);
	}
	else if(c.dir == CAR_DIR_RIGHT) {
		Motor_Right(c.speed);
	}
}
/**
  ******************************************************************************
  * 以下代码可以更加细致的控制小车电机
  ******************************************************************************
  */

//右前方电机控制
//传参如果速度 >=0 正转，否则反转
void Motor_RightFrontControl(int speed)
{
	if(speed >= 0) {
		Motor_Front_Right_F(speed);
	}
	else {
		Motor_Front_Right_B(-speed);
	}
}

//右后方电机控制
void Motor_RightBackControl(int speed)
{
	if(speed >= 0) {
		Motor_Back_Right_F(speed);
	}
	else {
		Motor_Back_Right_B(-speed);
	}
}

//左前方电机控制
void Motor_LeftFrontControl(int speed)
{
	if(speed >= 0) {
		Motor_Front_Left_F(speed);
	}
	else {
		Motor_Front_Left_B(-speed);
	}
}

//左后方电机控制
void Motor_LeftBackControl(int speed)
{
	if(speed >= 0) {
		Motor_Back_Left_F(speed);
	}
	else {
		Motor_Back_Left_B(-speed);
	}
}



