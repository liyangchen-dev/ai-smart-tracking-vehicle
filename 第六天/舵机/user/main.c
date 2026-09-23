#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "usart1.h"
#include "blu.h"
#include "lcd.h"
#include "motor.h"
#include "tcrt5000.h"
#include "sr04.h"
#include "wifi.h"

char beep_flag;
int key_mode;
int last_mode;
int key_value;
extern const unsigned char gImage_pic[40960];
extern const unsigned char gImage_pic1[40960];
extern const unsigned char gImage_pic2[40960];
extern const unsigned char gImage_pic3[40960];
int main(void)
{
    JTAG_SW_Config();   //关闭PA15的复用
    Systick_Init(72000);
    Led_Config();                //led初始化
    Beep_Config();               //beep初始化
    Key_Config();                //key初始化
    USART1_Config(115200);       //usart初始化
    Blue_Config();               //蓝牙初始化
    Blue_SetName("29car");
    LCD_Init();                  //LCD初始化
    MOTOR_Config();              //电机初始化
    Motor_Stop();                //电机停止
    TCRT5000_Config();           //循迹模块初始化
    Sr04_Init();                 //避障模块初始化
    Usart3_Config();             //WiFi与串口通信初始化
    printf("初始化完成\n");
    
    LCD_Fill(0, 0, 128, 160, BLUE);        //指定区域填充颜色
//    LCD_DrawLine(10, 10, 50, 50, RED);            //在指定位置画一条线
//    LCD_DrawRectangle(20, 20, 80, 80, LIGHTGREEN);    //在指定位置画一个矩形
//    Draw_Circle(55, 55, 20, BROWN);                      //在指定位置画一个圆
    
    LCD_ShowPicture(0,0,128,160,gImage_pic);
    LCD_ShowChinese(32,0,(u8*)"智能物联",BLACK,WHITE,16,1);//显示汉字串
    LCD_ShowChinese(32,32,(u8*)"李杨晨",BLACK,WHITE,16,1);//显示汉字串
    
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
                Motor_Stop();                //电机停止
            }
        key_time = 0;
        }
        if(key_mode == 1)
        {
            //循迹模式
            if(tcrt_time>30)
            {
                Find_Line();
                tcrt_time = 0;
            }
        }
        else if(key_mode == 2)
        {
            //蓝牙模式
            if(blu_time > 30)
            {
                Blue_Analysis();
                blu_time = 0;
            }
        }
        else if(key_mode == 3)
        {
            //避障模式
            if(sr04_time > 30)
            {
                Obstacle_Avoidance();       //避障函数
                Steer_Control();            //舵机控制
                sr04_time = 0;
            }
        }
        else if(key_mode == 4)
        {
            //wifi模式
            if(wifi_time > 30)
            {
                WIFI_Analysis();       //wifi指令分析
                wifi_time = 0;
            }
        }
        if(key_mode != last_mode)      // 只在切模式时重画
        {
            last_mode = key_mode;
            if(key_mode == 1) { LCD_ShowPicture(0,0,128,160,gImage_pic); LCD_ShowChinese(32,0,(u8*)"智能物联",BLACK,WHITE,16,1); LCD_ShowChinese(32,32,(u8*)"李杨晨",BLACK,WHITE,16,1); LCD_ShowChinese(32,140,(u8*)"循迹模式",BLACK,WHITE,16,1); }
            else if(key_mode == 2) { LCD_ShowPicture(0,0,128,160,gImage_pic1); LCD_ShowChinese(32,0,(u8*)"智能物联",BLACK,WHITE,16,1); LCD_ShowChinese(32,32,(u8*)"李杨晨",BLACK,WHITE,16,1);LCD_ShowChinese(32,140,(u8*)"蓝牙模式",BLACK,WHITE,16,1); }
            else if(key_mode == 3) { LCD_ShowPicture(0,0,128,160,gImage_pic2); LCD_ShowChinese(32,0,(u8*)"智能物联",BLACK,WHITE,16,1); LCD_ShowChinese(32,32,(u8*)"李杨晨",BLACK,WHITE,16,1);LCD_ShowChinese(32,140,(u8*)"避障模式",BLACK,WHITE,16,1); }
            else if(key_mode == 4) { LCD_ShowPicture(0,0,128,160,gImage_pic3); LCD_ShowChinese(32,0,(u8*)"智能物联",BLACK,WHITE,16,1); LCD_ShowChinese(32,32,(u8*)"李杨晨",BLACK,WHITE,16,1);LCD_ShowString(32,140,(u8*)"WIFI",BLACK,WHITE,16,1); LCD_ShowChinese(64,140,(u8*)"模式",BLACK,WHITE,16,1); }
        }
    }
}
