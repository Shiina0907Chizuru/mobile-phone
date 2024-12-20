#include "mp3.h"
#include "USART.h"
#include "delay.h"
#include "stm32f4xx.h"

unsigned char display_flag = 0;                                         //  ��ʾ��־λ
unsigned char databuf[20] = {0};                                        //  ���ݻ�����
unsigned char flag = 0;
u8 PWMflag=0;
unsigned char volume[6]  = {0x00,0x7E, 0x03, 0x06, 0x00, 0xEF};      //	���� 0-30
extern u32 opendoortime;
void setMp3Dev()
{
    unsigned char device[6]  = {0x00,0x7E, 0x03, 0x09, 0x00, 0xEF};      // ָ���豸 0��U 4:FLASH
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
	
    unsigned char Play[4]  = {0x7E, 0x02, 0x0D, 0xEF};                  //	����
    USART6_Senddata(Play, 4); 
}

void mp3Stop(void)
{
    unsigned char Stop[4]  = {0x7E, 0x02, 0x10, 0xEF};                  //	ֹͣ
    USART6_Senddata(Stop, 4); 
}



void Servo_init()
{
       GPIO_InitTypeDef            GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	TIM_OCInitTypeDef           TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	            //  TIM1ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	            //  ʹ��PORTFʱ��	
		
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                           //  GPIOA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                        //  ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	                //  �ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                      //  ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                        //  ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);                              //  ��ʼ��PA8
	  
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 3360-1;                          //  ��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;         //  ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 1000-1;                             //  �Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                    //  ��ʼ����ʱ��10
	
	//  ��ʼ��TIM10 Channel1 PWMģʽ	 
    TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                   //  ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;       //  �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;           //  �������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);                           //  ����Tָ���Ĳ�����ʼ������TIM1 4OC1

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);                  //  ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
 
    TIM_ARRPreloadConfig(TIM1, ENABLE);                                //  ARPEʹ�� 
	TIM_Cmd(TIM1, ENABLE);        
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
}


void OpenDoor(unsigned char vol){
	

	 delay_ms(200);
    setMp3Dev();              //  U��
    
    delay_ms(200);
    
    setMp3Vol(vol);                   //  ��������
	delay_ms(200);
    
    setMp3Vol(vol);                   //  ��������
    
    delay_ms(200); 
	  dinggequ();
	 delay_ms(200); 
		mp3Play();              //  ����
	PWMflag=1;
	opendoortime=5000;          //�൱��delay���ã�ֻ�ǲ�������������а���
//	while(PWMflag){
//		TIM_SetCompare1(TIM1, 90);
//	}
	// delay_ms(5000);
//	TIM_SetCompare1(TIM1, 30);
}
	