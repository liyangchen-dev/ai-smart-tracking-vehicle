#include "delay.h"


//系统滴答定时器的中断函数
//1ms
void SysTick_Handler(void)
{
	
}




//系统定时器初始化
void Systick_Init(uint32_t load)
{
	if(SysTick_Config(load) == 1)
	{
		while(1);
	}
}


void Delay_us(uint32_t time)
{
	while(time--) {
		delay_1us();
	}
}

void Delay_ms(uint32_t time)
{
	uint64_t t = time*1000;
	while(t--) {
		delay_1us();
	}
}






