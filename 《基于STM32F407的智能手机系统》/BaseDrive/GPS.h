#ifndef GPS_H
#define GPS_H

//#include "bsp.h"	

#define  R_NUM               10     //接收缓冲区个数
#define  RBUFF_UNIT          100   	//接收缓冲区长度

///*********************************************************************************************************
////串口接收数据包结构体
//*********************************************************************************************************/
struct GSVData
{
    unsigned char numMsg;	//  语句总数
	unsigned char msgNo;	//  当前语句编号
	unsigned char numSv;	//  可见卫星总数
    unsigned int SVID;		//  卫星编号
    unsigned char ele;		//  仰角
    unsigned int az;		//  方位角
    unsigned char cn0;		//  载噪比
};
typedef  struct GSVData GSVDATA;
typedef  GSVDATA       *PGSVDATA;

extern unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];       //外部变量声明，数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern unsigned char *RxDataInPtr;                        //外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *RxDataOutPtr;                       //外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *RxDataEndPtr;                       //外部变量声明，指向缓冲区结束的位置

extern GSVDATA gpgsv;


extern float latitude;                                               //  纬度
extern float longitude;                                              //  经度
extern char latitude_dir;
extern char longitude_dir;
extern unsigned char gps_time[50];

void GPS_Buff_Init(void);
	
void GNGGA_info(unsigned char *prdata);
void GSV_info(unsigned char *prdata);

#endif
