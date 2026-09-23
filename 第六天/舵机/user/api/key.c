#include "key.h"

void Key_Config(void)
{
    //开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    //IO配置
    GPIO_InitTypeDef  KEY;
    
    KEY.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    KEY.GPIO_Pin = GPIO_Pin_0;
    
    GPIO_Init(GPIOA, &KEY);
}

uint8_t Key_value(void)
{
    static int count = 0;
    if(key1 == 0)
    {
        count++;
    }
    else
    {
        if(count > 100)//代表长按
        {
            count = 0;
            return 1;
        }
        else if(count > 10)//代表短按
        {
            count = 0;
            return 2;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}
