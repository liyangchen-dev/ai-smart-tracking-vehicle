#include "blu.h"
#include "stdio.h"
#include "string.h"
//#include "motor.h"
#include "beep.h"
#include "led.h"
#include "motor.h"

extern int key_mode;   //主程序当前模式

uint8_t blueBuff[16] = {0};
uint8_t recvCount = 0;
uint8_t recvflag = 0;

void USART2_IRQHandler(void)
{
    uint8_t data = 0;
    //判断是否完成接收中断
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        blueBuff[recvCount++] = USART2->DR; //接受中断
        recvCount %= 16;
        USART1->DR = USART2->DR;
    }
    if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)
    {
        data = USART2->DR; //接收数据
        recvflag = 1;
    }
}

//AA 55 指令
uint8_t BluValue = 0;
void Blue_Analysis(void)
{
	if(recvflag==1)
	{
		if(recvCount < 3)	goto ERROR;
		if(blueBuff[0] != 0xAA ||  blueBuff[1] != 0x55) goto ERROR;
	//	if(blueBuff[2]>=0 && blueBuff[2]<=9)
	//		BluValue = '0' + blueBuff[2];
		switch(blueBuff[2])
		{
			//具体指令分析
            case 0x01: beep(1); break;
            case 0x02: beep(0); break;
            case 0x06: Motor_Front(450); car.dir = CAR_DIR_FRONT; break;   //电机控制前进
            case 0x05: Motor_Back(450); car.dir = CAR_DIR_BACK; break;     //电机控制后退
            case 0x04: Motor_Left(450); car.dir = CAR_DIR_LEFT; break;     //电机控制左转
            case 0x03: Motor_Right(450); car.dir = CAR_DIR_RIGHT; break;   //电机控制右转
            case 0x07: Motor_Stop(); car.dir = CAR_DIR_STOP; break;        //电机控制停止
            case 0x08: key_mode++;                                         //蓝牙指令切换模式
                       if(key_mode == 5) key_mode = 1;
                       Motor_Stop();
                       car.dir = CAR_DIR_STOP;
                       break;
		}
		ERROR:
		recvCount = 0;
		memset(blueBuff, 0, sizeof(blueBuff));
		recvflag = 0;
	}
}



//蓝牙初始化
void Blue_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//配置IO口  PA2      PA3
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;					
	GPIO_Init(GPIOA, &GPIO_InitStructure);						
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;						 
	GPIO_Init(GPIOA, &GPIO_InitStructure);							 

	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;							
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		//全双工
	USART_InitStruct.USART_Parity = USART_Parity_No;		
	USART_InitStruct.USART_StopBits = USART_StopBits_1;	
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	
    //打开串口接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //接收中断使能
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //空闲中断使能
    NVIC_SetPriority(USART2_IRQn, 0);   //设置中断优先级
    NVIC_EnableIRQ(USART2_IRQn);        //使能NVIC管理
    
	USART_Cmd(USART2, ENABLE);//使能串口
}







//串口发送数据
void Usart2_SendDate(uint8_t data)
{
	USART_SendData(USART2, data);//串口数据发送
	//等待数据发送完成
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
	{}
}


//串口发送字符串
void Usart2_SendString(char str[])
{
	uint16_t i = 0;//数组下标计数
	while(str[i] != '\0') 
	{
		Usart2_SendDate(str[i]);
		i++;
	}
}


void Blue_SendCMD(char cmd[])
{
	Usart2_SendString(cmd);
}



void Blue_SetName(char name[])
{
	char buff[32] = {0};
	sprintf(buff,"AT+NAME%s\r\n", name);
	Usart2_SendString(buff);
}

void BLE_SendStr(char *p)
{
	while(*p != '\0')
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
		USART2->DR = *p;
		p++;
	}
}
