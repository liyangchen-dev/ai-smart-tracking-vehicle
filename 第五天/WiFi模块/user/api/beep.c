#include "beep.h"

void Beep_Config(void)
{
    //开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    //IO配置
    GPIO_InitTypeDef Beep_Struct;
    
    Beep_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
    Beep_Struct.GPIO_Pin = GPIO_Pin_15;
    Beep_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &Beep_Struct);
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}

//厂家测试接口
void JTAG_SW_Config(void)
{
	//关闭JTAG接口，开启SW接口--PA15、PB3、PB4可用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}
