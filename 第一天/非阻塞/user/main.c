#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "beep.h"

char beep_flag;

int main(void)
{
    JTAG_SW_Config();   //关闭PA15的复用
    Systick_Init(72000);
    Led_Config();       //led初始化
    Beep_Config();      //beep初始化
    while(1)
    {
        //流水灯任务
        if(led_time > 500)
        {
            led_water();
            led_time = 0;
        }
        //蜂鸣器任务
        if(beep_time > 1500)
        {
            beep_flag = !beep_flag;
            if(beep_flag)
            {
                beep(1);
            }
            else
            {
                beep(0);
            }
            beep_time = 0;
            
        }
    }
}
