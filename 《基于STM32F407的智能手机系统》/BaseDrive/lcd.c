#include "lcd.h"
#include "GPS.h"
#include "stdlib.h"
#include "font.h"
#include "USART3.h"
#include "delay.h"
#include "Hzk16song.h"
#include "photo.h"
#include "timer.h"
// #include "111.h"
#include "back.h"
#include "shang.h"
#include "xia.h"
#include "motor.h"
#include "laba.h"
#include "phone.h"
#include "a.h"
#include "gps1.h"
#include "light.h"
#include "touch.h"
#include "led.h"
#include "LED.h"
#include "KEY.h"
// #include "2015.h"
#include "lcd.h"
#include "mp3.h"
#include "listen.h"
#include "ADC.h"
#include "StepMotor.h"
#include "DCMotor.h"
#include "PWM.h"
#include "CH455I2C.H"
#include "I2C.h"
#include <math.h>
#include "girl1.h"
#include "girl2.h"
// #include "girl3.h"
// #include "girl4.h"
//LCD的画笔颜色和背景色
u16 POINT_COLOR = 0x0000;	                                            // 画笔颜色
u16 BACK_COLOR  = 0xFFFF;                                               // 背景色
int password_smart[4]={0};//输入的密码
int Password_smart[4]={8,8,8,8};//预设置的密码
int voice = 15;//音量
u8 photo_flag = 0;
u8 exit_flag = 0;
u8 light_flag = 0;
u8 gps_flag=0;
u8 mp3_flag=0;
u8 temwet_flag=0;
u8 motor_flag=0;
u8 phone_flag=0;
u8 time_flag=0;
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;
const u8 BCD_decode_tab[0x10] = { 0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F, 
									 0X77, 0X7C, 0X58, 0X5E, 0X79, 0X71 };//BCD
u8 ch455_key;//CH455的按键键值
//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(vu16 regval)
{
    regval = regval;		                                            // 使用-O2优化的时候,必须插入的延时
    LCD->LCD_REG = regval;                                              // 写入要写的寄存器序号
}

//写LCD数据
//data:要写入的值
void LCD_WR_DATA(vu16 data)
{
    data = data;			                                            // 使用-O2优化的时候,必须插入的延时
    LCD->LCD_RAM = data;
}

//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{
    vu16 ram;			                                                // 防止被优化
    ram = LCD->LCD_RAM;
    
    return ram;
}

//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;		                                        // 写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue;                                        // 写入数据
}

//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);		                                        // 写入要读的寄存器序号
    delay_us(5);
    
    return LCD_RD_DATA();		                                        // 返回读到的值
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;                                            // 写十六位GRAM
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
    u16  r, g, b, rgb;

    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);

    return(rgb);
}

//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
    while(i--);
}

//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x, u16 y)
{
    u16 r = 0, g = 0, b = 0;
    u16 ret;

    if(x >= lcddev.width || y >= lcddev.height)
    {
        return 0;	                                                    // 超过了范围,直接返回
    }

    LCD_SetCursor(x, y);

    if(lcddev.id == 0X5510)
    {
        LCD_WR_REG(0X2E00);	                                            // 5510 发送读GRAM指令
    }

    r = LCD_RD_DATA();								                    // dummy Read

    opt_delay(2);
    r = LCD_RD_DATA();  		  						                // 实际坐标颜色
    if(lcddev.id == 0X5510)		                                        // NT35510要分2次读出
    {
        opt_delay(2);
        b = LCD_RD_DATA();
        g = r & 0XFF;		                                            // 对于5510,第一次读取的是RG的值,R在前,G在后,各占8位
        g <<= 8;
    }
    
    if(lcddev.id == 0X5510)
    {
        ret =  ((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11);        // NT35510需要公式转换一下
    }

    return ret;
}

//LCD开启显示
void LCD_DisplayOn(void)
{
    if(lcddev.id == 0X5510)
    {
        LCD_WR_REG(0X2900);	                                            // 开启显示
    }
}

//LCD关闭显示
void LCD_DisplayOff(void)
{
    if(lcddev.id == 0X5510)
    {
        LCD_WR_REG(0X2800);	                                            // 关闭显示
    }
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    if(lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos & 0XFF);
    }
}

//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;

    if(lcddev.id == 0X5510)                                             // 5510,特殊处理
    {
        switch(dir)
        {
        case    L2R_U2D://从左到右,从上到下
                regval |= (0 << 7) | (0 << 6) | (0 << 5);
                break;
        
        case    L2R_D2U://从左到右,从下到上
                regval |= (1 << 7) | (0 << 6) | (0 << 5);
                break;
        
        case    R2L_U2D://从右到左,从上到下
                regval |= (0 << 7) | (1 << 6) | (0 << 5);
                break;
        
        case    R2L_D2U://从右到左,从下到上
                regval |= (1 << 7) | (1 << 6) | (0 << 5);
                break;
        
        case    U2D_L2R://从上到下,从左到右
                regval |= (0 << 7) | (0 << 6) | (1 << 5);
                break;
        
        case    U2D_R2L://从上到下,从右到左
                regval |= (0 << 7) | (1 << 6) | (1 << 5);
                break;
        
        case    D2U_L2R://从下到上,从左到右
                regval |= (1 << 7) | (0 << 6) | (1 << 5);
                break;
        
        case    D2U_R2L://从下到上,从右到左
                regval |= (1 << 7) | (1 << 6) | (1 << 5);
                break;
        }

        if(lcddev.id == 0X5510)
        {
            dirreg = 0X3600;
        }
        if(lcddev.id != 0X5510)
        {
            regval |= 0X08;                                             // 5510不需要BGR
        }

        LCD_WriteReg(dirreg, regval);

        if(lcddev.id == 0X5510)
        {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setxcmd + 1);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setxcmd + 2);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_REG(lcddev.setxcmd + 3);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
            LCD_WR_REG(lcddev.setycmd);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setycmd + 1);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setycmd + 2);
            LCD_WR_DATA((lcddev.height - 1) >> 8);
            LCD_WR_REG(lcddev.setycmd + 3);
            LCD_WR_DATA((lcddev.height - 1) & 0XFF);
        }
    }
}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);		                                        // 设置光标位置
    LCD_WriteRAM_Prepare();	                                            // 开始写入GRAM
    LCD->LCD_RAM = POINT_COLOR;
	  
}
void LCD_DrawPointcn(u16 x, u16 y, u16 color)
{
    LCD_SetCursor(x, y);												//	设置光标位置
    LCD_WriteRAM_Prepare();												//	开始写入GRAM
    LCD->LCD_RAM = color;
}

//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
    if(lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(y & 0XFF);
    }

    LCD->LCD_REG = lcddev.wramcmd;
    LCD->LCD_RAM = color;
}


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
    if(dir == 0)			                                            // 竖屏
    {
        lcddev.dir = 0;	                                                // 竖屏
        lcddev.width = 240;
        lcddev.height = 320;

        if(lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 480;
            lcddev.height = 800;
        }
    }
    else 				                                                // 横屏
    {
        lcddev.dir = 1;	                                                // 横屏
        lcddev.width = 320;
        lcddev.height = 240;

        if(lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
    }
    LCD_Scan_Dir(DFT_SCAN_DIR);	                                        // 默认扫描方向
}

//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
    u16 twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    if(lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA(theight & 0XFF);
    }
}
#define assert_param(expr) ((void)0)
#define RCC_AHB1Periph_GPIOA             ((uint32_t)0x00000001)

//初始化lcd
//该初始化函数可以初始化各种型号的LCD(详见本.c文件最前面的描述)
void LCD_Init(void)
{
    RCC->AHB1ENR |= 0XF << 3;    	                                    //使能PD,PE,PF,PG时钟
    RCC->AHB1ENR |= 1 << 1;     	//使能PB时钟
    RCC->AHB1ENR |= 1 << 0;     	//使能PA时钟
    RCC->AHB3ENR |= 1 << 0;     	//使能FSMC时钟
//    GPIO_Set(GPIOA,(0<<9),GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);							//PA9 推挽输出,控制背光
    GPIO_Set(GPIOD, (3 << 0) | (3 << 4) | (7 << 8) | (3 << 14), GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);	//PD0,1,4,5,8,9,10,14,15 AF OUT
    GPIO_Set(GPIOE, (0X1FF << 7), GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);						//PE7~15,AF OUT
    GPIO_Set(GPIOG, PIN1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);							//PG1,FSMC_A11
    GPIO_Set(GPIOG, PIN12, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);							//PG12,FSMC_NE4
//    GPIO_AF_Set(GPIOA, 9, 12);	//PA9,AF12
    GPIO_Set(GPIOA,PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PA9
	LCD_BL=1;//打开背光
    GPIO_AF_Set(GPIOD, 0, 12);	//PD0,AF12
    GPIO_AF_Set(GPIOD, 1, 12);	//PD1,AF12
    GPIO_AF_Set(GPIOD, 4, 12);	//PD4,AF12
    GPIO_AF_Set(GPIOD, 5, 12);	//PD5,AF12
    GPIO_AF_Set(GPIOD, 8, 12);	//PD8,AF12
    GPIO_AF_Set(GPIOD, 9, 12);	//PD9,AF12
    GPIO_AF_Set(GPIOD, 10, 12);	//PD10,AF12
    GPIO_AF_Set(GPIOD, 14, 12);	//PD14,AF12
    GPIO_AF_Set(GPIOD, 15, 12);	//PD15,AF12

    GPIO_AF_Set(GPIOE, 7, 12);	//PE7,AF12
    GPIO_AF_Set(GPIOE, 8, 12);	//PE8,AF12
    GPIO_AF_Set(GPIOE, 9, 12);	//PE9,AF12
    GPIO_AF_Set(GPIOE, 10, 12);	//PE10,AF12
    GPIO_AF_Set(GPIOE, 11, 12);	//PE11,AF12
    GPIO_AF_Set(GPIOE, 12, 12);	//PE12,AF12
    GPIO_AF_Set(GPIOE, 13, 12);	//PE13,AF12
    GPIO_AF_Set(GPIOE, 14, 12);	//PE14,AF12
    GPIO_AF_Set(GPIOE, 15, 12);	//PE15,AF12

    GPIO_AF_Set(GPIOG, 1, 12);	//PG1,AF12
    GPIO_AF_Set(GPIOG, 12, 12);	//PG12,AF12

    //寄存器清零
    //bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。
    //这里我们使用NE1 ，也就对应BTCR[0],[1]。
    FSMC_Bank1->BTCR[6] = 0X00000000;
    FSMC_Bank1->BTCR[7] = 0X00000000;
    FSMC_Bank1E->BWTR[6] = 0X00000000;
    //操作BCR寄存器	使用异步模式
    FSMC_Bank1->BTCR[6] |= 1 << 12;		//存储器写使能
    FSMC_Bank1->BTCR[6] |= 1 << 14;		//读写使用不同的时序
    FSMC_Bank1->BTCR[6] |= 1 << 4; 		//存储器数据宽度为16bit
    //操作BTR寄存器
    //读时序控制寄存器
    FSMC_Bank1->BTCR[7] |= 0 << 28;		//模式A
    FSMC_Bank1->BTCR[7] |= 0XF << 0; 	//地址建立时间(ADDSET)为15个HCLK 1/168M=6ns*15=90ns
    //因为液晶驱动IC的读数据的时候，速度不能太快,尤其是个别奇葩芯片。
    FSMC_Bank1->BTCR[7] |= 60 << 8;  	//数据保存时间(DATAST)为60个HCLK	=6*60=360ns
    //写时序控制寄存器
    FSMC_Bank1E->BWTR[6] |= 0 << 28; 	//模式A
    FSMC_Bank1E->BWTR[6] |= 9 << 0;		//地址建立时间(ADDSET)为9个HCLK=54ns
    //9个HCLK（HCLK=168M）,某些液晶驱动IC的写信号脉宽，最少也得50ns。
    FSMC_Bank1E->BWTR[6] |= 8 << 8; 	//数据保存时间(DATAST)为6ns*9个HCLK=54ns
    //使能BANK1,区域4
    FSMC_Bank1->BTCR[6] |= 1 << 0;		//使能BANK1，区域1

    delay_ms(50); // delay 50 ms

    LCD_WR_REG(0XDA00);
    lcddev.id = LCD_RD_DATA();		//读回0X00
    LCD_WR_REG(0XDB00);
    lcddev.id = LCD_RD_DATA();		//读回0X80
    lcddev.id <<= 8;
    LCD_WR_REG(0XDC00);
    lcddev.id |= LCD_RD_DATA();		//读回0X00
    if(lcddev.id == 0x8000)
    {
        lcddev.id = 0x5510; //NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
    }

    if(lcddev.id == 0X5510) //如果是这几个IC,则设置WR时序为最快
    {
        //重新配置写时序控制寄存器的时序
        FSMC_Bank1E->BWTR[6] &= ~(0XF << 0); //地址建立时间(ADDSET)清零
        FSMC_Bank1E->BWTR[6] &= ~(0XF << 8); //数据保存时间清零
        FSMC_Bank1E->BWTR[6] |= 3 << 0;		//地址建立时间(ADDSET)为3个HCLK =18ns
        FSMC_Bank1E->BWTR[6] |= 2 << 8; 	//数据保存时间(DATAST)为6ns*3个HCLK=18ns
    }

    if(lcddev.id == 0x5510)
    {
        LCD_WriteReg(0xF000, 0x55);
        LCD_WriteReg(0xF001, 0xAA);
        LCD_WriteReg(0xF002, 0x52);
        LCD_WriteReg(0xF003, 0x08);
        LCD_WriteReg(0xF004, 0x01);
        //AVDD Set AVDD 5.2V
        LCD_WriteReg(0xB000, 0x0D);
        LCD_WriteReg(0xB001, 0x0D);
        LCD_WriteReg(0xB002, 0x0D);
        //AVDD ratio
        LCD_WriteReg(0xB600, 0x34);
        LCD_WriteReg(0xB601, 0x34);
        LCD_WriteReg(0xB602, 0x34);
        //AVEE -5.2V
        LCD_WriteReg(0xB100, 0x0D);
        LCD_WriteReg(0xB101, 0x0D);
        LCD_WriteReg(0xB102, 0x0D);
        //AVEE ratio
        LCD_WriteReg(0xB700, 0x34);
        LCD_WriteReg(0xB701, 0x34);
        LCD_WriteReg(0xB702, 0x34);
        //VCL -2.5V
        LCD_WriteReg(0xB200, 0x00);
        LCD_WriteReg(0xB201, 0x00);
        LCD_WriteReg(0xB202, 0x00);
        //VCL ratio
        LCD_WriteReg(0xB800, 0x24);
        LCD_WriteReg(0xB801, 0x24);
        LCD_WriteReg(0xB802, 0x24);
        //VGH 15V (Free pump)
        LCD_WriteReg(0xBF00, 0x01);
        LCD_WriteReg(0xB300, 0x0F);
        LCD_WriteReg(0xB301, 0x0F);
        LCD_WriteReg(0xB302, 0x0F);
        //VGH ratio
        LCD_WriteReg(0xB900, 0x34);
        LCD_WriteReg(0xB901, 0x34);
        LCD_WriteReg(0xB902, 0x34);
        //VGL_REG -10V
        LCD_WriteReg(0xB500, 0x08);
        LCD_WriteReg(0xB501, 0x08);
        LCD_WriteReg(0xB502, 0x08);
        LCD_WriteReg(0xC200, 0x03);
        //VGLX ratio
        LCD_WriteReg(0xBA00, 0x24);
        LCD_WriteReg(0xBA01, 0x24);
        LCD_WriteReg(0xBA02, 0x24);
        //VGMP/VGSP 4.5V/0V
        LCD_WriteReg(0xBC00, 0x00);
        LCD_WriteReg(0xBC01, 0x78);
        LCD_WriteReg(0xBC02, 0x00);
        //VGMN/VGSN -4.5V/0V
        LCD_WriteReg(0xBD00, 0x00);
        LCD_WriteReg(0xBD01, 0x78);
        LCD_WriteReg(0xBD02, 0x00);
        //VCOM
        LCD_WriteReg(0xBE00, 0x00);
        LCD_WriteReg(0xBE01, 0x64);
        //Gamma Setting
        LCD_WriteReg(0xD100, 0x00);
        LCD_WriteReg(0xD101, 0x33);
        LCD_WriteReg(0xD102, 0x00);
        LCD_WriteReg(0xD103, 0x34);
        LCD_WriteReg(0xD104, 0x00);
        LCD_WriteReg(0xD105, 0x3A);
        LCD_WriteReg(0xD106, 0x00);
        LCD_WriteReg(0xD107, 0x4A);
        LCD_WriteReg(0xD108, 0x00);
        LCD_WriteReg(0xD109, 0x5C);
        LCD_WriteReg(0xD10A, 0x00);
        LCD_WriteReg(0xD10B, 0x81);
        LCD_WriteReg(0xD10C, 0x00);
        LCD_WriteReg(0xD10D, 0xA6);
        LCD_WriteReg(0xD10E, 0x00);
        LCD_WriteReg(0xD10F, 0xE5);
        LCD_WriteReg(0xD110, 0x01);
        LCD_WriteReg(0xD111, 0x13);
        LCD_WriteReg(0xD112, 0x01);
        LCD_WriteReg(0xD113, 0x54);
        LCD_WriteReg(0xD114, 0x01);
        LCD_WriteReg(0xD115, 0x82);
        LCD_WriteReg(0xD116, 0x01);
        LCD_WriteReg(0xD117, 0xCA);
        LCD_WriteReg(0xD118, 0x02);
        LCD_WriteReg(0xD119, 0x00);
        LCD_WriteReg(0xD11A, 0x02);
        LCD_WriteReg(0xD11B, 0x01);
        LCD_WriteReg(0xD11C, 0x02);
        LCD_WriteReg(0xD11D, 0x34);
        LCD_WriteReg(0xD11E, 0x02);
        LCD_WriteReg(0xD11F, 0x67);
        LCD_WriteReg(0xD120, 0x02);
        LCD_WriteReg(0xD121, 0x84);
        LCD_WriteReg(0xD122, 0x02);
        LCD_WriteReg(0xD123, 0xA4);
        LCD_WriteReg(0xD124, 0x02);
        LCD_WriteReg(0xD125, 0xB7);
        LCD_WriteReg(0xD126, 0x02);
        LCD_WriteReg(0xD127, 0xCF);
        LCD_WriteReg(0xD128, 0x02);
        LCD_WriteReg(0xD129, 0xDE);
        LCD_WriteReg(0xD12A, 0x02);
        LCD_WriteReg(0xD12B, 0xF2);
        LCD_WriteReg(0xD12C, 0x02);
        LCD_WriteReg(0xD12D, 0xFE);
        LCD_WriteReg(0xD12E, 0x03);
        LCD_WriteReg(0xD12F, 0x10);
        LCD_WriteReg(0xD130, 0x03);
        LCD_WriteReg(0xD131, 0x33);
        LCD_WriteReg(0xD132, 0x03);
        LCD_WriteReg(0xD133, 0x6D);
        LCD_WriteReg(0xD200, 0x00);
        LCD_WriteReg(0xD201, 0x33);
        LCD_WriteReg(0xD202, 0x00);
        LCD_WriteReg(0xD203, 0x34);
        LCD_WriteReg(0xD204, 0x00);
        LCD_WriteReg(0xD205, 0x3A);
        LCD_WriteReg(0xD206, 0x00);
        LCD_WriteReg(0xD207, 0x4A);
        LCD_WriteReg(0xD208, 0x00);
        LCD_WriteReg(0xD209, 0x5C);
        LCD_WriteReg(0xD20A, 0x00);

        LCD_WriteReg(0xD20B, 0x81);
        LCD_WriteReg(0xD20C, 0x00);
        LCD_WriteReg(0xD20D, 0xA6);
        LCD_WriteReg(0xD20E, 0x00);
        LCD_WriteReg(0xD20F, 0xE5);
        LCD_WriteReg(0xD210, 0x01);
        LCD_WriteReg(0xD211, 0x13);
        LCD_WriteReg(0xD212, 0x01);
        LCD_WriteReg(0xD213, 0x54);
        LCD_WriteReg(0xD214, 0x01);
        LCD_WriteReg(0xD215, 0x82);
        LCD_WriteReg(0xD216, 0x01);
        LCD_WriteReg(0xD217, 0xCA);
        LCD_WriteReg(0xD218, 0x02);
        LCD_WriteReg(0xD219, 0x00);
        LCD_WriteReg(0xD21A, 0x02);
        LCD_WriteReg(0xD21B, 0x01);
        LCD_WriteReg(0xD21C, 0x02);
        LCD_WriteReg(0xD21D, 0x34);
        LCD_WriteReg(0xD21E, 0x02);
        LCD_WriteReg(0xD21F, 0x67);
        LCD_WriteReg(0xD220, 0x02);
        LCD_WriteReg(0xD221, 0x84);
        LCD_WriteReg(0xD222, 0x02);
        LCD_WriteReg(0xD223, 0xA4);
        LCD_WriteReg(0xD224, 0x02);
        LCD_WriteReg(0xD225, 0xB7);
        LCD_WriteReg(0xD226, 0x02);
        LCD_WriteReg(0xD227, 0xCF);
        LCD_WriteReg(0xD228, 0x02);
        LCD_WriteReg(0xD229, 0xDE);
        LCD_WriteReg(0xD22A, 0x02);
        LCD_WriteReg(0xD22B, 0xF2);
        LCD_WriteReg(0xD22C, 0x02);
        LCD_WriteReg(0xD22D, 0xFE);
        LCD_WriteReg(0xD22E, 0x03);
        LCD_WriteReg(0xD22F, 0x10);
        LCD_WriteReg(0xD230, 0x03);
        LCD_WriteReg(0xD231, 0x33);
        LCD_WriteReg(0xD232, 0x03);
        LCD_WriteReg(0xD233, 0x6D);
        LCD_WriteReg(0xD300, 0x00);
        LCD_WriteReg(0xD301, 0x33);
        LCD_WriteReg(0xD302, 0x00);
        LCD_WriteReg(0xD303, 0x34);
        LCD_WriteReg(0xD304, 0x00);
        LCD_WriteReg(0xD305, 0x3A);
        LCD_WriteReg(0xD306, 0x00);
        LCD_WriteReg(0xD307, 0x4A);
        LCD_WriteReg(0xD308, 0x00);
        LCD_WriteReg(0xD309, 0x5C);
        LCD_WriteReg(0xD30A, 0x00);

        LCD_WriteReg(0xD30B, 0x81);
        LCD_WriteReg(0xD30C, 0x00);
        LCD_WriteReg(0xD30D, 0xA6);
        LCD_WriteReg(0xD30E, 0x00);
        LCD_WriteReg(0xD30F, 0xE5);
        LCD_WriteReg(0xD310, 0x01);
        LCD_WriteReg(0xD311, 0x13);
        LCD_WriteReg(0xD312, 0x01);
        LCD_WriteReg(0xD313, 0x54);
        LCD_WriteReg(0xD314, 0x01);
        LCD_WriteReg(0xD315, 0x82);
        LCD_WriteReg(0xD316, 0x01);
        LCD_WriteReg(0xD317, 0xCA);
        LCD_WriteReg(0xD318, 0x02);
        LCD_WriteReg(0xD319, 0x00);
        LCD_WriteReg(0xD31A, 0x02);
        LCD_WriteReg(0xD31B, 0x01);
        LCD_WriteReg(0xD31C, 0x02);
        LCD_WriteReg(0xD31D, 0x34);
        LCD_WriteReg(0xD31E, 0x02);
        LCD_WriteReg(0xD31F, 0x67);
        LCD_WriteReg(0xD320, 0x02);
        LCD_WriteReg(0xD321, 0x84);
        LCD_WriteReg(0xD322, 0x02);
        LCD_WriteReg(0xD323, 0xA4);
        LCD_WriteReg(0xD324, 0x02);
        LCD_WriteReg(0xD325, 0xB7);
        LCD_WriteReg(0xD326, 0x02);
        LCD_WriteReg(0xD327, 0xCF);
        LCD_WriteReg(0xD328, 0x02);
        LCD_WriteReg(0xD329, 0xDE);
        LCD_WriteReg(0xD32A, 0x02);
        LCD_WriteReg(0xD32B, 0xF2);
        LCD_WriteReg(0xD32C, 0x02);
        LCD_WriteReg(0xD32D, 0xFE);
        LCD_WriteReg(0xD32E, 0x03);
        LCD_WriteReg(0xD32F, 0x10);
        LCD_WriteReg(0xD330, 0x03);
        LCD_WriteReg(0xD331, 0x33);
        LCD_WriteReg(0xD332, 0x03);
        LCD_WriteReg(0xD333, 0x6D);
        LCD_WriteReg(0xD400, 0x00);
        LCD_WriteReg(0xD401, 0x33);
        LCD_WriteReg(0xD402, 0x00);
        LCD_WriteReg(0xD403, 0x34);
        LCD_WriteReg(0xD404, 0x00);
        LCD_WriteReg(0xD405, 0x3A);
        LCD_WriteReg(0xD406, 0x00);
        LCD_WriteReg(0xD407, 0x4A);
        LCD_WriteReg(0xD408, 0x00);
        LCD_WriteReg(0xD409, 0x5C);
        LCD_WriteReg(0xD40A, 0x00);
        LCD_WriteReg(0xD40B, 0x81);

        LCD_WriteReg(0xD40C, 0x00);
        LCD_WriteReg(0xD40D, 0xA6);
        LCD_WriteReg(0xD40E, 0x00);
        LCD_WriteReg(0xD40F, 0xE5);
        LCD_WriteReg(0xD410, 0x01);
        LCD_WriteReg(0xD411, 0x13);
        LCD_WriteReg(0xD412, 0x01);
        LCD_WriteReg(0xD413, 0x54);
        LCD_WriteReg(0xD414, 0x01);
        LCD_WriteReg(0xD415, 0x82);
        LCD_WriteReg(0xD416, 0x01);
        LCD_WriteReg(0xD417, 0xCA);
        LCD_WriteReg(0xD418, 0x02);
        LCD_WriteReg(0xD419, 0x00);
        LCD_WriteReg(0xD41A, 0x02);
        LCD_WriteReg(0xD41B, 0x01);
        LCD_WriteReg(0xD41C, 0x02);
        LCD_WriteReg(0xD41D, 0x34);
        LCD_WriteReg(0xD41E, 0x02);
        LCD_WriteReg(0xD41F, 0x67);
        LCD_WriteReg(0xD420, 0x02);
        LCD_WriteReg(0xD421, 0x84);
        LCD_WriteReg(0xD422, 0x02);
        LCD_WriteReg(0xD423, 0xA4);
        LCD_WriteReg(0xD424, 0x02);
        LCD_WriteReg(0xD425, 0xB7);
        LCD_WriteReg(0xD426, 0x02);
        LCD_WriteReg(0xD427, 0xCF);
        LCD_WriteReg(0xD428, 0x02);
        LCD_WriteReg(0xD429, 0xDE);
        LCD_WriteReg(0xD42A, 0x02);
        LCD_WriteReg(0xD42B, 0xF2);
        LCD_WriteReg(0xD42C, 0x02);
        LCD_WriteReg(0xD42D, 0xFE);
        LCD_WriteReg(0xD42E, 0x03);
        LCD_WriteReg(0xD42F, 0x10);
        LCD_WriteReg(0xD430, 0x03);
        LCD_WriteReg(0xD431, 0x33);
        LCD_WriteReg(0xD432, 0x03);
        LCD_WriteReg(0xD433, 0x6D);
        LCD_WriteReg(0xD500, 0x00);
        LCD_WriteReg(0xD501, 0x33);
        LCD_WriteReg(0xD502, 0x00);
        LCD_WriteReg(0xD503, 0x34);
        LCD_WriteReg(0xD504, 0x00);
        LCD_WriteReg(0xD505, 0x3A);
        LCD_WriteReg(0xD506, 0x00);
        LCD_WriteReg(0xD507, 0x4A);
        LCD_WriteReg(0xD508, 0x00);
        LCD_WriteReg(0xD509, 0x5C);
        LCD_WriteReg(0xD50A, 0x00);
        LCD_WriteReg(0xD50B, 0x81);

        LCD_WriteReg(0xD50C, 0x00);
        LCD_WriteReg(0xD50D, 0xA6);
        LCD_WriteReg(0xD50E, 0x00);
        LCD_WriteReg(0xD50F, 0xE5);
        LCD_WriteReg(0xD510, 0x01);
        LCD_WriteReg(0xD511, 0x13);
        LCD_WriteReg(0xD512, 0x01);
        LCD_WriteReg(0xD513, 0x54);
        LCD_WriteReg(0xD514, 0x01);
        LCD_WriteReg(0xD515, 0x82);
        LCD_WriteReg(0xD516, 0x01);
        LCD_WriteReg(0xD517, 0xCA);
        LCD_WriteReg(0xD518, 0x02);
        LCD_WriteReg(0xD519, 0x00);
        LCD_WriteReg(0xD51A, 0x02);
        LCD_WriteReg(0xD51B, 0x01);
        LCD_WriteReg(0xD51C, 0x02);
        LCD_WriteReg(0xD51D, 0x34);
        LCD_WriteReg(0xD51E, 0x02);
        LCD_WriteReg(0xD51F, 0x67);
        LCD_WriteReg(0xD520, 0x02);
        LCD_WriteReg(0xD521, 0x84);
        LCD_WriteReg(0xD522, 0x02);
        LCD_WriteReg(0xD523, 0xA4);
        LCD_WriteReg(0xD524, 0x02);
        LCD_WriteReg(0xD525, 0xB7);
        LCD_WriteReg(0xD526, 0x02);
        LCD_WriteReg(0xD527, 0xCF);
        LCD_WriteReg(0xD528, 0x02);
        LCD_WriteReg(0xD529, 0xDE);
        LCD_WriteReg(0xD52A, 0x02);
        LCD_WriteReg(0xD52B, 0xF2);
        LCD_WriteReg(0xD52C, 0x02);
        LCD_WriteReg(0xD52D, 0xFE);
        LCD_WriteReg(0xD52E, 0x03);
        LCD_WriteReg(0xD52F, 0x10);
        LCD_WriteReg(0xD530, 0x03);
        LCD_WriteReg(0xD531, 0x33);
        LCD_WriteReg(0xD532, 0x03);
        LCD_WriteReg(0xD533, 0x6D);
        LCD_WriteReg(0xD600, 0x00);
        LCD_WriteReg(0xD601, 0x33);
        LCD_WriteReg(0xD602, 0x00);
        LCD_WriteReg(0xD603, 0x34);
        LCD_WriteReg(0xD604, 0x00);
        LCD_WriteReg(0xD605, 0x3A);
        LCD_WriteReg(0xD606, 0x00);
        LCD_WriteReg(0xD607, 0x4A);
        LCD_WriteReg(0xD608, 0x00);
        LCD_WriteReg(0xD609, 0x5C);
        LCD_WriteReg(0xD60A, 0x00);
        LCD_WriteReg(0xD60B, 0x81);

        LCD_WriteReg(0xD60C, 0x00);
        LCD_WriteReg(0xD60D, 0xA6);
        LCD_WriteReg(0xD60E, 0x00);
        LCD_WriteReg(0xD60F, 0xE5);
        LCD_WriteReg(0xD610, 0x01);
        LCD_WriteReg(0xD611, 0x13);
        LCD_WriteReg(0xD612, 0x01);
        LCD_WriteReg(0xD613, 0x54);
        LCD_WriteReg(0xD614, 0x01);
        LCD_WriteReg(0xD615, 0x82);
        LCD_WriteReg(0xD616, 0x01);
        LCD_WriteReg(0xD617, 0xCA);
        LCD_WriteReg(0xD618, 0x02);
        LCD_WriteReg(0xD619, 0x00);
        LCD_WriteReg(0xD61A, 0x02);
        LCD_WriteReg(0xD61B, 0x01);
        LCD_WriteReg(0xD61C, 0x02);
        LCD_WriteReg(0xD61D, 0x34);
        LCD_WriteReg(0xD61E, 0x02);
        LCD_WriteReg(0xD61F, 0x67);
        LCD_WriteReg(0xD620, 0x02);
        LCD_WriteReg(0xD621, 0x84);
        LCD_WriteReg(0xD622, 0x02);
        LCD_WriteReg(0xD623, 0xA4);
        LCD_WriteReg(0xD624, 0x02);
        LCD_WriteReg(0xD625, 0xB7);
        LCD_WriteReg(0xD626, 0x02);
        LCD_WriteReg(0xD627, 0xCF);
        LCD_WriteReg(0xD628, 0x02);
        LCD_WriteReg(0xD629, 0xDE);
        LCD_WriteReg(0xD62A, 0x02);
        LCD_WriteReg(0xD62B, 0xF2);
        LCD_WriteReg(0xD62C, 0x02);
        LCD_WriteReg(0xD62D, 0xFE);
        LCD_WriteReg(0xD62E, 0x03);
        LCD_WriteReg(0xD62F, 0x10);
        LCD_WriteReg(0xD630, 0x03);
        LCD_WriteReg(0xD631, 0x33);
        LCD_WriteReg(0xD632, 0x03);
        LCD_WriteReg(0xD633, 0x6D);
        //LV2 Page 0 enable
        LCD_WriteReg(0xF000, 0x55);
        LCD_WriteReg(0xF001, 0xAA);
        LCD_WriteReg(0xF002, 0x52);
        LCD_WriteReg(0xF003, 0x08);
        LCD_WriteReg(0xF004, 0x00);
        //Display control
        LCD_WriteReg(0xB100, 0xCC);
        LCD_WriteReg(0xB101, 0x00);
        //Source hold time
        LCD_WriteReg(0xB600, 0x05);
        //Gate EQ control
        LCD_WriteReg(0xB700, 0x70);
        LCD_WriteReg(0xB701, 0x70);
        //Source EQ control (Mode 2)
        LCD_WriteReg(0xB800, 0x01);
        LCD_WriteReg(0xB801, 0x03);
        LCD_WriteReg(0xB802, 0x03);
        LCD_WriteReg(0xB803, 0x03);
        //Inversion mode (2-dot)
        LCD_WriteReg(0xBC00, 0x02);
        LCD_WriteReg(0xBC01, 0x00);
        LCD_WriteReg(0xBC02, 0x00);
        //Timing control 4H w/ 4-delay
        LCD_WriteReg(0xC900, 0xD0);
        LCD_WriteReg(0xC901, 0x02);
        LCD_WriteReg(0xC902, 0x50);
        LCD_WriteReg(0xC903, 0x50);
        LCD_WriteReg(0xC904, 0x50);
        LCD_WriteReg(0x3500, 0x00);
        LCD_WriteReg(0x3A00, 0x55); //16-bit/pixel
        LCD_WR_REG(0x1100);
        delay_us(120);
        LCD_WR_REG(0x2900);
    }
    LCD_Display_Dir(0);		//默认为竖屏

    LCD_Clear(WHITE);
}

//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
    u32 index = 0;
    u32 totalpoint = lcddev.width;
    totalpoint    *= lcddev.height; 			                        // 得到总点数

   
    LCD_SetCursor(0x00, 0x0000);	                                    // 设置光标位置

    LCD_WriteRAM_Prepare();     		                                // 开始写入GRAM

    for(index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;

    xlen = ex - sx + 1;
    for(i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);      				                        // 设置光标位置
        LCD_WriteRAM_Prepare();     			                        // 开始写入GRAM
        
        for(j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;	                                    // 显示颜色
        }
    }
}

//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;

    width  = ex - sx + 1; 			                                    // 得到填充的宽度
    height = ey - sy + 1;			                                    // 高度

    for(i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);   	                                // 设置光标位置
        LCD_WriteRAM_Prepare();                                         // 开始写入GRAM
        
        for(j = 0; j < width; j++)
        {
            LCD->LCD_RAM = color[i * width + j];                        // 写入数据
        }
    }
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x2 - x1;                                                  // 计算坐标增量
    delta_y = y2 - y1;
    
    uRow = x1;
    uCol = y1;

    if(delta_x > 0)
    {
        incx = 1;                                                       // 设置单步方向
    }
    
    else if(delta_x == 0)
    {
        incx = 0;                                                       // 垂直线
    }
    else
    {
        incx    = -1;
        delta_x = -delta_x;
    }
    
    if(delta_y > 0)
    {
        incy = 1;
    }
    
    else if(delta_y == 0)
    {
        incy = 0;                                                       // 水平线
    }
    else
    {
        incy    = -1;
        delta_y = -delta_y;
    }
    
    if( delta_x > delta_y)
    {
        distance = delta_x;                                             // 选取基本增量坐标轴
    }
    
    else 
    {
        distance = delta_y;
    }
    
    for(t = 0; t <= distance + 1; t++ )                                 // 画线输出
    {
        LCD_DrawPoint(uRow, uCol);                                      // 画点
        
        xerr += delta_x ;
        yerr += delta_y ;
        
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**********************************************************************************************************
函数名称：画矩形
输入参数：(x1,y1),(x2,y2):矩形的对角坐标
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;

    a = 0;
    b = r;
    di = 3 - (r << 1);                                                  // 判断下个点位置的标志
    
    while(a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b);                                  // 5
        LCD_DrawPoint(x0 + b, y0 - a);                                  // 0
        LCD_DrawPoint(x0 + b, y0 + a);                                  // 4
        LCD_DrawPoint(x0 + a, y0 + b);                                  // 6
        LCD_DrawPoint(x0 - a, y0 + b);                                  // 1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b);                                  // 2
        LCD_DrawPoint(x0 - b, y0 - a);                                  // 7
        a++;
        
        // 使用Bresenham算法画圆
        if(di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;

    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		    // 得到字体一个字符对应点阵集所占的字节数
    num = num - ' ';                                                    // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for(t = 0; t < csize; t++)
    {
        if(size == 12)
        {
            temp = asc2_1206[num][t]; 	 	                            // 调用1206字体
        }
        
        else if(size == 16)
        {
            temp = asc2_1608[num][t];	                                // 调用1608字体
        }
        
        else if(size == 24)
        {
            temp = asc2_2412[num][t];	                                // 调用2412字体
        }
        
        else 
        {
            return;								                        // 没有的字库
        }
        
        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)
            {
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            }
            
            else if(mode == 0)
            {
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            }
            temp <<= 1;
            y++;
            
            if(y >= lcddev.height)
            {
                return;		                                            // 超区域了
            }
            
            if((y - y0) == size)
            {
                y = y0;
                x++;
                
                if(x >= lcddev.width)
                {
                    return;	                                            // 超区域了
                }
                break;
            }
        }
    }
}

//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;
    
    while(n--)
    {
        result *= m;
    }
    
    return result;
}

//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;

        }
        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, 0);
    }
}

//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode & 0X80)
                {
                    LCD_ShowChar(x + (size / 2)*t, y, '0', size, mode & 0X01);
                }
                else 
                {
                    LCD_ShowChar(x + (size / 2)*t, y, ' ', size, mode & 0X01);
                }
                continue;
            }
            else 
            {
                enshow = 1;
            }

        }
        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode & 0X01);
    }
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowStringMy(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;

    width += x;
    height += y;

    while((*p <= '~') && (*p >= ' '))                                   // 判断是不是非法字符!
    {
        if(x >= width)
        {
            x = x0;
            y += size;
        }
        
        if(y >= height)
        {
            break; //退出
        }
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}
/**********************************************************************************************************
函数名称：显示字符串
输入参数：x,y			起点坐标
		  u8 *pcStr		字符
		  PenColor  	字符颜色
		  BackColor 	背景颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_ShowString(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
{
    while(*pcStr)
	{
		if(*pcStr > 0xa1)                                               //  显示汉字
		{
			LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
			pcStr += 2;
			x0 += 16;
		}
		else                                                            //  显示字符
		{
			LCD_ShowCharString(x0, y0, pcStr, PenColor, BackColor);	
			pcStr +=1;
			x0+= 8;
		}
	}	
}
void LCD_ShowHzString(u16 x, u16 y, u8 *c,u16 PenColor, u16 BackColor)
{
	int i, j;
	u8 buffer[32];
	u16 tmp_char=0;
	unsigned int c1, c2;
    unsigned int c3 = 0;
    unsigned char *c4;
    
    c4 = c4;
    c1 = *c;	                                                        //  区号
    c2 = *(c + 1);	                                                    //  位号	修改正确
    c3 = ((c1 - 0xa1)*0x5e + (c2 - 0xa1))*0x20;                         //  汉字在字库中的地址
    c4 = (unsigned char*)&Hzk16[c3];                                    //  换算成指针
		  
	for(i=0;i<32;i++)
    {
        buffer[i] = Hzk16[c3+i];
    }
	for (i = 0; i < 16; i++)		 
	{
		tmp_char = buffer[i*2];
		tmp_char = (tmp_char << 8);
		tmp_char |= buffer[2*i + 1];                                    //  现在tmp_char存储着一行的点阵数据

		for (j = 0; j < 16; j++)
		{
			if((tmp_char >> 15 - j) & 0x01 == 0x01)
			{
				LCD_DrawPointcn(x + j, y + i, PenColor);                  //  字符颜色
			}
			else
			{
				LCD_DrawPointcn(x + j, y + i,BackColor);                 //  背景颜色
			}
		}
	}

}
void LCD_Showcn(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
{
while(*pcStr)
	{
		
			LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
			pcStr += 2;
			x0 += 16;
		
	}	
}

void LCDDrawLine_fill(u16 x1,u16 y1,u16 x2,u16 y2,u16 *color)
{
	u16 dx,dy,e;
	dx=x2-x1;
	dy=y2-y1;
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
     { 
			 e=dy-dx/2;
       while(x1<=x2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
         LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{y1+=1;e-=dx;}
           x1+=1;
           e+=dy;
      }
		}
		else
		{
		   e=dx-dy/2;
       while(y1<=y2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
         LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{x1+=1;e-=dy;}
           y1+=1;
           e+=dx;
      }
     }
		}
	else   //dy<0
	{
	    dy=-dy;
	    if(dx>=dy) // 1/8 octant
     {  
			 e=dy-dx/2;
       while(x1<=x2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
          LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{y1-=1;e-=dx;}
           x1+=1;
           e+=dy;
      }
		}
		else
		{
		 e=dx-dy/2;
       while(y1>=y2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
          LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{x1+=1;e-=dy;}
           y1-=1;
           e+=dx;
      }
     }
	 }
 }
	else         //dx<0
		{
			dx=-dx;
	if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
     { 
			 e=dy-dx/2;
       while(x1>=x2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
          LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{y1+=1;e-=dx;}
           x1-=1;
           e+=dy;
      }
		}
		else
		{
		 e=dx-dy/2;
       while(y1<=y2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
          LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{x1-=1;e-=dy;}
           y1+=1;
           e+=dx;
      }
     }
		}
	else   //dy<0
	{
	    dy=-dy;
	    if(dx>=dy) // 1/8 octant
     {  
			 e=dy-dx/2;
       while(x1>=x2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
         LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{y1-=1;e-=dx;}
           x1-=1;
           e+=dy;
      }
		}
		else
		{
		 e=dx-dy/2;
       while(y1>=y2)
     {
          if((x1 > 480)|| (y1 > 800))
                   return;
          LCD_Color_Fill(x1,y1,x1+1,y1+1,color);
					if(e>0)
					{x1-=1;e-=dy;}
           y1-=1;
           e+=dx;
      }
     }
	 }
 }
}

void LCD_DrawCircle_fill(int xc,int yc,int r,u16 *color)
{
int x,y,d;
y=r;
d=3-(r+r);
x=0;
while(x <= y)
{
LCDDrawLine_fill(xc+x,yc+y,xc-x,yc+y,color);
LCDDrawLine_fill(xc+x,yc-y,xc-x,yc-y,color);
LCDDrawLine_fill(xc+y,yc+x,xc-y,yc+x,color);
LCDDrawLine_fill(xc+y,yc-x,xc-y,yc-x,color);

if(d< 0)
   d += 4*x+6;
else
{
d+=4*(x -y)+ 10;
y--;
}
x++;
}
}
/**********************************************************************************************************
函数名称：LCD_DrawPoint
输入参数：x.y:画点的坐标
		  color：颜色
输出参数：无
函数返回：无
**********************************************************************************************************/
void LCD_DrawOnrPoint(uint16_t xsta, uint16_t ysta, uint16_t color)
{
	LCD_SetCursor(xsta, ysta);  		                                //  设置光标位置
	LCD_WriteRAM_Prepare();          	                                //  开始写入GRAM
	LCD_WriteRAM(color); 

}
void LCD_DrawPicture(u16 StartX, u16 StartY, u16 Xend, u16 Yend, u8 *pic)
{
    static	u16 i = 0, j = 0;
    
    u16 *bitmap = (u16 *)pic;
    
    for(j = 0; j < Yend - StartY; j++)
    {
        for(i = 0; i < Xend - StartX; i++)
        {
            LCD_DrawOnrPoint(StartX + i, StartY + j, *bitmap++);
        }
    }
}
void Exit(void)
{
    u8 t = 0;
    u8 i = 0;
    u16 lastpos[5][2];		//最后一次的数据
    int f=0;
    exit_flag = 0;
    while(1)
    {
        tp_dev.scan(0);

        for(t = 0; t < 5; t++)
        {
            if((tp_dev.sta) & (1 << t))
            {
                if(tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)
                {
                    if(lastpos[t][0] == 0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }
                }
            }
            if(tp_dev.x[t]>0&&tp_dev.x[t]<100&&tp_dev.y[t]>0&&tp_dev.y[t]<100&&f==0)
            {
                exit_flag = 1;
							  delay_ms(100);
                break;
            }
        }
        delay_ms(30);
        // i++;
        if(exit_flag == 1)
            break;
    }
}


// void ctp_test2(u8 sign)//输入密码
// {   
// 	  int step=150;
// 	  int start_x=40;
// 	  int start_y=180; 
// 	  int end_x=140;
// 	  int end_y=280; 
// 	  int step2=60;
//     u8 t = 0;
//     u8 i = 0;
// 	  u8 k=0;
// 	  int flag=0;
//     u8 confirm_flag = 0;
//     u16 lastpos[5][2];		//???????
// 	  u8 r;
// 	  POINT_COLOR = BLACK;
//     // while(flag==0)
//     while(1)
//     {
//         tp_dev.scan(0);
//         for(t = 0; t < 5; t++)
//         {
//             if((tp_dev.sta) & (1 << t))
//             {
//                 if(tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)
//                 {
//                     if(lastpos[t][0] == 0XFFFF)
//                     {
//                         lastpos[t][0] = tp_dev.x[t];
//                         lastpos[t][1] = tp_dev.y[t];
//                     }
//                 }
//             }
// 				// 		for(i=0;i<8;i++)
// 				// {
// 				// 		GPIOF->MODER &= ~(3<<2*i);
// 				// 		GPIOF->MODER |=1<<(2*i);
// 				// 		GPIOF->OSPEEDR &= ~(3<<2*i); 
// 				// 		GPIOF->OSPEEDR |=2<<(2*i);
// 				// 		GPIOF->PUPDR &= ~(3<<2*i);
// 				// 		GPIOF->PUPDR |= 1<<(2*i);
// 				// 		GPIOF->OTYPER &= ~(1<<i);
// 				// 		GPIOF->OTYPER |= 0<<i;
// 				// 		GPIOF->ODR |=1<<i;
// 				// }
//          if((tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>675 && tp_dev.y[t]<765)&&(confirm_flag==0)){
//                 confirm_flag=1;
//             }
//                 if(tp_dev.x[t]>start_x&&tp_dev.x[t]<end_x&&tp_dev.y[t]>start_y&&tp_dev.y[t]<end_y)  //1
//                 {
//                           if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=1;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;									
//                 } 
//                 else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y&&tp_dev.y[t]<end_y)  //2
//                 {
//                           if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=2;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;		
//                 } 
// 								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y&&tp_dev.y[t]<end_y)  //3
//                 {
//                           if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=3;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;		
//                 } 
// 								else if(tp_dev.x[t]>start_x&&tp_dev.x[t]<end_x&&tp_dev.y[t]>start_y+step&&tp_dev.y[t]<end_y+step)  //4
//                 {
// 									        if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=4;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;										
//                 } 
// 								else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y+step&&tp_dev.y[t]<end_y+step)  //5
//                 {
//                           if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=5;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;		
//                 } 
// 								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y+step&&tp_dev.y[t]<end_y+step)  //6
//                 {
//                           if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=6;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;		
//                 }
// 								else if(tp_dev.x[t]>start_x&&tp_dev.x[t]<end_x&&tp_dev.y[t]>start_y+step*2&&tp_dev.y[t]<end_y+step*2)  //7
//                 {
// 									        if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=7;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;										
//                 } 
// 								else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y*2+step&&tp_dev.y[t]<end_y+step*2)  //8
//                 {
//                          if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=8;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;									
//                 } 
// 								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y+step*2&&tp_dev.y[t]<end_y+step*2)  //9
//                 {
//                           if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=9;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;											
//                 }
// //								else if(tp_dev.x[t]>start_x&&tp_dev.x[t]<end_x&&tp_dev.y[t]>start_y+step*3&&tp_dev.y[t]<end_y+step*3)  //AC
// //                {
// //									        GPIOF->ODR &= ~(1<<0);
// //									        flag=1;
// //									        Load_Drow_Dialog();//清除
// //                          break;									
// //                } 
// 								else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y*3+step&&tp_dev.y[t]<end_y+step*3)  //0
//                 {
//                           if(confirm_flag==0) break;//确认
// 									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
// 									        password_smart[sign]=0;
// 									        for(k=0;k<sign+1;k++)
// 									            for(r=10;r>0;r=r-0.01)
// 									               LCD_Draw_Circle(150+step2*k,130,r);
// 									        flag=1;
//                           confirm_flag=0;//确认
//                           // break;		
//                 } 
// //								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y+step*3&&tp_dev.y[t]<end_y+step*3)  //x
// //                {
// //                          GPIOF->ODR &= ~(1<<2);
// //									        flag=1;
// //                          break;									
// //                }
// //								else
// //								{
// //													lastpos[t][0] = 0XFFFF;
// //								}
// 			// GPIOF->ODR |= 1<<0;
// 		  // GPIOF->ODR |= 1<<1;
// 			// GPIOF->ODR |= 1<<2;
// 			// GPIOF->ODR |= 1<<3;
// 			// GPIOF->ODR |= 1<<4;
// 			// GPIOF->ODR |= 1<<5;
// 			// GPIOF->ODR |= 1<<8;   
// 			// GPIOF->ODR |= 1<<7;	  
// 			// GPIOF->ODR |= 1<<6; 			
//         }
				
//         //
//         delay_ms(5);
//         //i++;

//     }
// }
void ctp_test2()//输入密码
{   
	  int step=150;
	  int start_x=40;
	  int start_y=180; 
	  int end_x=140;
	  int end_y=280; 
	  int step2=60;
    u8 t = 0;
    u8 i = 0;
	  u8 k=0;
    int sign=0;
	  int flag=0;
    u8 confirm_flag = 0;
    u16 lastpos[5][2];		//???????
	  u8 r;
	  POINT_COLOR = BLACK;
    // while(flag==0)
    while(1)
    {
        tp_dev.scan(0);
        for(t = 0; t < 5; t++)
        {
            if((tp_dev.sta) & (1 << t))
            {
                if(tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)
                {
                    if(lastpos[t][0] == 0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }
                }
            }
				// 		for(i=0;i<8;i++)
				// {
				// 		GPIOF->MODER &= ~(3<<2*i);
				// 		GPIOF->MODER |=1<<(2*i);
				// 		GPIOF->OSPEEDR &= ~(3<<2*i); 
				// 		GPIOF->OSPEEDR |=2<<(2*i);
				// 		GPIOF->PUPDR &= ~(3<<2*i);
				// 		GPIOF->PUPDR |= 1<<(2*i);
				// 		GPIOF->OTYPER &= ~(1<<i);
				// 		GPIOF->OTYPER |= 0<<i;
				// 		GPIOF->ODR |=1<<i;
				// }
         if((tp_dev.x[t]>start_x && tp_dev.x[t]<end_x && tp_dev.y[t]>start_y+step*3 && tp_dev.y[t]<end_y+step*3)&&(confirm_flag==0)){
                confirm_flag=1;
            }
                if(tp_dev.x[t]>start_x&&tp_dev.x[t]<end_x&&tp_dev.y[t]>start_y&&tp_dev.y[t]<end_y)  //1
                {
                          if(confirm_flag==0) continue;//确认
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=1;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;									
                } 
                else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y&&tp_dev.y[t]<end_y)  //2
                {
                          if(confirm_flag==0) continue;//确认
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=2;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;		
                } 
								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y&&tp_dev.y[t]<end_y)  //3
                {
                          if(confirm_flag==0) continue;//确认
                          
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=3;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;		
                } 
								else if(tp_dev.x[t]>start_x&&tp_dev.x[t]<end_x&&tp_dev.y[t]>start_y+step&&tp_dev.y[t]<end_y+step)  //4
                {
									        if(confirm_flag==0) continue;//确认
                          
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=4;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;										
                } 
								else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y+step&&tp_dev.y[t]<end_y+step)  //5
                {
                          if(confirm_flag==0) continue;//确认
                         
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=5;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;		
                } 
								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y+step&&tp_dev.y[t]<end_y+step)  //6
                {
                          if(confirm_flag==0) continue;//确认
                          
                          if(sign > 3)
                              sign =3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=6;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;		
                }
								else if(tp_dev.x[t]>start_x&&tp_dev.x[t]<end_x&&tp_dev.y[t]>start_y+step*2&&tp_dev.y[t]<end_y+step*2)  //7
                {
									        if(confirm_flag==0) continue;//确认
                         
                          if(sign > 3)
                              sign =3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=7;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;										
                } 
								else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y*2+step&&tp_dev.y[t]<end_y+step*2)  //8
                {
                         if(confirm_flag==0) continue;//确认
                          
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=8;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;									
                } 
								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y+step*2&&tp_dev.y[t]<end_y+step*2)  //9
                {
                          if(confirm_flag==0) continue;//确认
                          
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=9;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;
									        flag=1;
                          confirm_flag=0;//确认
                          // break;											
                }
								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y+step*3&&tp_dev.y[t]<end_y+step*3)  //删除
               {
                         Password_smart[sign] = 0;
                         sign--;
                         if(sign<0){
                          sign=0;
                         }
                         LCD_Fill(130,90,340,160,WHITE);
                         for(i=0;i<4*step2;i=i+step2)
                          LCD_Draw_Circle(90+60+i,230-100,10);
                          for(k=0;k<sign;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                         delay_ms(50);

               } 
								else if(tp_dev.x[t]>start_x+step&&tp_dev.x[t]<end_x+step&&tp_dev.y[t]>start_y*3+step&&tp_dev.y[t]<end_y+step*3)  //0
                {
                          if(confirm_flag==0) continue;//确认
                         
                          if(sign > 3)
                              sign = 3;
									        // GPIOF->ODR &= ~(1<<0);   //将GPIOF端口的第0位清除为0  
									        password_smart[sign]=0;
									        for(k=0;k<sign+1;k++)
									            for(r=10;r>0;r=r-0.01)
									               LCD_Draw_Circle(150+step2*k,130,r);
                          sign++;       
									        flag=1;
                          confirm_flag=0;//确认
                          // break;		
                } 
//								else if(tp_dev.x[t]>start_x+step*2&&tp_dev.x[t]<end_x+step*2&&tp_dev.y[t]>start_y+step*3&&tp_dev.y[t]<end_y+step*3)  //x
//                {
//                          GPIOF->ODR &= ~(1<<2);
//									        flag=1;
//                          break;									
//                }
//								else
//								{
//													lastpos[t][0] = 0XFFFF;
//								}
			// GPIOF->ODR |= 1<<0;
		  // GPIOF->ODR |= 1<<1;
			// GPIOF->ODR |= 1<<2;
			// GPIOF->ODR |= 1<<3;
			// GPIOF->ODR |= 1<<4;
			// GPIOF->ODR |= 1<<5;
			// GPIOF->ODR |= 1<<8;   
			// GPIOF->ODR |= 1<<7;	  
			// GPIOF->ODR |= 1<<6; 			
        }
				
        //
        delay_ms(5);
        //i++;
      if(sign == 4)
      break;
    }
    
}
void gui_draw_hline(u16 x0, u16 y0, u16 len, u16 color)
{
    if(len == 0)return;
    LCD_Fill(x0, y0, x0 + len - 1, y0, color);
}
void gui_fill_circle(u16 x0, u16 y0, u16 r, u16 color)
{
    u32 i;
    u32 imax = ((u32)r * 707) / 1000 + 1;
    u32 sqmax = (u32)r * (u32)r + (u32)r / 2;
    u32 x = r;
    gui_draw_hline(x0 - r, y0, 2 * r, color);
    for (i = 1; i <= imax; i++)
    {
        if ((i * i + x * x) > sqmax) // draw lines from outside
        {
            if (x > imax)
            {
                gui_draw_hline (x0 - i + 1, y0 + x, 2 * (i - 1), color);
                gui_draw_hline (x0 - i + 1, y0 - x, 2 * (i - 1), color);
            }
            x--;
        }
        // draw lines from inside (center)
        gui_draw_hline(x0 - x, y0 + i, 2 * x, color);
        gui_draw_hline(x0 - x, y0 - i, 2 * x, color);
    }
}
void close()    //锁屏界面的设计
{
	  
	  int i=0,j=0;
	  int step=150;
	  int step2=60;
	  int star_x=90;
	  int star_y=230; 
	  int k=1;
	  for(i=0;i<3*step;i=i+step)
	     for(j=0;j<4*step;j=j+step)
	{
		    //  if((i==0&&j==3*step)||(i==2*step&&j==3*step))
				// 	 continue;
         LCD_Draw_Circle(star_x+i,star_y+j,50);
	}
	  for(i=0;i<4*step2;i=i+step2)
       LCD_Draw_Circle(star_x+60+i,star_y-100,10);
	      //LCD_DrawCircle_fill(star_x+60+i,star_y-100,10,BLACK);
	  POINT_COLOR = BLACK;  
	  for(i=0;i<3*step;i=i+step)
	     for(j=0;j<3*step;j=j+step)
        { 
				 LCD_ShowNum(star_x-4+j, star_y-10+i,k,1,24);
	       k=k+1;
				}
	  // LCD_ShowStringMy(star_x-10, star_y-10+step*3, 200, 24,24, "AC");
	  LCD_ShowStringMy(star_x-4+step, star_y-10+step*3, 200, 24,24, "0");
	  //  LCD_ShowStringMy(star_x-4+step*2, star_y-10+step*3, 200, 24,24, "X");
    // LCD_ShowStringMy(star_x-10, star_y-10+step*3, 200, 24,24, " 确认");
    LCD_Showcn(star_x-10, star_y-10+step*3,"确认",BLACK, WHITE);
    LCD_Showcn(star_x-4+step*2, star_y-10+step*3,"删除",BLACK, WHITE);
    // LCD_ShowStringMy(star_x-4+step*2, star_y-10+step*3, 200, 24,24, "删除");
    
	  POINT_COLOR = BLUE; 
	  LCD_Showcn(star_x+90,star_y-160,"请输入锁屏密码！",BLACK, WHITE);
}
// int In_FR(void)
// {
//     uint8_t key = 0;
//     uint8_t local_mode = 1;  // 1:选择界面 2:密码解锁 3:指纹解锁
//     uint8_t flag = 1;        // 显示标志
    
//     // 初始化屏幕
//     LCD_Clear(WHITE);
//     POINT_COLOR = BLUE;
    
//     while(1)
//     {
//         // 显示界面
//         if(flag)
//         {
//             LCD_Clear(WHITE);  // 每次切换界面都清屏
//             switch(local_mode)
//             {
//                 case 1:  // 选择解锁方式界面
//                     LCD_ShowStringMy(100,100,300,24,24,"请选择解锁方式");
//                     LCD_ShowStringMy(50,150,400,24,24,"1.密码解锁  2.指纹解锁");
//                     break;
                    
//                 case 2:  // 密码解锁界面
//                     close();  // 显示密码输入界面
//                     break;
                    
//                 case 3:  // 指纹解锁界面
//                     LCD_ShowStringMy(100,100,300,24,24,"请进行指纹验证");
//                     LCD_ShowStringMy(50,150,400,24,24,"按1返回选择界面");
//                     Compare_FR();  // 立即开始指纹验证
//                     break;
//             }
//             flag = 0;
//         }
        
//         key = KeyScan();
//         if(key != 0)  // 只有在有按键时才处理
//         {
//             switch(local_mode)
//             {
//                 case 1:  // 选择解锁方式
//                     if(key == 1) {
//                         local_mode = 2;  // 进入密码解锁
//                         flag = 1;
//                     }
//                     else if(key == 2) {
//                         local_mode = 3;  // 进入指纹解锁
//                         flag = 1;
//                     }
//                     break;
                    
//                 case 2:  // 密码解锁处理
//                     ctp_test2();  // 获取密码输入
//                     delay_ms(200);
                    
//                     // 验证密码
//                     for(int j=0; j<4; j++)
//                     { 
//                         if(password_smart[j] != Password_smart[j])
//                         {  
//                             BEEP2();
//                             LCD_Clear(WHITE);
//                             LCD_ShowStringMy(0,300,450,24,24,"密码错误，请重试");
//                             delay_ms(1000);
//                             local_mode = 1;  // 返回选择界面
//                             flag = 1;
//                             break;
//                         }
//                         if(j == 3)  // 密码正确
//                         {
//                             BEEP1();
//                             LCD_Clear(WHITE);
//                             LCD_ShowStringMy(200,300,200,24,24,"WELCOME");
//                             delay_ms(1000);
//                             return 1;
//                         }
//                     }
//                     break;
                    
//                 case 3:  // 指纹解锁处理
//                     if(key == 1) {  // 返回选择界面
//                         local_mode = 1;
//                         flag = 1;
//                     }
//                     else if(PS_Sta) {  // 检查指纹模块状态
//                         if(PS_Sta == 0x00) {  // 指纹验证成功
//                             BEEP1();
//                             LCD_Clear(WHITE);
//                             LCD_ShowStringMy(200,300,200,24,24,"WELCOME");
//                             delay_ms(1000);
//                             return 1;
//                         }
//                         else {  // 指纹验证失败
//                             BEEP2();
//                             LCD_Clear(WHITE);
//                             LCD_ShowStringMy(100,300,300,24,24,"指纹验证失败，请重试");
//                             delay_ms(1000);
//                             local_mode = 1;  // 返回选择界面
//                             flag = 1;
//                         }
//                     }
//                     break;
//             }
//         }
        
//         delay_ms(10);  // 缩短延时，提高响应速度
//     }
// }
int In()
{
		int k=1;
	  while(k!=0)
		{ 
      int i=0,j=0;
      k=0;
      close();			
			// for(i=0;i<4;i++)
      //     {
			// 			delay_ms(1000);
			// 			ctp_test2(i); 	//进入四次，输入四位数密码			
			// 			//
			// 		}
      ctp_test2();
			delay_ms(200);
			for(j=0;j<4;j++)
			 { 
				 if(password_smart[j]!= Password_smart[j])
				 {  
					 k=1;
           break;
				 }
			 }
			 if(k==1)
			 {   
				  BEEP2();
				  LCD_Clear(WHITE);
				  LCD_ShowStringMy(0,300,450,24,24,"Incorrect password_smart, please re-enter");
				  delay_ms(1000);
				  LCD_Clear(WHITE);
          return 0;
			 }
			 else
			 {
				  BEEP1();
			    LCD_Clear(WHITE);
//			  LCD_Showcn(200,300,"欢迎进入!",BLACK, WHITE);
				  LCD_ShowStringMy(200,300,200,24,24,"WELCOME");
				  delay_ms(1000);
				  LCD_Clear(WHITE);
				return 1;
			 }
     }
}
void FR_Management(void)
{
    uint8_t key = 0;
    uint8_t local_mode = 1;  // 局部模式变量，1为主界面
    uint8_t flag = 1;        // 显示标志
    
    while(1)  // 持续运行直到返回
    {
        // 显示界面
        if(flag)
        {
            switch(local_mode)
            {
                case 1:  // 指纹管理主界面
                    LCD_Clear(WHITE);
                    LCD_ShowStringMy(100,100,300,24,24,"指纹管理系统");
                    LCD_ShowStringMy(50,150,400,24,24,"1.添加指纹  2.删除指纹  3.返回");
                    break;
                    
                case 2:  // 添加指纹界面
                    LCD_Clear(WHITE);
                    Add_FR();         // 添加指纹
                    Show_Message();
                    break;
                    
                case 3:  // 删除指纹界面
                    LCD_Clear(WHITE);
                    LCD_ShowStringMy(100,100,300,24,24,"删除指纹");
                    LCD_ShowStringMy(50,150,400,24,24,"1.删除单个  2.清空指纹库  3.返回");
                    break;
                    
                case 4:  // 删除单个指纹
                    LCD_Clear(WHITE);
                    Del_FR();
                    Show_Message();
                    break;
                    
                case 5:  // 清空指纹库
                    LCD_Clear(WHITE);
                    Clean_FR();
                    Show_Message();
                    break;
            }
            flag = 0;  // 清除显示标志
        }
        
        // 按键处理
        key = KeyScan();
        switch(local_mode)
        {
            case 1:  // 指纹管理主界面
                if(key == 1) {
                    local_mode = 2;  // 进入添加指纹
                    flag = 1;
                }
                else if(key == 2) {
                    local_mode = 3;  // 进入删除指纹
                    flag = 1;
                }
                else if(key == 3) {
                    LCD_Clear(WHITE);
                    return;          // 返回上级菜单
                }
                break;
                
            case 2:  // 添加指纹界面
                if(key == 1) {
                    local_mode = 1;  // 返回主界面
                    flag = 1;
                }
                break;
                
            case 3:  // 删除指纹界面
                if(key == 1) {
                    local_mode = 4;  // 删除单个指纹
                    flag = 1;
                }
                else if(key == 2) {
                    local_mode = 5;  // 清空指纹库
                    flag = 1;
                }
                else if(key == 3) {
                    local_mode = 1;  // 返回主界面
                    flag = 1;
                }
                break;
                
            case 4:  // 删除单个指纹
            case 5:  // 清空指纹库
                if(key == 1) {
                    local_mode = 3;  // 返回删除指纹界面
                    flag = 1;
                }
                break;
        }
        
        delay_ms(100);  // 延时防抖
    }
}
void menuset()
{

	      LCD_DrawPicture(60, 20, 210, 170, (u8*)gImage_photo);	  			
        LCD_ShowStringMy(110,180,200,24,24,"Photo");
	
        LCD_DrawPicture(250, 20, 400, 170, (u8*)gImage_timer);	  		
	      LCD_ShowStringMy(310,180,200,24,24,"Time");
	
	      LCD_DrawPicture(60, 210, 210, 360, (u8*)gImage_gps1);	  		
	      LCD_ShowStringMy(120,370,200,24,24,"GPS");
	
				LCD_DrawPicture(250, 210, 400, 360, (u8*)gImage_phone);	  		
	      LCD_ShowStringMy(310,370,200,24,24,"Phone");
				
				LCD_DrawPicture(60, 400, 210, 550, (u8*)gImage_a);	  		
	      LCD_ShowStringMy(110,560,200,24,24,"TandW");
	
				LCD_DrawPicture(250, 400, 400, 550, (u8*)gImage_laba);	  		
	      LCD_ShowStringMy(310,560,200,24,24,"MP3");
				
				// LCD_DrawPicture(60, 590, 210, 740, (u8*)gImage_light);	  		
	      // LCD_ShowStringMy(110,750,200,24,24,"Light");
	
				// LCD_DrawPicture(250, 590, 400, 740, (u8*)gImage_motor);	  		
	      // LCD_ShowStringMy(310,750,200,24,24,"Motor");
	
	
}
// void Light()
// {
// 	u8 tempLIGHT=0;
//   LCD_Clear(WHITE);
// 	LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
//   gui_fill_circle(240,400,150,BLACK);
// 	          while(1)
// 								{
									
// 								tempLIGHT = KeyScan();
// 								if(tempLIGHT == 1)
// 								{
// 								gui_fill_circle(240,400,150,RED);
// 								LED1_ON;
// 	              delay_ms(500);
// 								LED1_OFF;
// 								LED2_ON;
// 								delay_ms(500);
// 								LED2_OFF;
//                 LED3_ON;
//                 delay_ms(500);
//                 LED3_OFF;
//                 LED4_ON;
//                 delay_ms(500);
//                 LED4_OFF;
//                 LED5_ON;
//                 delay_ms(500);
//                 LED5_OFF;
//                 LED6_ON;
//                 delay_ms(500);
//                 LED6_OFF;
//                 LED7_ON;
//                 delay_ms(500);
//                 LED7_OFF;
//                 LED8_ON;
//                 delay_ms(500);
//                 LED8_OFF;
                
//                   LED1_ON;
//                   LED2_ON;
//                   LED3_ON;
//                   LED4_ON;
//                   LED5_ON;
//                   LED6_ON;
//                   LED7_ON;
//                   LED8_ON;
// 								}
// 								else if(tempLIGHT == 2)
// 								{
// 								  gui_fill_circle(240,400,150,GREEN); 
//                   LED1_OFF;
//                   LED2_OFF;
//                   LED3_OFF;
//                   LED4_OFF;
//                   LED5_OFF;
//                   LED6_OFF;
//                   LED7_OFF;
//                   LED8_OFF;
// 								}
// 								else if(tempLIGHT == 3)
// 								{
// 								 break;
// 								}
// 								}
// 								while(1)
// 								{
// 									Exit();
// 								  if(exit_flag == 1)
//                     {
//                         light_flag = 0;
// 											  LCD_Clear(WHITE);
// 											 // BEEP_ON;
//                         break; 
//                     }
// 								}

// }
void photo()
{
                u8 tempKEY=0;
	              u8 number=1;
								LCD_Clear(WHITE);
								LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
								LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl1);	
							  LCD_DrawPicture(90, 600, 190, 700, (u8*)gImage_shang);
                LCD_DrawPicture(300, 600, 400, 700, (u8*)gImage_xia);
	              LCD_ShowStringMy(120,720,450,24,24,"S1");
	              LCD_ShowStringMy(340,720,450,24,24,"S2");
								while(1)
								{
								tempKEY = KeyScan();
								if(tempKEY == 1)
								{
//									BEEP_ON;
								  number--;
									if(number<=0)
										number=1;
								  switch(number)
                  {
										case 1:
									      LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl1);	break;
										case 2:
											  LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl2);	break;
									  // case 3:
									  //     LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl3);	break;
                    // case 4:
                    //     LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl4);	break;
									}
								}
								else if(tempKEY == 2)
								{
								  number++;
									if(number>=2)
										number=2;
								  switch(number)
                  {
										case 1:
									      LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl1);	break;
										case 2:
											  LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl2);	break;
									  // case 3:
									  //     LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl3);	break;
                    // case 4:
                    //     LCD_DrawPicture(80, 200, 400, 440, (u8*)gImage_girl4);	break;
									}
								}
								else if(tempKEY == 3)
								{
								 break;
								}
								}
								while(1)
								{
									Exit();
								  if(exit_flag == 1)
                    {
                        photo_flag = 0;
											  LCD_Clear(WHITE);
                        break; 
                    }
								}

}
void LCD_ShowChar2(u16 x, u16 y, u8 num, u8 size, u16 PenColor, u16 BackColor)
{       
    u8 temp, t1, t;
	u16 y0 = y;
    
	u8 csize = (size/8 + ((size%8)? 1 : 0))*(size/2);		            //  得到字体一个字符对应点阵集所占的字节数	
    
    //  得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
 	num = num - ' ';                                                    
	
    for(t = 0; t < csize; t++)
	{   
		if (size == 12)
        {
            temp = asc2_1206[num][t]; 	 	                            //  调用1206字体
        }
        
		else if(size == 16) 
        {
            temp = asc2_1608[num][t];	                                //  调用1608字体
        }
        
		else if(size == 24)
        {
            temp = asc2_2412[num][t];	                                //  调用2412字体
        }
		else 
        {
            return;								                        //  没有的字库
        }
        
		for(t1 = 0; t1 < 8; t1++)
		{			    
			if(temp&0x80)
            {
                LCD_Fast_DrawPoint(x, y, PenColor);
            }
            
			else
            {
                LCD_Fast_DrawPoint(x, y, BackColor);
            }
            
			temp <<= 1;
            
			y++;
			
            if (y >= lcddev.height)
            {
                return;		//超区域了
            }
            
			if((y - y0) == size)
			{
				y = y0;
				
                x++;
				
                if(x >= lcddev.width)
                {
                    return;	//超区域了
                }
				break;
			}
		}  	 
	}  	    	
}
void LCD_ShowCharString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t PenColor, uint16_t BackColor)
{   
	uint8_t size = 16;                                                  //  字符大小默认16*8
	 
    if(x > lcddev.width)                                                //  超出X轴字体最小单位，换行
    {
        x = 0;
        y += size;
    }			         
    
    if(y > lcddev.height)                                               //  超出Y轴字体最小单位，回到原点，并且清屏
    {
        y = x = 0;
        LCD_Clear(WHITE);
    }	 
    
    LCD_ShowChar2(x, y, *p, size, PenColor, BackColor);			   		//	0表示非叠加方式
}
void LCD_ShowString2(u16 x0, u16 y0, u8 *pcStr, u16 PenColor, u16 BackColor)
{
    while(*pcStr)
	{
		if(*pcStr > 0xa1)                                               //  显示汉字
		{
			LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
			pcStr += 2;
			x0 += 16;
		}
		else                                                            //  显示字符
		{
			LCD_ShowCharString(x0, y0, pcStr, PenColor, BackColor);	
			pcStr +=1;
			x0+= 8;
		}
	}	
}
void Gps()
{
  char dispbuf[100] = "";
	u8 key=0;
  LCD_Clear(WHITE);
	   
	LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
	GPS_Buff_Init();
	LCD_ShowString2(150, 50, "GPS全球实时定位系统", BLACK, WHITE);	
	while (1)
    {
        if(RxDataOutPtr != RxDataInPtr)   //if成立的话，说明接收缓冲区有数据了
		{
			if (memcmp(&RxDataOutPtr[2], "$GNGGA", 6) == 0)
			{
				GNGGA_info(&RxDataOutPtr[2]);
				sprintf(dispbuf, "%s", gps_time);
				//LCD_ShowString2(150, 200, (u8*)dispbuf,BLACK, WHITE);
				LCD_ShowStringMy(80,300,200, 24,24,"Time:");
				LCD_ShowStringMy(300,300,200, 24,24,(u8*)dispbuf);	
				sprintf(dispbuf, "%6.2f%c", longitude,longitude_dir);
//				LCD_ShowString2(150, 220, (u8*)dispbuf,BLACK, WHITE);
				LCD_ShowStringMy(80,350,200, 24,24,"Longitude:");
				LCD_ShowStringMy(300,350,200, 24,24,(u8*)dispbuf);
				sprintf(dispbuf, "%6.2f%c", latitude,latitude_dir);
//				LCD_ShowString2(150, 240, (u8*)dispbuf,BLACK, WHITE);
				LCD_ShowStringMy(80,400,200, 24,24,"Latitude:");
				LCD_ShowStringMy(300,400,200, 24,24,(u8*)dispbuf);
			}
			else if (memcmp(&RxDataOutPtr[2], "$GPGSV", 6) == 0)
			{
				GSV_info(&RxDataOutPtr[2]);
				sprintf(dispbuf, "   %d\r\n ",gpgsv.numSv);
				//LCD_ShowString2(150, 300, (u8*)dispbuf,BLACK, WHITE);
				LCD_ShowStringMy(80,450,200, 24,24,"GPS:");
				LCD_ShowStringMy(300,450,200, 24,24,(u8*)dispbuf);
			}
			else if (memcmp(&RxDataOutPtr[2], "$BDGSV", 6) == 0)
			{
				GSV_info(&RxDataOutPtr[2]);
				sprintf(dispbuf, "   %d ",gpgsv.numSv);
				//LCD_ShowString2(150, 320, (u8*)dispbuf,BLACK, WHITE);
				LCD_ShowStringMy(80,500,200, 24,24,"Beidou:");
				LCD_ShowStringMy(300,500,200, 24,24,(u8*)dispbuf);
			}
			RxDataOutPtr += RBUFF_UNIT;                       //指针下移
			if(RxDataOutPtr == RxDataEndPtr)                  //如果指针到缓冲区尾部了
				RxDataOutPtr = RxDataBuf[0];                  //指针归位到缓冲区开头
		}
		key = KeyScan();
		if(key==3)
			break;	
    }
		while(1)
		{
			Exit();
			if(exit_flag == 1)
      {
      gps_flag = 0;
			LCD_Clear(WHITE);
			// BEEP_ON;
      break; 
      }
		}
}


void music()
{
	u8 i=0;
	u8 flag=0;
	u8 voice=15;
	u8 number=1;
	u8 showH[2],showL[2];
	float tempadc;
  LCD_Clear(WHITE);	
	LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
	setMp3Vol(voice);
	init_CH455();
  CH455_Write( CH455_SYSON ); 
	LCD_ShowStringMy(50, 500, 400,24,24,"Press K1  to start    playing");
	LCD_ShowStringMy(50, 530, 400,24,24,"Press K2  to stop     playing");
	LCD_ShowStringMy(50, 560, 400,24,24,"Press K3  to start    next   ");
	LCD_ShowStringMy(50, 590, 400,24,24,"Press K4  to start    last   ");
	LCD_ShowStringMy(50, 620, 400,24,24,"Press K5  to increase volume ");
	LCD_ShowStringMy(50, 650, 400,24,24,"Press K6  to decrease volume ");
	LCD_ShowStringMy(50, 680, 400,24,24,"Press K7  to start    looping");
	LCD_ShowStringMy(50, 710, 400,24,24,"Press K8  to stop     looping");
	LCD_ShowStringMy(50, 740, 400,24,24,"Press K16 to exit     mp3    ");
	LCD_ShowStringMy(0, 470, 470,24,24,"Real time volume on the right screen");
	showL[0] = BCD_decode_tab[voice%10];
	showH[0] = BCD_decode_tab[voice/10];
	showL[1] = BCD_decode_tab[0];
	showH[1] = BCD_decode_tab[0];
	CH455_Write( CH455_DIG3 | showL[0] );	
	CH455_Write( CH455_DIG2 | showH[0] ); 
	CH455_Write( CH455_DIG1 | showL[1] |0x80); 
	CH455_Write( CH455_DIG0 | showH[1] ); 
while(1)
	{
	#ifdef USE_CH455_KEY
		 ch455_key = 0xff;
		 while( ch455_key == 0xff );
		 i = ch455_key & 0x3f;//按键值
        if( i<=7 )
        {
           i=i-3;
        }else if( 12<=i & i<=15 )
        {
           i=i-7;
        }else if( 20<=i & i<=23 )
        {
           i=i-11;
        }else
        {
           i=i-15;
        }
//以下是对应数字键值1-16
        if( i==1 )
        {
				LCD_ShowStringMy(200, 50, 200,24,24,"Play ");
			
					if(flag==0)
					{ 
						
						flag=1;
						showH[1] = BCD_decode_tab[0];
		        showL[1] = BCD_decode_tab[number]; 
					}
						mp3Play(); 
        }
				else if(i==2)
        {
				mp3Stop();              //  停止
        LCD_ShowStringMy(200, 50, 200,24,24,"Stop ");
        }
				else if(i==3)
        {
				mp3next();              //  下一曲
				number++;
				if(number>=4)
						 number=1;
        LCD_ShowStringMy(200,150, 200,24,24,"Next ");
				showH[1] = BCD_decode_tab[0];
		    showL[1] = BCD_decode_tab[number]; 	
        }
				else if(i==4)
        {
				mp3last();              //  上一曲
        LCD_ShowStringMy(200, 150, 200,24,24,"Last ");
				number--;
					if(number<=0)
						number=3;
				showH[1] = BCD_decode_tab[0];
		    showL[1] = BCD_decode_tab[number]; 
        }
				else if(i==5)
        {
				mp3voiceadd();              //  音量加
				voice++;
				if(voice>30)
						voice=30;
        LCD_ShowStringMy(200, 250, 200,24,24,"Loud ");
				showH[0] = BCD_decode_tab[voice/10];
		    showL[0] = BCD_decode_tab[voice%10]; 
        }
				else if(i==6)
        {
				mp3voicedet();              // 音量减
				voice--;	
					if(voice<0)
						 voice=0;
        LCD_ShowStringMy(200, 250, 200,24,24,"Small");
				showH[0] = BCD_decode_tab[voice/10];
		    showL[0] = BCD_decode_tab[voice%10]; 
        }
				else if(i==7)
        {
				mp3loopplay();              //循环播放开始
        LCD_ShowStringMy(200,350, 200,24,24,"LoopStart");
        }
				else if(i==8)
        {
				mp3loopstop();              // 循环播放结束
        LCD_ShowStringMy(200,350, 200,24,24,"LoopEnd  ");
        }
				
				else if(i==16)   //退出
				{
        LCD_ShowStringMy(200, 450, 200,24,24,"Exit ");
				break;
        }
			 CH455_Write( CH455_DIG3 | showL[0] );	
	     CH455_Write( CH455_DIG2 | showH[0] ); 
	     CH455_Write( CH455_DIG1 | showL[1] |0x80 ); 
	     CH455_Write( CH455_DIG0 | showH[1] );
		 while(1)
		 {
		    i = CH455_Read( );        //读按键数值
			  if( i & 0x40 )            //按键按下没有释放
			  {
			    CH455_Write( CH455_DIG3 );
	        CH455_Write( CH455_DIG2 );
          delay_ms(50);
				  CH455_Write( CH455_DIG3 | showL[0] );	
	        CH455_Write( CH455_DIG2 | showH[0] );
			    delay_ms(50);
					//BEEP1();
			  }
			  else//按键已经释放
			  {
			      break;
			  }
		  }
    #endif
		
	}
	while(1)
		{
			Exit();
			if(exit_flag == 1)
      {
      phone_flag = 0;
			LCD_Clear(WHITE);
			// BEEP_ON;
      break; 
      }
		}
}
void tandw()
{
	unsigned char temperature;
	unsigned char humidity;
	u8 key;
  char dispbuf[100] = "";
  LCD_Clear(WHITE);	
	LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
	LCD_ShowString2(150, 50, "温湿度传感器报警系统", BLACK, WHITE);	
  while(1)
    {
        DTH11_ReadData(&temperature, &humidity);		                //  读取温湿度值
        sprintf(dispbuf, "%d C", temperature);
       // LCD_ShowString2(180, 200, (u8*)dispbuf,BLACK, WHITE);
			  LCD_ShowStringMy(100,200,200, 24,24,"Temperature：");
				LCD_ShowStringMy(300,200,200, 24,24,(u8*)dispbuf);	
		    sprintf(dispbuf, "%d% %", humidity);
        //LCD_ShowString2(180, 220, (u8*)dispbuf,BLACK, WHITE);
		   	LCD_ShowStringMy(100,250,200, 24,24,"Humidity：");
				LCD_ShowStringMy(300,250,200, 24,24,(u8*)dispbuf);	
//        delay_ms(1000);
			  if(temperature>=35)
				{
					gui_fill_circle(150,400,70,RED); 
					LCD_ShowStringMy(110, 500, 200,24,24,"Too High");
					BEEP2();
					delay_ms(1000);
				}
				if(temperature<=10)
				{
					gui_fill_circle(150,400,70,BLUE); 
					LCD_ShowStringMy(110, 500, 200,24,24,"Too Cold");
					BEEP2();
					delay_ms(1000);
				}
				else
				{
					gui_fill_circle(150,400,70,GREEN); 
					LCD_ShowStringMy(110, 500, 200,24,24,"Allright");
					
				}
				if(humidity>=70)
				{
					gui_fill_circle(350,400,70,RED); 
					LCD_ShowStringMy(300, 500, 200,24,24,"Too Wet");
					BEEP2();
					delay_ms(1000);
				}
				if(humidity<=10)
				{
					gui_fill_circle(350,400,70,BLUE); 
					LCD_ShowStringMy(300, 500, 200,24,24,"Too Dry");
					BEEP2();
					delay_ms(1000);
				}
				else
				{
					gui_fill_circle(350,400,70,GREEN); 
					LCD_ShowStringMy(300, 500, 200,24,24,"Nor mal");
					delay_ms(1000);
				}
		key = KeyScan();
		if(key==3)
			break;	
    }
while(1)
		{
			Exit();
			if(exit_flag == 1)
      {
      temwet_flag = 0;
			LCD_Clear(WHITE);
			// BEEP_ON;
      break; 
      }
		}
}
// void motor()
// {
// 	u8 t=0;
// 	unsigned int data = 0;
//   char dispbuf[100] = "";
// //StepMotorGpio_Init(); 	
//   LCD_Clear(WHITE);	
	
// 	LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
// 	LCD_ShowStringMy(150, 50, 200,24,24,"Motor Imitate");
// 	LCD_ShowStringMy(100, 200,200,24,24, "Duty Cycle:0");
// // while(1)
// //	{
// //        temp = KeyScan();
// //		//步进电机
// //        //  S1正转
// ////        if (temp == 1)
// ////        {
// ////					  LCD_ShowString2(200, 200, "正转",BLACK, WHITE);
// ////            StepMotorTest(0);
// ////            
// ////        }
// ////        //  S2反转
// ////        else if (temp == 2)
// ////        {
// ////					  LCD_ShowString2(200, 200, "反转",BLACK, WHITE);
// ////            StepMotorTest(1);  
// ////        }
// //		//直流电机
// //		
// //	} 
// 	while(1)
// 	{
//         t = KeyScan();           //  扫描按键
//         if (t == 1)              //  按键S2
//         {
//             data += 200;
//             if(data > 1000)
//             {
//                 data = 1000;
//             }
            
//             TIM_SetCompare1(TIM10, data);       //  占空比增加
//             sprintf(dispbuf, "Duty Cycle:%-5d", data);  //  显示高电平数值
// //            LCD_ShowString2(200, 200, (u8*)dispbuf,BLACK, WHITE);
// 						LCD_ShowStringMy(100, 200,200,24,24, (u8*)dispbuf);
//         }
// 				else if(t == 2)         
//         {
//             if(data!=0)    
//             {
//                 data -= 200;
//                 if(data < 100)
//                 {
//                     data = 0;
//                 }
//             }
//             TIM_SetCompare1(TIM10, data);          
//             sprintf(dispbuf, "Duty Cycle:%-5d", data);  
//             LCD_ShowStringMy(100, 200,200,24,24, (u8*)dispbuf);
//         }
// 				else if(t == 3) 
// 				{
// 				TIM_SetCompare1(TIM10, 0);
// 				LCD_ShowStringMy(100, 200,200,24,24, "Duty Cycle:0");
// 				break;
// 				}

//     }
// 	while(1)
// 		{
// 			Exit();
// 			if(exit_flag == 1)
//       {
//       motor_flag = 0;
// 			LCD_Clear(WHITE);
// 			// BEEP_ON;
//       break; 
//       }
// 		}
// }
void closeforphone()    //锁屏界面的设计
{
	  
	  int i=0,j=0;
	  int step=150;
	  int step2=60;
	  int star_x=90;
	  int star_y=230; 
	  int k=1;
	  LCD_Clear(WHITE);	
	  LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
	  for(i=0;i<3*step;i=i+step)
	     for(j=0;j<4*step;j=j+step)  
         LCD_Draw_Circle(star_x+i,star_y+j,50);
	  POINT_COLOR = BLACK;  
	  for(i=0;i<3*step;i=i+step)
	     for(j=0;j<3*step;j=j+step)
        { 
				 LCD_ShowNum(star_x-4+j, star_y-10+i,k,1,24);
	       k=k+1;
				}
	  LCD_ShowStringMy(star_x-10, star_y-10+step*3, 200, 24,24, "AC");
	  LCD_ShowStringMy(star_x-4+step, star_y-10+step*3, 200, 24,24, "0");
	  LCD_ShowStringMy(star_x-4+step*2, star_y-10+step*3, 200, 24,24, "X");
		LCD_ShowStringMy(star_x-10, star_y-10+step*3+65, 200, 24,24, "K12");
		LCD_ShowStringMy(star_x-4+step, star_y-10+step*3+65, 200, 24,24, "K10");
		LCD_ShowStringMy(star_x-4+step*2, star_y-10+step*3+65, 200, 24,24, "K11");
//		POINT_COLOR = GREEN;		
    LCD_DrawRectangle(40,100,440,180,BLACK);
}
void phone()
{
	u8 i;
	u8 key;
  u8 showH[2],showL[2];
	u8 number[20]={0};
	u8 n=0;u8 j=0;u8 k=0;
	char dispbuf[100] = "";
	u32 data=0;
	u32 datatem=0;
	closeforphone();		
	init_CH455();
  CH455_Write( CH455_SYSON );
	showL[0] = BCD_decode_tab[0];
	showH[0] = BCD_decode_tab[0];
	showL[1] = BCD_decode_tab[0];
	showH[1] = BCD_decode_tab[0];
	CH455_Write( CH455_DIG3 | showL[0] );	
	CH455_Write( CH455_DIG2 | showH[0] ); 
	CH455_Write( CH455_DIG1 | showL[1] ); 
	CH455_Write( CH455_DIG0 | showH[1] ); 
  
while(1)
	{
	#ifdef USE_CH455_KEY
		 ch455_key = 0xff;
//		 key=KeyScan();
//		 if(key==3)
//				break;
		 while( ch455_key == 0xff );
		 i = ch455_key & 0x3f;//按键值
        if( i<=7 )
        {
           i=i-3;
        }else if( 12<=i & i<=15 )
        {
           i=i-7;
        }else if( 20<=i & i<=23 )
        {
           i=i-11;
        }else
        {
           i=i-15;
        }
//以下是对应数字键值1-16
        if( i<10 )
        {
				    data=data*10;
				datatem=i;
			  data=data+datatem;
				sprintf(dispbuf, "%d", data);  
				LCD_ShowStringMy(200,140,200, 24,24,(u8*)dispbuf);	
        //LCD_ShowString2(200, 150, (u8*)dispbuf, BLACK, WHITE);
        showH[0] = BCD_decode_tab[0];
		    showL[0] = BCD_decode_tab[i]; 
				BEEP3();
				
        }
				else if(i==10)
        {
					data=data*10;
				  sprintf(dispbuf, "%d", data);  
				  LCD_ShowStringMy(200, 140,200, 24,24,(u8*)dispbuf);	
         showH[0] = BCD_decode_tab[i/10];
		     showL[0] = BCD_decode_tab[i-10];
				 BEEP3();
			
        }
				else if(i==11)//x
				{
					closeforphone();	
					data=data/10;
				  sprintf(dispbuf, "%d", data);  
				  LCD_ShowStringMy(200, 140,200, 24,24,(u8*)dispbuf);	
         showH[0] = BCD_decode_tab[i/10];
		     showL[0] = BCD_decode_tab[i-10];
				 BEEP3();
				 n++;
        }	
				else if(i==12)   //ac
				{
					data=0;
					closeforphone();	
				  //sprintf(dispbuf, "%d", data);  
				 // LCD_ShowStringMy(200, 150,200, 24,24,(u8*)dispbuf);	
         showH[0] = BCD_decode_tab[i/10];
		     showL[0] = BCD_decode_tab[i-10];
				 BEEP3();
				
        }
				else if(i==16)   //退出
				{

				  break;
        }
		 //数码管显示之间的移位操作
       CH455_Write( CH455_DIG3 | showL[0] );	
	     CH455_Write( CH455_DIG2 | showH[0] ); 
	     CH455_Write( CH455_DIG1 | showL[1] ); 
	     CH455_Write( CH455_DIG0 | showH[1] );

		 //判断按键有没有释放
		 while(1)
		 {
		    i = CH455_Read( );        //读按键数值
			  if( i & 0x40 )            //按键按下没有释放
			  {
					
			      //闪烁
			    CH455_Write( CH455_DIG3 );
	        CH455_Write( CH455_DIG2 );
          delay_ms(50);
				  CH455_Write( CH455_DIG3 | showL[0] );	
	        CH455_Write( CH455_DIG2 | showH[0] );
			    delay_ms(50);
					//BEEP1();
			  }
			  else//按键已经释放
			  {
			      break;
			  }
		  }
    #endif
		
	}
	while(1)
		{
			Exit();
			if(exit_flag == 1)
      {
      phone_flag = 0;
			LCD_Clear(WHITE);
			// BEEP_ON;
      break; 
      }
		}

}
void timescreen()
{
  LCD_Clear(WHITE);
	LCD_DrawPicture(0, 0, 100, 100, (u8*)gImage_back);
	gui_fill_circle(240,400,150,WHITE); 

}
void timer()
{
  char dispbuf[100] = "";
	u8 key=0;
	u8 rs=120;
	u8 rm=90;
	u8 rh=60;
	u8 showH[2],showL[2];
	u8	m0;u8	m1;u8	s0;u8	s1;u8 s;u8 m;u8	h0;u8	h1;u8	h;
	float angle = 6.0;   // ???
  float radian = angle * 3.1415926 / 180.0;     	
  timescreen();
	init_CH455();
  CH455_Write( CH455_SYSON );
	showL[0] = BCD_decode_tab[0];
	showH[0] = BCD_decode_tab[0];
	showL[1] = BCD_decode_tab[0];
	showH[1] = BCD_decode_tab[0];
	CH455_Write( CH455_DIG3 | showL[0] );	
	CH455_Write( CH455_DIG2 | showH[0] ); 
	CH455_Write( CH455_DIG1 | showL[1] ); 
	CH455_Write( CH455_DIG0 | showH[1] ); 
	GPS_Buff_Init();
	LCD_Draw_Circle(240,400,180);
	while (1)
    {
        if(RxDataOutPtr != RxDataInPtr)   //if成立的话，说明接收缓冲区有数据了
		{
			if (memcmp(&RxDataOutPtr[2], "$GNGGA", 6) == 0)
			{
				GNGGA_info(&RxDataOutPtr[2]);
				sprintf(dispbuf, "%s", gps_time);
				LCD_ShowStringMy(190,180,200, 24,24,(u8*)dispbuf);	
				h0=dispbuf[0]-'0';
				h1=dispbuf[1]-'0';
				m0=dispbuf[3]-'0';
				m1=dispbuf[4]-'0';
				s0=dispbuf[6]-'0';
				s1=dispbuf[7]-'0';
				s=s0*10+s1;
				m=m0*10+m1;
				h=h0*10+h1;
				showL[0] = BCD_decode_tab[s1];
	      showH[0] = BCD_decode_tab[s0];
	      showL[1] = BCD_decode_tab[m1];
	      showH[1] = BCD_decode_tab[m0];
			  CH455_Write( CH455_DIG3 | showL[0] );	
	      CH455_Write( CH455_DIG2 | showH[0] ); 
	      CH455_Write( CH455_DIG1 | showL[1]|0x80 ); 
	      CH455_Write( CH455_DIG0 | showH[1] );
				gui_fill_circle(240,400,150,WHITE); 
				
				LCD_DrawLine(240,400,240+rs*sin(angle *s* 3.1415926 / 180.0),400-rs*cos(angle *s* 3.1415926 / 180.0));
				LCD_DrawLine(240,400,240+rm*sin(angle *m* 3.1415926 / 180.0),400-rm*cos(angle *m* 3.1415926 / 180.0));
				LCD_DrawLine(240,400,240+rh*sin(30.0 *h* 3.1415926 / 180.0),400-rh*cos(30.0 *h* 3.1415926 / 180.0));
			}
			RxDataOutPtr += RBUFF_UNIT;                       //指针下移
			if(RxDataOutPtr == RxDataEndPtr)                  //如果指针到缓冲区尾部了
				RxDataOutPtr = RxDataBuf[0];                  //指针归位到缓冲区开头
		}
		key = KeyScan();
		if(key==3)
		{
			showL[0] = BCD_decode_tab[0];
	    showH[0] = BCD_decode_tab[0];
	    showL[1] = BCD_decode_tab[0];
	showH[1] = BCD_decode_tab[0];
	CH455_Write( CH455_DIG3 | showL[0] );	
	CH455_Write( CH455_DIG2 | showH[0] ); 
	CH455_Write( CH455_DIG1 | showL[1] ); 
	CH455_Write( CH455_DIG0 | showH[1] ); 
			break;	
		}
    }
		while(1)
		{
			Exit();
			if(exit_flag == 1)
      {
      time_flag = 0;
			LCD_Clear(WHITE);
			// BEEP_ON;
      break; 
      }
		}


}
void apps()
{
    u8 t = 0;
    u8 i = 0;
    u16 lastpos[5][2];		//最后一次的数据
    int f[8]={0,0,0,0,0,0,0,0};
	  LED1_OFF;
	LED2_OFF;
	  LED3_OFF;
	LED4_OFF;
	  LED5_OFF;
	LED6_OFF;
	  LED7_OFF;
	  LED8_OFF;
    while(1)
    {
        tp_dev.scan(0);

        for(t = 0; t < 5; t++)
        {
            if((tp_dev.sta) & (1 << t))
            {
                if(tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)
                {
                    if(lastpos[t][0] == 0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }
								}
						}
						if(exit_flag == 1)
						{
							    //LCD_Clear(WHITE);
              menuset();
							photo_flag=0;
							light_flag=0;
							gps_flag=0;
							mp3_flag=0;
							temwet_flag = 0;
							motor_flag = 0;
							phone_flag = 0;
							time_flag=0;
						}
                if(tp_dev.x[t]>60&&tp_dev.x[t]<210&&tp_dev.y[t]>20&&tp_dev.y[t]<170&&f[0]==0)  //photo
                {
                    f[0] = 1;
									  photo_flag=1;
                }
								// if(tp_dev.x[t]>60&&tp_dev.x[t]<210&&tp_dev.y[t]>590&&tp_dev.y[t]<740&&f[1]==0)  //light
                // {
								// 	  f[1] = 1;
                //     light_flag=1;
									
                // }
								if(tp_dev.x[t]>60&&tp_dev.x[t]<210&&tp_dev.y[t]>210&&tp_dev.y[t]<360&&f[2]==0)  //gps
                {
                    f[2] = 1;
									  gps_flag=1;
									
                }
								if(tp_dev.x[t]>250&&tp_dev.x[t]<400&&tp_dev.y[t]>400&&tp_dev.y[t]<550&&f[3]==0)  //gps
                {
                    f[3] = 1;
									  mp3_flag=1;
									
                }
								if(tp_dev.x[t]>60&&tp_dev.x[t]<210&&tp_dev.y[t]>400&&tp_dev.y[t]<550&&f[4]==0)  //gps
                {
                    f[4] = 1;
									  temwet_flag=1;
									
                }
								// if(tp_dev.x[t]>250&&tp_dev.x[t]<400&&tp_dev.y[t]>590&&tp_dev.y[t]<740&&f[5]==0)  //gps
                // {
                //     f[5] = 1;
								// 	  motor_flag=1;
									
                // }
								if(tp_dev.x[t]>250&&tp_dev.x[t]<400&&tp_dev.y[t]>210&&tp_dev.y[t]<360&&f[6]==0)  //gps
                {
                    f[6] = 1;
									  phone_flag=1;
									
                }
								if(tp_dev.x[t]>250&&tp_dev.x[t]<400&&tp_dev.y[t]>21&&tp_dev.y[t]<170&&f[7]==0)  //gps
                {
                    f[7] = 1;
									  time_flag=1;
									
                }
                // 当触摸点不在photo区域内时，清除f[0]
                if (tp_dev.x[t] < 60 || tp_dev.x[t] > 210 || tp_dev.y[t] < 20 || tp_dev.y[t] > 170)
                {
                    f[0] = 0;
                }

                // 当触摸点不在light区域内时，清除f[1]
                if (tp_dev.x[t] < 60 || tp_dev.x[t] > 210 || tp_dev.y[t] < 590 || tp_dev.y[t] > 740)
                {
                    f[1] = 0;
                }

                // 当触摸点不在gps区域内时，清除f[2]
                if (tp_dev.x[t] < 60 || tp_dev.x[t] > 210 || tp_dev.y[t] < 210 || tp_dev.y[t] > 360)
                {
                    f[2] = 0;
                }

                // 当触摸点不在mp3区域内时，清除f[3]
                if (tp_dev.x[t] < 250 || tp_dev.x[t] > 400 || tp_dev.y[t] < 400 || tp_dev.y[t] > 550)
                {
                    f[3] = 0;
                }

                // 当触摸点不在temwet区域内时，清除f[4]
                if (tp_dev.x[t] < 60 || tp_dev.x[t] > 210 || tp_dev.y[t] < 400 || tp_dev.y[t] > 550)
                {
                    f[4] = 0;
                }

                // 当触摸点不在motor区域内时，清除f[5]
                if (tp_dev.x[t] < 250 || tp_dev.x[t] > 400 || tp_dev.y[t] < 590 || tp_dev.y[t] > 740)
                {
                    f[5] = 0;
                }

                // 当触摸点不在phone区域内时，清除f[6]
                if (tp_dev.x[t] < 250 || tp_dev.x[t] > 400 || tp_dev.y[t] < 210 || tp_dev.y[t] > 360)
                {
                    f[6] = 0;
                }

                // 当触摸点不在time区域内时，清除f[7]
                if (tp_dev.x[t] < 250 || tp_dev.x[t] > 400 || tp_dev.y[t] < 21 || tp_dev.y[t] > 170)
                {
                    f[7] = 0;
                }
								//交互
								if(photo_flag)
								{
                   photo();
								}
								// else if(light_flag)
                // {
								// 	Light();
								// }
								else if(gps_flag)
                {
									Gps();
								}
								else if(mp3_flag)
                {
									music();
								}
								else if(temwet_flag)
                {
									tandw();
								}
								// else if(motor_flag)
                // {
								// 	motor();
								// }
								else if(phone_flag)
                {
									phone();
								}
								else if(time_flag)
                {
									timer();
								}
								
								
        }
        delay_ms(5);
        i++;
    }
}	
