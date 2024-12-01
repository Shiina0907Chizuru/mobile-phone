#include "mp3.h"
#include "USART3.h"

void setMp3Dev(unsigned char dev)
{
    unsigned char device[5]  = {0x7E, 0x03, 0x09, 0x00, 0xEF};      // 指定设备 0：U 4:FLASH
    device[3] = dev;
    USART6_Senddata(device, 5);
}


void setMp3Vol(unsigned char vol)
{
    unsigned char volume[5]  = {0x7E, 0x03, 0x31, 0x00, 0xEF};      //	音量 0-30
    volume[3] = vol;
    USART6_Senddata(volume, 5);
}

void mp3Play(void)
{
    unsigned char Play[4]  = {0x7E, 0x02, 0x01, 0xEF};                  //	播放
    USART6_Senddata(Play, 4); 
}

void mp3Stop(void)
{
    unsigned char Stop[4]  = {0x7E, 0x02, 0x02, 0xEF};                  //	停止
    USART6_Senddata(Stop, 4); 
}
void mp3next(void)
{
    unsigned char Play[4]  = {0x7E, 0x02, 0x03, 0xEF};                  //	下一曲
    USART6_Senddata(Play, 4); 
}
void mp3last(void)
{
    unsigned char Play[4]  = {0x7E, 0x02, 0x04, 0xEF};                  //	上一曲
    USART6_Senddata(Play, 4); 
}
void mp3voiceadd(void)
{
    unsigned char Play[4]  = {0x7E, 0x02, 0x05, 0xEF};                  //	音量加
    USART6_Senddata(Play, 4); 
}
void mp3voicedet(void)
{
    unsigned char Play[4]  = {0x7E, 0x02, 0x06, 0xEF};                  //	音量减  
    USART6_Senddata(Play, 4); 
}
void mp3loopplay(void)
{
    unsigned char Play[5]  = {0x7E, 0x03,  0x07,0x01, 0xEF};                  //	音量减  
    USART6_Senddata(Play, 5); 
}
void mp3loopstop(void)
{
    unsigned char Play[5]  = {0x7E, 0x03,  0x07,0x00, 0xEF};                  //	音量减  
    USART6_Senddata(Play, 5); 
}
