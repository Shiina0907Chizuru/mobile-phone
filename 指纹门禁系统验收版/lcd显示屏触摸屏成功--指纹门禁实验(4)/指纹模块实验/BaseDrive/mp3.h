#ifndef _MP3_H_
#define _MP3_H_


#define UDISK   0

extern unsigned char display_flag;                                      //  显示标志位
extern unsigned char databuf[20];                                       //  数据缓冲区
extern unsigned char flag;
void setMp3Dev();
void setMp3Vol(unsigned char vol);
void dinggequ(void);
void mp3Play(void);
void mp3Stop(void);
void Servo_init(void);
void OpenDoor(unsigned char vol);
#endif
