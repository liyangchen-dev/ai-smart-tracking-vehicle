#include "usart1.h"

//brr -- 波特率
void USART1_Config(u32 brr)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	//1.打开端口时钟A
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//2.配置PA10
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;//10管脚
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//3.配置PA9
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//10管脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//USART1 1+8+0+1 9600
	USART_InitTypeDef USART_InitSturct = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitSturct.USART_BaudRate = brr;//波特率
	USART_InitSturct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流
	USART_InitSturct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//全双工
	USART_InitSturct.USART_Parity = USART_Parity_No;//不使用奇偶校验位
	USART_InitSturct.USART_StopBits = USART_StopBits_1;//停止位为1
	USART_InitSturct.USART_WordLength = USART_WordLength_8b;//数据位为8
	
	USART_Init(USART1,&USART_InitSturct);
	
	USART_Cmd(USART1,ENABLE);
}

//printf
int fputc(int ch,FILE *fp)
{
	//等待串口上次数据发送完成
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
	//发送新的数据
	USART_SendData(USART1,ch);
	return ch;
}
//获取串口给发送的数据
char Get_Usart_Data(void)
{
	char data;
	//等待串口上次数据接收完成
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET);
	data = USART_ReceiveData(USART1);
	return data;
}

