#include "stm32f4xx.h"
#include "dth11.h"
#include "delay.h"

/**********************************************************************************************************
函数名称：配置DHT11引脚为输出
输入参数：无
输出参数：无
函数返回：无
//DHT11_PIN         PE6
**********************************************************************************************************/
void DTH11_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/**********************************************************************************************************
函数名称：配置DHT11引脚为输入
输入参数：无
输出参数：无
函数返回：无
//DHT11_PIN         PE6
**********************************************************************************************************/
void DTH11_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //DHT11_PIN         PE6
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);               //  使能IO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);              //  使能SYSCFG时钟
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                        //  输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                        //  设置上拉
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/**********************************************************************************************************
函数名称：延时函数
输入参数：时间
输出参数：无
函数返回：无
**********************************************************************************************************/
void DTH11_Delay(unsigned int j)
{
    unsigned char i;

    for (; j > 0; j--)
    {
        for (i = 0; i < 250; i++);
    }
}

/**********************************************************************************************************
函数名称：10us延时函数
输入参数：时间
输出参数：无
函数返回：无
**********************************************************************************************************/
void Delay_10us(void)
{
    unsigned char i;
	
    i--;
    i--;
    i--;
    i--;
    i--;
    i--;
}

/**********************************************************************************************************
函数名称：10us延时函数
输入参数：时间
输出参数：无
函数返回：无
**********************************************************************************************************/
unsigned char DTH11_ReadByte(void)
{
    unsigned char i = 0;
    unsigned char temp = 0;
    unsigned char value = 0;
	
    for (i = 0; i < 8; i++)
    {
        DTH11_IN();
        while(!DHT11_READ());
       
		DTH11_Delay(4);
        
		temp = 0;
        
        if(DHT11_READ())
        {
            temp = 1;
        }

        while(DHT11_READ());

        
        value <<= 1;
        value |= temp;     
    }
    
    return value;
}

/**********************************************************************************************************
函数名称：10us延时函数
输入参数：时间
输出参数：无
函数返回：无
**********************************************************************************************************/
void DTH11_ReadData(unsigned char *temp, unsigned char *humi)
{
    unsigned char buffer[5] = {0};
    unsigned char checksum = 0;
    unsigned char i = 0;
    
    //主机拉低18ms
    DTH11_OUT();

    DHT11_L;

    DTH11_Delay(2600);

    DHT11_H;
	
    //总线由上拉电阻拉高 主机延时20us
    DTH11_Delay(3);
	
    //主机设为输入 判断从机响应信号
    //GPIO_SetBits(GPIOC, GPIO_Pin_3);
    DHT11_H;
	
    //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行
    DTH11_IN();
	
    if (!DHT11_READ())
    {        
		//判断从机是否发出 80us 的低电平响应信号是否结束
        while(!DHT11_READ()); 
		
		//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
        while(DHT11_READ());		
		
        //数据接收状态
        for (i = 0; i < 5; i++)
        {
            buffer[i] = DTH11_ReadByte();               
        }
        
        DTH11_OUT();
        DHT11_H;

        //数据校验
        checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
        
        if (checksum == buffer[4])
        {
            *humi = buffer[0];
            *temp = buffer[2];
        }
    }
}

