#include "spi.h"
#include "stm32f4xx.h"

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
//SPI3_CLK       PB3      
//SPI3_MOSI      PB5
//SPI3_MISO      PB4
void SPI3_Init(void)
{	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);               //  ʹ��GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);                //  ʹ��SPI2ʱ��

    //GPIOPB3��4��5��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;//  PB3��PB4��PB5���ù������	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                        //  ���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                      //  �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                  //  100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                        //  ����
    GPIO_Init(GPIOB, &GPIO_InitStructure);                              //  ��ʼ��

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);            //  PB10����Ϊ SPI2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3);            //  PB14����Ϊ SPI2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);            //  PB15����Ϊ SPI2

    //����ֻ���SPI�ڳ�ʼ��
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);                //  ��λSPI2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);               //  ֹͣ��λSPI2

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //  ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                //  ����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		            //  ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                    //  ����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                    //  ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                    //  NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//  ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //  ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	                        //  CRCֵ����Ķ���ʽ
    SPI_Init(SPI3, &SPI_InitStructure);                                 //  ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI3, ENABLE);                                              //  ʹ��SPI����

    SPI3_ReadWriteByte(0xff);//��������		 
}   

//SPI2 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
unsigned char SPI3_ReadWriteByte(unsigned char TxData)
{		 			 
    //  �ȴ���������  
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
    {
    
    }
	
	SPI_I2S_SendData(SPI3, TxData);                                     //  ͨ������SPIx����һ��byte  ����
		
    //�ȴ�������һ��byte  
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET)
    {
    
    } 
 
	return SPI_I2S_ReceiveData(SPI3);                                   //  ����ͨ��SPIx������յ�����	
 		    
}








