#include "WIFI.h"
#include "delay.h"
#include "USART3.h"
#include <string.h>
#include <stdio.h>
#include "LED.h"

unsigned char id;
/**********************************************************************************************************
函数名称：WIFI配置函数
输入参数：无
输出参数：无
函数返回：无
**********************************************************************************************************/
void WIFI_Configuration(void)
{
    unsigned char CWMODE[13]    = "AT+CWMODE=2\r\n";
    unsigned char CWSAP[38]     = "AT+CWSAP=\"ESP826\",\"0123456789\",11,0\r\n";
    unsigned char CIPMUX[13]    = "AT+CIPMUX=1\r\n";
    unsigned char CIPSERVER[21] = "AT+CIPSERVER=1,5000\r\n";
    
    Uart5.ReceiveFinish = 0;
    Uart5.RXlenth = 0;

    //  AT+CWMODE = 2，开启AP模式
    UART5_Senddata(CWMODE, sizeof(CWMODE));
    while(!Uart5.ReceiveFinish);
    Uart5.ReceiveFinish = 0;
    Uart5.RXlenth = 0;

    //  设置模块的WIFI名称和密码
	UART5_Senddata(CWSAP, sizeof(CWSAP));
    while(!Uart5.ReceiveFinish);
    Uart5.ReceiveFinish = 0;
    Uart5.RXlenth = 0;

    //	打开多连接
	UART5_Senddata(CIPMUX, sizeof(CIPMUX));
    while(!Uart5.ReceiveFinish);
    Uart5.ReceiveFinish = 0;
    Uart5.RXlenth = 0;

    //  设置模块服务器端口。模块默认的ip为192.168.4.1，端口为自己设定的5000（默认的为333）
    UART5_Senddata(CIPSERVER, sizeof(CIPSERVER));
    while(!Uart5.ReceiveFinish);
    Uart5.ReceiveFinish = 0;
    Uart5.RXlenth = 0;
}

/**********************************************************************************************************
函数名称：WIFI发送数据函数
输入参数：发送数据首地址和数据长度
输出参数：无
**********************************************************************************************************/
void WIFI_Send(unsigned char *ptr, unsigned int length)
{
    char CIPSEND[] = "";

    sprintf(CIPSEND, "AT+CIPSEND=%d,%d\r\n", id, length);
    
    UART5_Senddata((u8*)CIPSEND, strlen(CIPSEND));
    
    while(!Uart5.ReceiveFinish);
    Uart5.ReceiveFinish = 0;
    Uart5.RXlenth = 0;
    delay_ms(100);
    
    UART5_Senddata(ptr, length);

    delay_ms(100);
}

/**********************************************************************************************************
函数名称：WIFI接收数据
输入参数：无
输出参数：无
函数返回：
**********************************************************************************************************/
void WIFI_Receive(void)
{
    unsigned char len = 0;
    unsigned char IPD[6] = "\r\n+IPD";
    unsigned char ON[2] = "ON";
    unsigned char OFF[3] = "OFF";
    
    if (Uart5.ReceiveFinish == 1)
    {
        if (memcmp(Uart5.Rxbuf, IPD, 6) == 0)   //eg:"\r\n+IPD,0,9:123456789"
        {
            len = Uart5.Rxbuf[9] - 0x30;
            
            id = Uart5.Rxbuf[7] - 0x30;
            
            if (len == 2 && memcmp(&Uart5.Rxbuf[11], ON, 2) == 0)       //  "ON"
            {
                LED2_ON;
                LED3_ON;
                LED4_ON;
            }
            else if(len == 3 && memcmp(&Uart5.Rxbuf[11], OFF, 3) == 0)  //  "OFF"
            {
                LED2_OFF;
                LED3_OFF;
                LED4_OFF;
            }
        }
        
        Uart5.ReceiveFinish = 0;
        Uart5.RXlenth = 0;
    }
}

