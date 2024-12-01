#ifndef _MP3_H_
#define _MP3_H_


#define UDISK   0
#define FLASH0   4
void setMp3Dev(unsigned char dev);
void setMp3Vol(unsigned char vol);
void mp3Play(void);
void mp3Stop(void);
void mp3next(void);
void mp3last(void);
void mp3voiceadd(void);
void mp3voicedet(void);
void mp3loopstop(void);
void mp3loopplay(void);
#endif
