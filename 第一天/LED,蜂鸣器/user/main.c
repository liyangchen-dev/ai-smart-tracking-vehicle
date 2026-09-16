#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "beep.h"

int main(void)
{
    JTAG_SW_Config();
    Systick_Init(72000);
    Led_Config();   //led初始化
    Beep_Config();  //beep初始化
    
    while(1)
    {
        led_water();
        Delay_ms(500);
        beep(1);
        Delay_ms(500);
        beep(0);
        Delay_ms(500);
    }
}
