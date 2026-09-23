#include "tcrt5000.h"
#include "stdio.h"
#include "motor.h"
/***********************************
  * @brief  循迹初始化函数
  * @param  None
  * @retval None
  ***********************************/
void TCRT5000_Config(void)
{
	//PC7 PC8 PC9 PC10 PC11配置为浮空输入
	//打开C端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	//赋值
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
}
/***********************************
  * @brief  循迹实现函数
  * @param  None
  * @retval None
  ***********************************/
void Find_Line(void)
{
		switch(TCRT_VALUE)
		{
			case 0x1F://全黑      11111
			case 0x00://全白  停止 00000
				Motor_RightFrontControl(0);
				Motor_RightBackControl(0);
				Motor_LeftFrontControl(0);
				Motor_LeftBackControl(0);
				car.dir = CAR_DIR_STOP; 
			break;
			//左转直角弯
			case 0x1C://11100
				Motor_RightFrontControl(550);
				Motor_RightBackControl(550);
				Motor_LeftFrontControl(-300);
				Motor_LeftBackControl(-300);
				car.dir = CAR_DIR_LEFT;
			break;
			//左转大弯
			case 0x18://11000
			case 0x10://10000
				Motor_RightFrontControl(500);
				Motor_RightBackControl(500);
				Motor_LeftFrontControl(-250);
				Motor_LeftBackControl(-250);
				car.dir = CAR_DIR_LEFT;
			break;
			//左转小弯
			case 0x0C:	//01100	
			case 0x08:	//01000	
				Motor_RightFrontControl(500);
				Motor_RightBackControl(500);
				Motor_LeftFrontControl(-80);
				Motor_LeftBackControl(-80);		
				car.dir = CAR_DIR_LEFT;
			break;

			//右转直角弯
			case 0x07:	//00111
			
				Motor_RightFrontControl(-300);
				Motor_RightBackControl(-300);
				Motor_LeftFrontControl(550);
				Motor_LeftBackControl(550);
				car.dir = CAR_DIR_RIGHT;
			break;
			//右转大弯
			case 0x03://	00011
			case 0x01://	00001
				Motor_RightFrontControl(-250);
				Motor_RightBackControl(-250);
				Motor_LeftFrontControl(500);
				Motor_LeftBackControl(500);
				car.dir = CAR_DIR_RIGHT;
			break;
			//右转小湾
			case 0x06://00110
			case 0x02://00010
				Motor_RightFrontControl(-80);
				Motor_RightBackControl(-80);
				Motor_LeftFrontControl(500);
				Motor_LeftBackControl(500);
				car.dir = CAR_DIR_RIGHT;
			break;
			default: //直行
				Motor_RightFrontControl(500);
				Motor_RightBackControl(500);
				Motor_LeftFrontControl(500);
				Motor_LeftBackControl(500);
				car.dir = CAR_DIR_FRONT;
			break;	
		}
}
