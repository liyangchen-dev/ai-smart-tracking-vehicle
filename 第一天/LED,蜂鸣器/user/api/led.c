#include "led.h"

void Led_Config(void)
{
    //ø™ ±÷”
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    //IO≈‰÷√
    GPIO_InitTypeDef Led_Struct;
    Led_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
    Led_Struct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    Led_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOC, &Led_Struct);
}

void led_water(void)
{
    static int n = 0;
    switch(n)
    {
        case 0: led1(1);led2(0);led3(0);break;
        case 1: led1(0);led2(1);led3(0);break;
        case 2: led1(0);led2(0);led3(1);break;
    }
    n++;
    n %= 3;
}
