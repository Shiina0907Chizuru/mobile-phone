#include "mp3.h"
#include "USART.h"
#include "delay.h"
#include "stm32f4xx.h"

unsigned char display_flag = 0;                                         //  显示标志位
unsigned char databuf[20] = {0};                                        //  数据缓冲区
unsigned char flag = 0;
u8 PWMflag=0;
unsigned char volume[6]  = {0x00,0x7E, 0x03, 0x06, 0x00, 0xEF};      //	音量 0-30
extern u32 opendoortime;
void setMp3Dev()
{
    unsigned char device[6]  = {0x00,0x7E, 0x03, 0x09, 0x00, 0xEF};      // 指定设备 0：U 4:FLASH
    USART6_Senddata(device, 6);
}

void setMp3Vol(unsigned char vol)
{
    
    volume[4] = vol;
    USART6_Senddata(volume, 6);
}

void dinggequ(void)
	{   
	  
    unsigned char gequ[6]  = {0x7E, 0x04, 0x03,0x00,0x00,0xEF}; 
	  USART6_Senddata(gequ, 6); 
  
		
}
	
void mp3Play(void)
{   
	
    unsigned char Play[4]  = {0x7E, 0x02, 0x0D, 0xEF};                  //	播放
    USART6_Senddata(Play, 4); 
}

void mp3Stop(void)
{
    unsigned char Stop[4]  = {0x7E, 0x02, 0x10, 0xEF};                  //	停止
    USART6_Senddata(Stop, 4); 
}



void Servo_init()
{
       GPIO_InitTypeDef            GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	TIM_OCInitTypeDef           TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	            //  TIM1时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	            //  使能PORTF时钟	
		
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                           //  GPIOA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                        //  复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	                //  速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                      //  推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                        //  上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);                              //  初始化PA8
	  
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 3360-1;                          //  定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;         //  向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 1000-1;                             //  自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                    //  初始化定时器10
	
	//  初始化TIM10 Channel1 PWM模式	 
    TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                   //  选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;       //  比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;           //  输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);                           //  根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);                  //  使能TIM1在CCR1上的预装载寄存器
 
    TIM_ARRPreloadConfig(TIM1, ENABLE);                                //  ARPE使能 
	TIM_Cmd(TIM1, ENABLE);        
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
}


void OpenDoor(unsigned char vol){
	

	 delay_ms(200);
    setMp3Dev();              //  U盘
    
    delay_ms(200);
    
    setMp3Vol(vol);                   //  设置音量
	delay_ms(200);
    
    setMp3Vol(vol);                   //  设置音量
    
    delay_ms(200); 
	  dinggequ();
	 delay_ms(200); 
		mp3Play();              //  播放
	PWMflag=1;
	opendoortime=5000;          //相当于delay作用，只是不打扰主程序进行罢了
//	while(PWMflag){
//		TIM_SetCompare1(TIM1, 90);
//	}
	// delay_ms(5000);
//	TIM_SetCompare1(TIM1, 30);
}
	