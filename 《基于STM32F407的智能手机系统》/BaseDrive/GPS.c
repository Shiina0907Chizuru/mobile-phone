#include "GPS.h"	 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GSVDATA gpgsv;

unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];           //���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *RxDataInPtr;                            //ָ����ջ�����������ݵ�λ��
unsigned char *RxDataOutPtr;                           //ָ����ջ�������ȡ���ݵ�λ��
unsigned char *RxDataEndPtr;                           //ָ����ջ�����������λ��

unsigned char gps_time[50] = "";
float latitude = 0.0;                                               //  γ��
float longitude = 0.0;                                              //  ����
char latitude_dir;
char longitude_dir;

unsigned char numMsg;	//  �������
unsigned char msgNo;	//  ��ǰ�����
unsigned char numSv;	//  �ɼ���������
unsigned int SVID;		//  ���Ǳ��
unsigned char ele;		//  ����
unsigned int az;		//  ��λ��
unsigned char cn0;		//  �����

/*----------------------------------------------------------*/
/*����������ʼ������,����,�������ݵ� ������ �Լ���״̬����  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void GPS_Buff_Init(void)
{
    RxDataInPtr = RxDataBuf[0];             //ָ���ͻ�����������ݵ�ָ���λ
    RxDataOutPtr = RxDataInPtr;             //ָ���ͻ�������ȡ���ݵ�ָ���λ
    RxDataEndPtr = RxDataBuf[R_NUM - 1];    //ָ���ͻ�����������ָ���λ
}

/*********************************************************************************************************
��������: void GGA_info(unsigned char *prdata)
����˵������ʾ��Ϣ
�������: ���ջ�����
�������: ��
����ʱ�� = UTCʱ�� + 0800
GPS�������Ϊ��abcde.fghi
��γ�ȼ��㹫ʽ��abc + (de/60) + (fghi/600000)
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
    
    //-------------------------ʱ�䲿��-----------------------------
    i = 0;
    while(prdata[i++] != ',');                                      //  Ѱ��ʱ��
    
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
    
    //--------------------------γ�Ȳ���----------------------------
    i++;
    x = 0;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
    if (x > 5)
    {
        //  ����γ��
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
    i++;                                                                //  ��ʾγ��
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

    //--------------------------���Ȳ���----------------------------    
    x = 0;
    i = i + 2;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }
    
    if (x > 5)
    {
        //  ���㾭��
        abc  = (str[0] - '0')*100 + (str[1] - '0')*10 +(str[2] - '0');
        de   = (str[3] - '0')*10 + (str[4] - '0');
        fghi = (str[6] - '0')*1000 + (str[7] - '0')*100 + (str[8] - '0')*10 + (str[9] - '0');
        longitude = abc + (de/60) + (fghi/600000.0);
    }
    else
    {
        longitude = 0;
    }
    
      

    //  ��ʾ����
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

    //--------------------------��λ����----------------------------    
    i = i + 2;                                                          //  ��ʾ��λ��Чֵ��0/1
    if(prdata[i++] == '0')
    {   
//	     printf("No location\r\n");              //  δ��λ
    }
    else
    {   
//	     printf("Location\r\n");                 //  ��λ
    }
    
    //--------------------------������Ŀ----------------------------    
    x = 0;
    i = i + 1;
    while(prdata[i] != ',')
    {
        str[x++] = prdata[i++];
    }

    
    Disbuf[0] = (str[0]- '0')*10 + str[1]- '0';
    
//	printf("������  %d\r\n", Disbuf[0]);
}

/*********************************************************************************************************
��������: void GPGSV_info(unsigned char *prdata)
����˵������ʾ��Ϣ
�������: ���ջ�����
�������: 
*********************************************************************************************************/
void GSV_info(unsigned char *prdata)
{
	unsigned char i, j, x;
    unsigned char str[20];
	
    //-------------------------�������-----------------------------
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
	
    //--------------------------��ǰ�����----------------------------
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
	
	
    //------------------------�ɼ���������--------------------------------------
    i++;                                                                //  ��ʾγ��
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
	//  ���Ǳ��SVID
	//  ����ele 		0-90  ��
	//  ��λ��az 		0-359 ��
	//  �����cn0 		0-99  dB-Hz
	
	//  ���Ǳ��
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
	
	//  ����ele
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
	
	//  ��λ��az
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
	
	//  �����cn0
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
			printf("�������:%d �ɼ�GPS��������:%d\r\n\r\n", gpgsv.numMsg, gpgsv.numSv);
		}
		else if (memcmp(prdata, "$BDGSV", 6) == 0)
		{
			printf("�������:%d �ɼ�������������:%d\r\n\r\n", gpgsv.numMsg, gpgsv.numSv);
		}
		printf("\t���Ǳ��\t����\t��λ��\t�����\r\n");
	}
    
	printf("%d\t%d\t\t%d\t%d\t%d\r\n",gpgsv.msgNo,gpgsv.SVID, gpgsv.ele,gpgsv.az, gpgsv.cn0);
	
	if (gpgsv.msgNo == gpgsv.numMsg)
		printf("-----------------------------------------\r\n\r\n");
}

