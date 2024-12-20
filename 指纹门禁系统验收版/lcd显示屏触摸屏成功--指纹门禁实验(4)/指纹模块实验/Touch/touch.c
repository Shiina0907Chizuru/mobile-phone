#include "touch.h"
#include "lcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
//#include "24cxx.h"

_m_tp_dev tp_dev =
{
    TP_Init,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};



//��������ʼ��
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 TP_Init(void)
{
    if(lcddev.id == 0X5510)		//���ݴ�����
    {
        if(GT9147_Init() == 0)	//��GT9147
        {
            tp_dev.scan = GT9147_Scan;	//ɨ�躯��ָ��GT9147������ɨ��
        }

        tp_dev.touchtype |= 0X80;	//������
        tp_dev.touchtype |= lcddev.dir & 0X01; //������������
        return 0;
    }

    return 1;
}

