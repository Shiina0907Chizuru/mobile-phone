#include "GPS.h"	 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GSVDATA gpgsv;

unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];           //数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
unsigned char *RxDataInPtr;                            //指向接收缓冲区存放数据的位置
unsigned char *RxDataOutPtr;                           //指向接收缓冲区读取数据的位置
unsigned char *RxDataEndPtr;                           //指向接收缓冲区结束的位置

unsigned char gps_time[50] = "";
float latitude = 0.0;                                               //  纬度
float longitude = 0.0;                                              //  经度
char latitude_dir;
char longitude_dir;

unsigned char numMsg;	//  语句总数
unsigned char msgNo;	//  当前语句编号
unsigned char numSv;	//  可见卫星总数
unsigned int SVID;		//  卫星编号
unsigned char ele;		//  仰角
unsigned int az;		//  方位角
unsigned char cn0;		//  载噪比

/*----------------------------------------------------------*/
/*函数名：初始化接收,发送,命令数据的 缓冲区 以及各状态参数  */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void GPS_Buff_Init(void)
{
    RxDataInPtr = RxDataBuf[0];             //指向发送缓冲区存放数据的指针归位
    RxDataOutPtr = RxDataInPtr;             //指向发送缓冲区读取数据的指针归位
    RxDataEndPtr = RxDataBuf[R_NUM - 1];    //指向发送缓冲区结束的指针归位
}

/*********************************************************************************************************
函数名称: void GGA_info(unsigned char *prdata)
函数说明：显示信息
输入参数: 接收缓冲区
输出参数: 我
北京时间 = UTC时间 + 0800
GPS输出数据为：abcde.fghi
经纬度计算公式：abc + (de/60) + (fghi/600000)
*********************************************************************************************************/
void GNGGA_info(unsigned char *prdata)
{
    unsigned char i = 0;
    unsigned char x = 0;
    unsigned char abc = 0;
    unsigned char de = 0;
    unsigned int fghi = 0;
    unsigned char str[20];
    
    char Disbuf[20] = "";
	unsigned char hour = 0;
    
    //-------------------------时间部分-----------------------------
    i = 0;
    while(prdata[i++] != ',');                                      //  寻找时间
    
	x = 0;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    if (x > 5)
    {
        hour = (str[0] - '0')*10 + (str[1] - '0');
        hour += 8;
        Disbuf[0] = hour/10 + '0';
        Disbuf[1] = hour%10 + '0';
        Disbuf[2] = ':';
        Disbuf[3] = str[2];
        Disbuf[4] = str[3];
        Disbuf[5] = ':';
        Disbuf[6] = str[4];
        Disbuf[7] = str[5];
        
        memcpy(gps_time, Disbuf, 8);
    }
    else
    {
        Disbuf[0] = '0';
        Disbuf[1] = '0';
        Disbuf[2] = ':';
        Disbuf[3] = '0';
        Disbuf[4] = '0';
        Disbuf[5] = ':';
        Disbuf[6] = '0';
        Disbuf[7] = '0';
        
        memcpy(gps_time, Disbuf, 5);
    }
//    printf("T:%s\r\n",time);
    
    //--------------------------纬度部分----------------------------
    i++;
    x = 0;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
    if (x > 5)
    {
        //  计算纬度
        abc  = (str[0] - '0')*10 + (str[1] - '0');
        de   = (str[2] - '0')*10 + (str[3] - '0');
        fghi = (str[5] - '0')*1000 + (str[6] - '0')*100 + (str[7] - '0')*10 + (str[8] - '0');
        latitude = abc + (de/60) + (fghi/600000.0);
    }
    else
    {
        latitude = 0;
    }
    
    

    //--------------------------------------------------------------
    i++;                                                                //  显示纬度
    if (prdata[i] == 'N')
    {
        latitude_dir = 'N';
//        printf("N:%.3f\r\n",latitude);
    }

    else
    {
        latitude_dir = 'S';
//        printf("S:%.3f\r\n",latitude);
    }

    //--------------------------经度部分----------------------------    
    x = 0;
    i = i + 2;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
    if (x > 5)
    {
        //  计算经度
        abc  = (str[0] - '0')*100 + (str[1] - '0')*10 +(str[2] - '0');
        de   = (str[3] - '0')*10 + (str[4] - '0');
        fghi = (str[6] - '0')*1000 + (str[7] - '0')*100 + (str[8] - '0')*10 + (str[9] - '0');
        longitude = abc + (de/60) + (fghi/600000.0);
    }
    else
    {
        longitude = 0;
    }
    
      

    //  显示经度
    i++;
    if(prdata[i] == 'E')
    {
        longitude_dir = 'E';
//        printf("E:%.3f\r\n",longitude);
    }
    else
    {   
        longitude_dir = 'W';
//        printf("W:%.3f\r\n",longitude);
    }

    //--------------------------定位部分----------------------------    
    i = i + 2;                                                          //  显示定位有效值：0/1
    if(prdata[i++] == '0')
    {   
//	     printf("No location\r\n");              //  未定位
    }
    else
    {   
//	     printf("Location\r\n");                 //  定位
    }
    
    //--------------------------卫星数目----------------------------    
    x = 0;
    i = i + 1;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }

    
    Disbuf[0] = (str[0]- '0')*10 + str[1]- '0';
    
//	printf("卫星数  %d\r\n", Disbuf[0]);
}

/*********************************************************************************************************
函数名称: void GPGSV_info(unsigned char *prdata)
函数说明：显示信息
输入参数: 接收缓冲区
输出参数: 
*********************************************************************************************************/
void GSV_info(unsigned char *prdata)
{
	unsigned char i, j, x;
    unsigned char str[20];
	
    //-------------------------语句总数-----------------------------
    i = 0;
    while(prdata[i++] != ',');
    
	x = 0;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    if (x == 1)
    {
        gpgsv.numMsg = str[0] - '0';
    }
    else
    {
        gpgsv.numMsg = 0;
    }
	
    //--------------------------当前语句编号----------------------------
    i++;
    x = 0;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
    if (x == 1)
    {
        gpgsv.msgNo  = str[0] - '0';
    }
    else
    {
        gpgsv.msgNo = 0;
    }
	
	
    //------------------------可见卫星总数--------------------------------------
    i++;                                                                //  显示纬度
	x = 0;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
	
    if (x > 0)
    {
		gpgsv.numSv = 0;
		
		for (j=0;j<x;j++)
		{
			gpgsv.numSv =  gpgsv.numSv*10 + (str[j]- '0');
		}
        
        if (gpgsv.numSv == 0)
        {
            return;
        }
    }
    else
    {
        gpgsv.numSv = 0;
    }
	
    //----------------------------------------------------------------
	//  卫星编号SVID
	//  仰角ele 		0-90  度
	//  方位角az 		0-359 度
	//  载噪比cn0 		0-99  dB-Hz
	
	//  卫星编号
	i++;
	x = 0;
    while (prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
	if (x > 0)
    {
		gpgsv.SVID = 0;
		
		for (j=0;j<x;j++)
		{
			gpgsv.SVID =  gpgsv.SVID*10 + (str[j]- '0');
		}
    }
    else
    {
        gpgsv.SVID = 0;
    }
	
	//  仰角ele
	i++;
	x = 0;
    while (prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
	
	if (x > 0)
    {
		gpgsv.ele = 0;
		
		for (j=0;j<x;j++)
		{
			gpgsv.ele =  gpgsv.ele*10 + (str[j]- '0');
		}
    }
    else
    {
        gpgsv.ele = 0;
    }
	
	//  方位角az
	i++;
	x = 0;
    while (prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
	if (x > 0)
    {
		gpgsv.az = 0;
		
		for (j=0;j<x;j++)
		{
			gpgsv.az =  gpgsv.az*10 + (str[j]- '0');
		}
    }
    else
    {
        gpgsv.az = 0;
    }
	
	//  载噪比cn0
	i++;
	x = 0;
    while (prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
	if (x > 0)
    {
		gpgsv.cn0 = 0;
		
		for (j=0;j<x;j++)
		{
			gpgsv.cn0 =  gpgsv.cn0*10 + (str[j]- '0');
		}
    }
    else
    {
        gpgsv.cn0 = 0;
    }
	
	if (gpgsv.msgNo == 1)
	{
		printf("-----------------------------------------\r\n");
		if (memcmp(prdata, "$GPGSV", 6) == 0)
		{
			printf("语句总数:%d 可见GPS卫星总数:%d\r\n\r\n", gpgsv.numMsg, gpgsv.numSv);
		}
		else if (memcmp(prdata, "$BDGSV", 6) == 0)
		{
			printf("语句总数:%d 可见北斗卫星总数:%d\r\n\r\n", gpgsv.numMsg, gpgsv.numSv);
		}
		printf("\t卫星编号\t仰角\t方位角\t载噪比\r\n");
	}
    
	printf("%d\t%d\t\t%d\t%d\t%d\r\n",gpgsv.msgNo,gpgsv.SVID, gpgsv.ele,gpgsv.az, gpgsv.cn0);
	
	if (gpgsv.msgNo == gpgsv.numMsg)
		printf("-----------------------------------------\r\n\r\n");
}

