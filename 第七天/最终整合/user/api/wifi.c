#include "wifi.h"
#include "string.h"
#include "stdio.h"
#include "sr04.h"
#include "motor.h"
__WIFI_TYPE wifi = {0};

void USART3_IRQHandler(void)
{
	uint8_t data = 0;
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)
	{
		data = USART3->DR;
		USART1->DR = USART3->DR;
		if(wifi.recvOver == 0) {
			wifi.recvData[wifi.recvCount++] = data;
		}
	}
	if(USART_GetFlagStatus(USART3, USART_FLAG_IDLE) == SET)
	{
		data = USART3->DR;
		wifi.recvOver = 1;
//		WIFI_Analysis();
	}	
}

void Usart3_Config(void)
{
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	//配置IO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//配置USART3
	USART_InitTypeDef USART_InitStructuer;
	USART_InitStructuer.USART_BaudRate = 9600;
	USART_InitStructuer.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructuer.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructuer.USART_Parity = USART_Parity_No;
	USART_InitStructuer.USART_StopBits = USART_StopBits_1;
	USART_InitStructuer.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStructuer);
	//使能接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	//配置NVIC
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	//使能串口
	USART_Cmd(USART3, ENABLE);
}

extern uint8_t SteerDir;
uint8_t wifiCarDir = 'O';
//解析wifi收到的数据
void WIFI_Analysis(void)
{
	uint8_t check = 0;
	if(wifi.recvOver == 0)	return;
	if(wifi.recvCount < 5)	goto err;
	if(wifi.recvData[0] != 0xAA || wifi.recvData[1] != 0x55)	goto err;
//	check = wifi.recvData[0] + wifi.recvData[1] + wifi.recvData[2] + wifi.recvData[3] + wifi.recvData[4];
//	if(check != wifi.recvData[5])	goto err;
//	wifi.connectFlag = 1;
	switch(wifi.recvData[2])
	{
		case 0x00: Motor_Stop(); car.dir = CAR_DIR_STOP; break;// 刹车
		case 0x01: Motor_Front(500); car.dir = CAR_DIR_FRONT; break;		// 前进
		case 0x02: Motor_Back(500); car.dir = CAR_DIR_BACK; break;		// 后退		
		case 0x03: Motor_Left(500); car.dir = CAR_DIR_LEFT; break;		// 左转		
		case 0x04: Motor_Right(500); car.dir = CAR_DIR_RIGHT; break;	// 右转
        case 0x0A: Motor_Right(350); car.dir = CAR_DIR_RIGHT; break;	// 速度:慢速
        case 0x0B: Motor_Right(500); car.dir = CAR_DIR_RIGHT; break;	// 速度:中速
        case 0x0C: Motor_Right(650); car.dir = CAR_DIR_RIGHT; break;	// 速度:快速
//		default:wifiCarDir = 'O';break;
	}
	SteerDir=wifi.recvData[3];
	
//	switch(wifi.recvData[3])
//	{
//		case 0x05: SteerDir = SteerStop; break;		// 停止
//		case 0x06: SteerDir = SteerLeft; break;		// 左转	
//		case 0x07: SteerDir = SteerRight; break;	// 右转		
//		case 0x08: SteerDir = SteerUp; break;			// 向上		
//		case 0x09: SteerDir = SteerDown; break;		// 向下	
//		//default:SteerDir = SteerStop;break;
//	}
//	
	err:
		memset(wifi.recvData, 0, 256);
		wifi.recvCount = 0;
		wifi.recvOver = 0;
}







