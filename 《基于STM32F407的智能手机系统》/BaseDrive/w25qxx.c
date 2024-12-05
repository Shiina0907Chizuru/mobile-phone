#include "w25qxx.h" 
#include "spi.h"
#include "delay.h"	   
#include "USART3.h"
#include "stm32f4xx.h"
#include "led.h"
#include "LCD.h"

unsigned short W25QXX_TYPE = W25Q16;	                                //  Ĭ����W25Q16
unsigned char WriteBuf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};			//���ڱ���д�뻺������
unsigned char ReadBuf[10]  = {0};										//���ڱ���������������
extern unsigned char Password1[3];     //����Ա����
//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector 
													 
//��ʼ��SPI FLASH��IO��
//W25QXX_CS     PB6
void W25QXX_Init(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;
 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);               //  ʹ��GPIOGʱ��

	//W25QXX_CS     PB6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	W25QXX_CS_H;
    
    SPI3_Init();		   			                                    //  ��ʼ��SPI
}  

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
unsigned char W25QXX_ReadSR(void)   
{  
	unsigned char byte = 0;   
    
	W25QXX_CS_L;
    
	SPI3_ReadWriteByte(W25X_ReadStatusReg);                             //  ���Ͷ�ȡ״̬�Ĵ�������    
	byte = SPI3_ReadWriteByte(0Xff);                                    //  ��ȡһ���ֽ�  
	W25QXX_CS_H;
    
	return byte;   
} 

//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(unsigned char sr)   
{   
	W25QXX_CS_L;
	
    SPI3_ReadWriteByte(W25X_WriteStatusReg);                            //  ����дȡ״̬�Ĵ�������    
	SPI3_ReadWriteByte(sr);                                             //  д��һ���ֽ�  
	
    W25QXX_CS_H;
}  

//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_WriteEnable);                               //  ����дʹ��  
	
    W25QXX_CS_H;
} 
//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_WriteDisable);                              //  ����д��ָֹ��    
    
    W25QXX_CS_H;
} 		
//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
unsigned short W25QXX_ReadID(void)
{
	unsigned short Temp = 0;
    
	W25QXX_CS_L;
    
	SPI3_ReadWriteByte(0x90);                                           //  ���Ͷ�ȡID����	    
	SPI3_ReadWriteByte(0x00); 	    
	SPI3_ReadWriteByte(0x00); 	    
	SPI3_ReadWriteByte(0x00); 	
    
	Temp |= SPI3_ReadWriteByte(0xFF) << 8;  
	Temp |= SPI3_ReadWriteByte(0xFF);
    
	W25QXX_CS_H;
    
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(unsigned char* pBuffer, unsigned int ReadAddr, unsigned short NumByteToRead)   
{ 
 	unsigned short i;
    
	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_ReadData);                                  //  ���Ͷ�ȡ����   
    SPI3_ReadWriteByte((u8)((ReadAddr) >> 16));                         //  ����24bit��ַ    
    SPI3_ReadWriteByte((u8)((ReadAddr) >> 8));   
    SPI3_ReadWriteByte((u8)ReadAddr);   
    
    for ( i = 0; i < NumByteToRead; i++ )
	{ 
        pBuffer[i] = SPI3_ReadWriteByte(0XFF);                          //  ѭ������  
    }
    
	W25QXX_CS_H; 
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite)
{
 	unsigned short i;  
    
    W25QXX_Write_Enable();                                              //  SET WEL 
	
    W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_PageProgram);                               //  ����дҳ����   
    SPI3_ReadWriteByte((u8)((WriteAddr) >> 16));                        //  ����24bit��ַ    
    SPI3_ReadWriteByte((u8)((WriteAddr) >> 8));   
    SPI3_ReadWriteByte((u8)WriteAddr);   
    
    for ( i = 0; i < NumByteToWrite; i++ )
    {
        SPI3_ReadWriteByte(pBuffer[i]);                                 //  ѭ��д��  
    }
	
    W25QXX_CS_H;
    
	W25QXX_Wait_Busy();					                                //  �ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite)   
{ 			 		 
	unsigned short pageremain;	 
    
	pageremain = 256 - WriteAddr % 256;                                 //  ��ҳʣ����ֽ���
    
	if ( NumByteToWrite <= pageremain )
    {
        pageremain = NumByteToWrite;                                    //  ������256���ֽ�
    }
    
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        
		if ( NumByteToWrite == pageremain )
        {
            break;                                                      //  д�������
        }
	 	else
		{
			pBuffer   += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			                    //  ��ȥ�Ѿ�д���˵��ֽ���
			
            if ( NumByteToWrite > 256 )
            {
                pageremain = 256;                                       //  һ�ο���д��256���ֽ�
            }
			else 
            {
                pageremain = NumByteToWrite; 	                        //  ����256���ֽ���
            }
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
unsigned char W25QXX_BUFFER[4096];		 
void W25QXX_Write(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite)   
{ 
	unsigned int secpos;
	unsigned short secoff;
	unsigned short secremain;	   
 	unsigned short i;    
	unsigned char* W25QXX_BUF;	  
    
   	W25QXX_BUF = W25QXX_BUFFER;	     
 	secpos = WriteAddr / 4096;                                          //  ������ַ  
	secoff = WriteAddr % 4096;                                          //  �������ڵ�ƫ��
	secremain = 4096 - secoff;                                          //  ����ʣ��ռ��С   

 	if ( NumByteToWrite <= secremain )
    {
        secremain = NumByteToWrite;                                     //  ������4096���ֽ�
    }
    
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);                   //  ������������������
        
		for (i = 0; i < secremain; i++ )                                //  У������
		{
			if ( W25QXX_BUF[secoff + i] != 0XFF )
            {
                break;                                                  //  ��Ҫ����  	  
            }
		}
        
		if (i < secremain)                                              //  ��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);                                //  �����������
			
            for ( i = 0; i < secremain; i++ )	                        //  ����
			{
				W25QXX_BUF[ i + secoff] = pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);      //  д����������  

		}
        else 
        {
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);        //  д�Ѿ������˵�,ֱ��д������ʣ������. 				   
        }
        
		if ( NumByteToWrite == secremain)
        {
            break;                                                      //  д�������
        }
		else                                                            //  д��δ����
		{
			secpos++;                                                   //  ������ַ��1
			secoff = 0;                                                 //  ƫ��λ��Ϊ0 	 

		   	pBuffer   += secremain;                                     //  ָ��ƫ��
			WriteAddr += secremain;                                     //  д��ַƫ��	   
		   	NumByteToWrite -= secremain;				                //  �ֽ����ݼ�
            
			if ( NumByteToWrite > 4096 )
            {
                secremain = 4096;	                                    //  ��һ����������д����
            }
			else 
            {
                secremain = NumByteToWrite;			                    //  ��һ����������д����
            }
		}	 
	};	 
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                                              //  SET WEL 
    W25QXX_Wait_Busy();   
  	
    W25QXX_CS_L;
    SPI3_ReadWriteByte(W25X_ChipErase);                                 //  ����Ƭ��������  
    
    W25QXX_CS_H;
    
	W25QXX_Wait_Busy();   				                                //  �ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(unsigned int Dst_Addr)   
{  
	//����falsh�������,������   
 	Dst_Addr *= 4096;
    W25QXX_Write_Enable();                                              //  SET WEL 	 
    W25QXX_Wait_Busy();   
  	
    W25QXX_CS_L;
    SPI3_ReadWriteByte(W25X_SectorErase);                               //  ������������ָ�� 
    SPI3_ReadWriteByte((u8)((Dst_Addr)>>16));                           //  ����24bit��ַ    
    SPI3_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI3_ReadWriteByte((u8)Dst_Addr);  
	
    W25QXX_CS_H;
    W25QXX_Wait_Busy();   				                                //  �ȴ��������
}  
//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);                                //  �ȴ�BUSYλ���
}

//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_PowerDown);                                 //  ���͵�������  
	
    W25QXX_CS_H;
    
    delay_us(3);                                                        //  �ȴ�TPD  
}   

//����
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_ReleasePowerDown);                          //  send W25X_PowerDown command 0xAB  
    
	W25QXX_CS_H;
    delay_us(3);                                                        //  �ȴ�TRES1
}   

/*************************************************
Function: W25QXX_TEST()
Description: //����д����������ܷ�ʹ��
			//����д��һ������2�ƣ�������3��
Input: null
Output: null
Return: null
Author: UU��
*************************************************/
void W25QXX_TEST(void)
{
	volatile unsigned char i = 0;  
    volatile unsigned char value = 0;
	W25QXX_Erase_Sector((SIZE - 100)/4096);
    W25QXX_Write_Page((u8*)WriteBuf, SIZE - 100, sizeof(WriteBuf));
    
    W25QXX_Read((u8*)ReadBuf, SIZE - 100, sizeof(WriteBuf));//sizeof(WriteBuf)
//    W25QXX_Write((u8*)WriteBuf, SIZE - 100, sizeof(WriteBuf));		     //  �ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����    
//    W25QXX_Read((u8*)ReadBuf, SIZE - 100, sizeof(WriteBuf));
    
    for (i = 0; i < 10; i++)
    {
        printf("READ0x%x\r\n", ReadBuf[i]);
        printf("WRITE0x%x\r\n", WriteBuf[i]);
        if (ReadBuf[i] != WriteBuf[i])
        {
            value = 1;
            
            break;
        }
    }
    
    if (value == 0)
    {
        LED2_ON;
    }
    else
    {
       LED3_ON;
    }
	W25QXX_Erase_Sector((SIZE - 100)/4096);
	// WriteBuf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};			//���ڱ���д�뻺������
	for(int j=0;j<10;j++){
		ReadBuf[j]  = 0;										//������ڱ���������������
	}
}
/*************************************************
Function: W25QXX_TEST()
Description: //�������Ա����
Input: null
Output: null
Return: null
Author: UU��
*************************************************/
void save_password(void)
{
	char nums[4];
	//��������д��flash����
	for(int i=0;i<3;i++){
		WriteBuf[i]=Password1[i];
		// sprintf(nums,"%d",WriteBuf[i]);
		// LCD_ShowString(100,420, (u8 *)"WriteBuf1",BLACK,WHITE);
        // LCD_ShowString(i*100+20,480,(u8*)nums,BLACK,WHITE);
	}
    delay_ms(1000);
	W25QXX_Erase_Sector((SIZE - 100)/4096);
    W25QXX_Write_Page((u8*)WriteBuf, SIZE - 100, sizeof(WriteBuf));
}
/*************************************************
Function: W25QXX_TEST()
Description: //��ȡ����Ա����
Input: null
Output: null
Return: null
Author: UU��
*************************************************/
void read_password(void)
{
	char nums[4];
	W25QXX_Read((u8*)ReadBuf, SIZE - 100, sizeof(WriteBuf));
	int flag=1;
	for(int i=0;i<3;i++){
		if(Password1[i]!=ReadBuf[i])	flag=0;
	}
    for(int i=0;i<3;i++){
		if(ReadBuf[i]==255)	flag=1;
	}
	if(flag==0){
		for(int i=0;i<3;i++){
			Password1[i]=ReadBuf[i];
			// sprintf(nums,"%d",Password1[i]);
			// LCD_ShowString(100,420, (u8 *)"Password1",BLACK,WHITE);
        	// LCD_ShowString(i*100+20,480,(u8*)nums,BLACK,WHITE);
		}
	}
}


















