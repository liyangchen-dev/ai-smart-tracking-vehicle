#include "sr04.h"
#include "delay.h"
#include "motor.h"
#include "lcd.h"
#include "stdio.h"

//超声波初始化函数
void Sr04_Init(void)
{
	Sr04_Config();
	Tim1Sr04_Config();
}

//初始化SR04的IO口，以及对应的外部中断的配置
void Sr04_Config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	//开时钟
	RCC_APB2PeriphClockCmd(SR04_TRIG_CLK | SR04_ECHO_CLK, ENABLE);
	
	//发射引脚 通用推挽输出 GPIO_Mode_Out_PP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SR04_TRIG_PIN;
	GPIO_Init(SR04_TRIG_PORT, &GPIO_InitStructure);
	//接收引脚：GPIO_Mode_IN_FLOATING
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = SR04_ECHO_PIN;
	GPIO_Init(SR04_ECHO_PORT, &GPIO_InitStructure);	

	GPIO_ResetBits(SR04_TRIG_PORT, SR04_TRIG_PIN);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(SR04_ECHO_PinSourcePort, SR04_ECHO_PinSource);
  
  EXTI_InitStructure.EXTI_Line = SR04_ECHO_EXTI_Line;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断请求
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//设置输入线路下降沿为中断请求  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = SR04_ECHO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//先占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;//从优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

__SR04_TypeDef sr04 = {0};

//周期发送  10us 调用一次
//发送10us高59990us低   周期60000us  60ms
void Sr04_SendTTL(void) 
{
	sr04.sendCount++; //123
	if(sr04.sendCount == 1 && GPIO_ReadOutputDataBit(SR04_TRIG_PORT, SR04_TRIG_PIN) == Bit_RESET) {
		GPIO_SetBits(SR04_TRIG_PORT, SR04_TRIG_PIN);//拉高为高电平_触发信号开始
	}
	else if(sr04.sendCount == 3 && GPIO_ReadOutputDataBit(SR04_TRIG_PORT, SR04_TRIG_PIN) == Bit_SET) {
		GPIO_ResetBits(SR04_TRIG_PORT, SR04_TRIG_PIN);//触发信号结束
	}
	else if(sr04.sendCount > 6000){
		sr04.sendCount = 0;
		GPIO_ResetBits(SR04_TRIG_PORT, SR04_TRIG_PIN);
	}
}

//IO的EXTI中断服务函数
//只要检测到边沿就会触发
void SR04_ECHO_IRQHandler(void)
{
  if(EXTI_GetITStatus(SR04_ECHO_EXTI_Line) != RESET)
  {
    if(GPIO_ReadInputDataBit(SR04_ECHO_PORT, SR04_ECHO_PIN) == Bit_SET) {
			//改变开始开始计数标志  置1
			sr04.recvCount = 0;
			sr04.recvCountFlag = 1;
		}
		else {
			//计数完成
			sr04.recvCountFlag = 0;
		}
    EXTI_ClearITPendingBit(SR04_ECHO_EXTI_Line);
  }
}


//增加计数
//10us调用一次，用来计数
void Sr04_CountInc(void)
{
	if(sr04.recvCountFlag == 1)//高电平期间计数
		sr04.recvCount++;
}

//获取SR04数据,获取频率越快，计算越快
float Sr04_GetLength(void)
{
	if(sr04.recvCountFlag == 0)
		sr04.leng = sr04.recvCount*34/200.0;//距离单位：cm/10us
	if(sr04.leng > 400)	sr04.leng = 380;
	return sr04.leng;
}

//10us一次的定时器中断
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		Sr04_SendTTL();//发送脉冲波  10us发送一次
		Sr04_CountInc();//计数
		TIM_ClearFlag(TIM2, TIM_IT_Update);
	}
}

//定时器1的初始化，实现了10us进入一次中断
void Tim1Sr04_Config(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	//10us一次的中断   定时器1初始化
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseStructure.TIM_Prescaler = 36-1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 20-1;//20-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_CtrlPWMOutputs(TIM2, DISABLE);
}



/*
 **********************************************
 以下程序是舵机控制程序
 ***********************************************/
int steer1Angle = 0, steer2Angle = -90;
void Tim1_SteerInit(u16 psc, u16 arr, u16 ccr)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);
	
	// 输出比较通道 GPIO 初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/**********************定时器初始化************************/
	TIM_TimeBaseStructure.TIM_Period = arr - 1;// 自动重装载寄存器的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;// 驱动 CNT 计数器的时钟
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;// 重复计数器的值
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// 配置为 PWM 模式 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// 输出使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;// 互补输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// 输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;// 互补输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;// 输出通道空闲电平极性配置
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;// 互补输出通道空闲电平极性配置
	TIM_OCInitStructure.TIM_Pulse = (steer1Angle*(2000.0/180)+1500);// 设置占空比大小
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_Pulse = (steer2Angle*(-2000.0/180)+500);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
	NVIC_DisableIRQ(TIM1_UP_IRQn);
	
	// 使能计数器
	TIM_Cmd(TIM1, ENABLE);
	// 主输出使能
	TIM_CtrlPWMOutputs(TIM1, ENABLE);	
}

//舵机初始化
void Steer_Init(void)
{
	Tim1_SteerInit(72, 20000, 1500);
//	Steer_SetAngle(1, steer1Angle);
//	Steer_SetAngle(2, steer2Angle);
}

void Steer_SetPWM(uint8_t steer, uint16_t pwm) 
{
	switch(steer)
	{
		case 1: TIM_SetCompare1(TIM1, pwm); break;
		case 2: TIM_SetCompare4(TIM1, pwm); break;
	}
}

/*************************************************
 *Steer1:	 可控范围：-90° -- 90°
 *	500-2500	0°对应1500	-90°对应500	90°对应2500
 *
 *Steer2:  可控范围：0° -- -180°
 *	500-2500	0°对应500	 -180°对应2500
 *************************************************
 */
void Steer_SetAngle(uint8_t steer, int angle) 
{
	switch(steer)
	{
		case 1: 
			if(angle < -90 || angle > 90)	break;
			angle = angle*(2000.0/180)+1500;
			TIM_SetCompare1(TIM1, angle); 
		break;
		case 2: 
			if(angle < -180 || angle > 0)	break;
			angle = angle*(-2000.0/180)+500;
			TIM_SetCompare4(TIM1, angle); 
		break;
	}
}
//控制舵机的
uint8_t SteerDir;
void Steer_Control(void)//30ms调用一次，固定周期调用，用于匀速控制舵机转动
{
	switch(SteerDir) {//这个方向的变量，由蓝牙、WIFI可以控制改变
		case SteerStop:  break;
		case SteerLeft: steer1Angle+=2; break;
		case SteerRight: steer1Angle-=2; break;
		case SteerUp: steer2Angle+=2; break;
		case SteerDown: steer2Angle-=2; break;
		default:break;
	}
	steer1Angle = steer1Angle>90?90:(steer1Angle<-90?(-90):steer1Angle);  // 限幅
	steer2Angle = steer2Angle>0?0:(steer2Angle<-180?(-180):steer2Angle); 	// 限幅
	Steer_SetAngle(1, steer1Angle);
	Steer_SetAngle(2, steer2Angle);
}


/***********************************
  * @brief  避障函数
  * @param  None
  * @retval None
  ***********************************/
void Obstacle_Avoidance(void)
{
	char buf[30] = "\0";
	float length = 0;
	length = Sr04_GetLength();
	printf("length=%.2f\r\n",length);
	sprintf(buf,"length=%6.2f",length);
	LCD_ShowString(0,100,(u8*)buf,BLACK,PINK,16,0);
	if(length>50)                       //障碍物较远时
		Motor_Front(500);
	else if(length<=50&&length>15)      //检测到障碍物时
		Motor_Right(500);
	else                                //安全行驶
		Motor_Back(500);
}
