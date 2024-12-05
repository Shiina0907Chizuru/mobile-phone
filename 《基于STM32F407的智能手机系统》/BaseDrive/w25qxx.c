#include "w25qxx.h" 
#include "spi.h"
#include "delay.h"	   
#include "USART3.h"
#include "stm32f4xx.h"
#include "led.h"
#include "LCD.h"

unsigned short W25QXX_TYPE = W25Q16;	                                //  默认是W25Q16
unsigned char WriteBuf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};			//用于表述写入缓存数组
unsigned char ReadBuf[10]  = {0};										//用于表述读出缓存数组
extern unsigned char Password1[3];     //管理员密码
//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector 
													 
//初始化SPI FLASH的IO口
//W25QXX_CS     PB6
void W25QXX_Init(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;
 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);               //  使能GPIOG时钟

	//W25QXX_CS     PB6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	W25QXX_CS_H;
    
    SPI3_Init();		   			                                    //  初始化SPI
}  

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
unsigned char W25QXX_ReadSR(void)   
{  
	unsigned char byte = 0;   
    
	W25QXX_CS_L;
    
	SPI3_ReadWriteByte(W25X_ReadStatusReg);                             //  发送读取状态寄存器命令    
	byte = SPI3_ReadWriteByte(0Xff);                                    //  读取一个字节  
	W25QXX_CS_H;
    
	return byte;   
} 

//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_Write_SR(unsigned char sr)   
{   
	W25QXX_CS_L;
	
    SPI3_ReadWriteByte(W25X_WriteStatusReg);                            //  发送写取状态寄存器命令    
	SPI3_ReadWriteByte(sr);                                             //  写入一个字节  
	
    W25QXX_CS_H;
}  

//W25QXX写使能	
//将WEL置位   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_WriteEnable);                               //  发送写使能  
	
    W25QXX_CS_H;
} 
//W25QXX写禁止	
//将WEL清零  
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_WriteDisable);                              //  发送写禁止指令    
    
    W25QXX_CS_H;
} 		
//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
unsigned short W25QXX_ReadID(void)
{
	unsigned short Temp = 0;
    
	W25QXX_CS_L;
    
	SPI3_ReadWriteByte(0x90);                                           //  发送读取ID命令	    
	SPI3_ReadWriteByte(0x00); 	    
	SPI3_ReadWriteByte(0x00); 	    
	SPI3_ReadWriteByte(0x00); 	
    
	Temp |= SPI3_ReadWriteByte(0xFF) << 8;  
	Temp |= SPI3_ReadWriteByte(0xFF);
    
	W25QXX_CS_H;
    
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(unsigned char* pBuffer, unsigned int ReadAddr, unsigned short NumByteToRead)   
{ 
 	unsigned short i;
    
	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_ReadData);                                  //  发送读取命令   
    SPI3_ReadWriteByte((u8)((ReadAddr) >> 16));                         //  发送24bit地址    
    SPI3_ReadWriteByte((u8)((ReadAddr) >> 8));   
    SPI3_ReadWriteByte((u8)ReadAddr);   
    
    for ( i = 0; i < NumByteToRead; i++ )
	{ 
        pBuffer[i] = SPI3_ReadWriteByte(0XFF);                          //  循环读数  
    }
    
	W25QXX_CS_H; 
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void W25QXX_Write_Page(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite)
{
 	unsigned short i;  
    
    W25QXX_Write_Enable();                                              //  SET WEL 
	
    W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_PageProgram);                               //  发送写页命令   
    SPI3_ReadWriteByte((u8)((WriteAddr) >> 16));                        //  发送24bit地址    
    SPI3_ReadWriteByte((u8)((WriteAddr) >> 8));   
    SPI3_ReadWriteByte((u8)WriteAddr);   
    
    for ( i = 0; i < NumByteToWrite; i++ )
    {
        SPI3_ReadWriteByte(pBuffer[i]);                                 //  循环写数  
    }
	
    W25QXX_CS_H;
    
	W25QXX_Wait_Busy();					                                //  等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite)   
{ 			 		 
	unsigned short pageremain;	 
    
	pageremain = 256 - WriteAddr % 256;                                 //  单页剩余的字节数
    
	if ( NumByteToWrite <= pageremain )
    {
        pageremain = NumByteToWrite;                                    //  不大于256个字节
    }
    
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        
		if ( NumByteToWrite == pageremain )
        {
            break;                                                      //  写入结束了
        }
	 	else
		{
			pBuffer   += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			                    //  减去已经写入了的字节数
			
            if ( NumByteToWrite > 256 )
            {
                pageremain = 256;                                       //  一次可以写入256个字节
            }
			else 
            {
                pageremain = NumByteToWrite; 	                        //  不够256个字节了
            }
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
unsigned char W25QXX_BUFFER[4096];		 
void W25QXX_Write(unsigned char* pBuffer, unsigned int WriteAddr, unsigned short NumByteToWrite)   
{ 
	unsigned int secpos;
	unsigned short secoff;
	unsigned short secremain;	   
 	unsigned short i;    
	unsigned char* W25QXX_BUF;	  
    
   	W25QXX_BUF = W25QXX_BUFFER;	     
 	secpos = WriteAddr / 4096;                                          //  扇区地址  
	secoff = WriteAddr % 4096;                                          //  在扇区内的偏移
	secremain = 4096 - secoff;                                          //  扇区剩余空间大小   

 	if ( NumByteToWrite <= secremain )
    {
        secremain = NumByteToWrite;                                     //  不大于4096个字节
    }
    
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);                   //  读出整个扇区的内容
        
		for (i = 0; i < secremain; i++ )                                //  校验数据
		{
			if ( W25QXX_BUF[secoff + i] != 0XFF )
            {
                break;                                                  //  需要擦除  	  
            }
		}
        
		if (i < secremain)                                              //  需要擦除
		{
			W25QXX_Erase_Sector(secpos);                                //  擦除这个扇区
			
            for ( i = 0; i < secremain; i++ )	                        //  复制
			{
				W25QXX_BUF[ i + secoff] = pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);      //  写入整个扇区  

		}
        else 
        {
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);        //  写已经擦除了的,直接写入扇区剩余区间. 				   
        }
        
		if ( NumByteToWrite == secremain)
        {
            break;                                                      //  写入结束了
        }
		else                                                            //  写入未结束
		{
			secpos++;                                                   //  扇区地址增1
			secoff = 0;                                                 //  偏移位置为0 	 

		   	pBuffer   += secremain;                                     //  指针偏移
			WriteAddr += secremain;                                     //  写地址偏移	   
		   	NumByteToWrite -= secremain;				                //  字节数递减
            
			if ( NumByteToWrite > 4096 )
            {
                secremain = 4096;	                                    //  下一个扇区还是写不完
            }
			else 
            {
                secremain = NumByteToWrite;			                    //  下一个扇区可以写完了
            }
		}	 
	};	 
}
//擦除整个芯片		  
//等待时间超长...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                                              //  SET WEL 
    W25QXX_Wait_Busy();   
  	
    W25QXX_CS_L;
    SPI3_ReadWriteByte(W25X_ChipErase);                                 //  发送片擦除命令  
    
    W25QXX_CS_H;
    
	W25QXX_Wait_Busy();   				                                //  等待芯片擦除结束
}   
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(unsigned int Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
 	Dst_Addr *= 4096;
    W25QXX_Write_Enable();                                              //  SET WEL 	 
    W25QXX_Wait_Busy();   
  	
    W25QXX_CS_L;
    SPI3_ReadWriteByte(W25X_SectorErase);                               //  发送扇区擦除指令 
    SPI3_ReadWriteByte((u8)((Dst_Addr)>>16));                           //  发送24bit地址    
    SPI3_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI3_ReadWriteByte((u8)Dst_Addr);  
	
    W25QXX_CS_H;
    W25QXX_Wait_Busy();   				                                //  等待擦除完成
}  
//等待空闲
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);                                //  等待BUSY位清空
}

//进入掉电模式
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_PowerDown);                                 //  发送掉电命令  
	
    W25QXX_CS_H;
    
    delay_us(3);                                                        //  等待TPD  
}   

//唤醒
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS_L;
    
    SPI3_ReadWriteByte(W25X_ReleasePowerDown);                          //  send W25X_PowerDown command 0xAB  
    
	W25QXX_CS_H;
    delay_us(3);                                                        //  等待TRES1
}   

/*************************************************
Function: W25QXX_TEST()
Description: //测试写入擦除功能能否使用
			//读和写不一样则亮2灯，正常亮3灯
Input: null
Output: null
Return: null
Author: UU酱
*************************************************/
void W25QXX_TEST(void)
{
	volatile unsigned char i = 0;  
    volatile unsigned char value = 0;
	W25QXX_Erase_Sector((SIZE - 100)/4096);
    W25QXX_Write_Page((u8*)WriteBuf, SIZE - 100, sizeof(WriteBuf));
    
    W25QXX_Read((u8*)ReadBuf, SIZE - 100, sizeof(WriteBuf));//sizeof(WriteBuf)
//    W25QXX_Write((u8*)WriteBuf, SIZE - 100, sizeof(WriteBuf));		     //  从倒数第100个地址处开始,写入SIZE长度的数据    
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
	// WriteBuf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};			//用于表述写入缓存数组
	for(int j=0;j<10;j++){
		ReadBuf[j]  = 0;										//清空用于表述读出缓存数组
	}
}
/*************************************************
Function: W25QXX_TEST()
Description: //保存管理员密码
Input: null
Output: null
Return: null
Author: UU酱
*************************************************/
void save_password(void)
{
	char nums[4];
	//将新密码写入flash里面
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
Description: //读取管理员密码
Input: null
Output: null
Return: null
Author: UU酱
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


















