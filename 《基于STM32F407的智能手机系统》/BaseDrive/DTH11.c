#include "stm32f4xx.h"
#include "dth11.h"
#include "delay.h"

/**********************************************************************************************************
�������ƣ�����DHT11����Ϊ���
�����������
�����������
�������أ���
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
�������ƣ�����DHT11����Ϊ����
�����������
�����������
�������أ���
//DHT11_PIN         PE6
**********************************************************************************************************/
void DTH11_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //DHT11_PIN         PE6
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);               //  ʹ��IOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);              //  ʹ��SYSCFGʱ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                        //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                        //  ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/**********************************************************************************************************
�������ƣ���ʱ����
���������ʱ��
�����������
�������أ���
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
�������ƣ�10us��ʱ����
���������ʱ��
�����������
�������أ���
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
�������ƣ�10us��ʱ����
���������ʱ��
�����������
�������أ���
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
�������ƣ�10us��ʱ����
���������ʱ��
�����������
�������أ���
**********************************************************************************************************/
void DTH11_ReadData(unsigned char *temp, unsigned char *humi)
{
    unsigned char buffer[5] = {0};
    unsigned char checksum = 0;
    unsigned char i = 0;
    
    //��������18ms
    DTH11_OUT();

    DHT11_L;

    DTH11_Delay(2600);

    DHT11_H;
	
    //������������������ ������ʱ20us
    DTH11_Delay(3);
	
    //������Ϊ���� �жϴӻ���Ӧ�ź�
    //GPIO_SetBits(GPIOC, GPIO_Pin_3);
    DHT11_H;
	
    //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������
    DTH11_IN();
	
    if (!DHT11_READ())
    {        
		//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
        while(!DHT11_READ()); 
		
		//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
        while(DHT11_READ());		
		
        //���ݽ���״̬
        for (i = 0; i < 5; i++)
        {
            buffer[i] = DTH11_ReadByte();               
        }
        
        DTH11_OUT();
        DHT11_H;

        //����У��
        checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
        
        if (checksum == buffer[4])
        {
            *humi = buffer[0];
            *temp = buffer[2];
        }
    }
}

