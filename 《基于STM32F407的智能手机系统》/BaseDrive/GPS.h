#ifndef GPS_H
#define GPS_H

//#include "bsp.h"	

#define  R_NUM               10     //���ջ���������
#define  RBUFF_UNIT          100   	//���ջ���������

///*********************************************************************************************************
////���ڽ������ݰ��ṹ��
//*********************************************************************************************************/
struct GSVData
{
    unsigned char numMsg;	//  �������
	unsigned char msgNo;	//  ��ǰ�����
	unsigned char numSv;	//  �ɼ���������
    unsigned int SVID;		//  ���Ǳ��
    unsigned char ele;		//  ����
    unsigned int az;		//  ��λ��
    unsigned char cn0;		//  �����
};
typedef  struct GSVData GSVDATA;
typedef  GSVDATA       *PGSVDATA;

extern unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];       //�ⲿ�������������ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *RxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *RxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *RxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��

extern GSVDATA gpgsv;


extern float latitude;                                               //  γ��
extern float longitude;                                              //  ����
extern char latitude_dir;
extern char longitude_dir;
extern unsigned char gps_time[50];

void GPS_Buff_Init(void);
	
void GNGGA_info(unsigned char *prdata);
void GSV_info(unsigned char *prdata);

#endif
