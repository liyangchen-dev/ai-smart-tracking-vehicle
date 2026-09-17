#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "usart1.h"

char beep_flag;
int key_mode;
int key_value;

int main(void)
{
    JTAG_SW_Config();   //关闭PA15的复用
    Systick_Init(72000);
    Led_Config();                //led初始化
    Beep_Config();               //beep初始化
    Key_Config();                //key初始化
    USART1_Config(115200);       //usart初始化
    printf("初始化完成\n");
    while(1)
    {
        //流水灯任务
        if(led_time > 500)
        {
            led_water();
            led_time = 0;
        }
        //按键任务
        if(key_time > 10)
        {
            key_value = Key_value();
            if(key_value == 1)   //按键长按
            {
            beep_flag = !beep_flag;
                printf("按键长按\n");
                if(beep_flag)
                {
                    beep(1);
                }
                else
                {
                    beep(0);
                }
            }
            else if(key_value == 2)   //按键短按
            {
                key_mode++;
                printf("按键短按\n");
                if(key_mode == 5)
                {
                    key_mode = 1;
                }
            }
        key_time = 0;
        }
        if(key_mode == 1)
        {
            //循迹模式
        }
        else if(key_mode == 2)
        {
            //蓝牙模式
        }
        else if(key_mode == 3)
        {
            //避障模式
        }
        else if(key_mode == 4)
        {
            //wifi模式
        }
    }
}
