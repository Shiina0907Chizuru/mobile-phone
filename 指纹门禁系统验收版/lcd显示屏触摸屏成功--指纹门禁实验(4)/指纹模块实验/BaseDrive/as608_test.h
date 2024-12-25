#ifndef __AS608_TEST_H
#define	__AS608_TEST_H


#include "bsp_as608.h"


void Connect_Test(void);
void AS608_USART_Config(void);
uint16_t PS_Connect(uint32_t *PS_Addr);
uint16_t PS_GetImage(void);
void Add_FR(void);
void Compare_FR(void);
void Del_FR(void);
void Clean_FR(void);

void Show_Message(void);
void FR_Task(void);
 unsigned char pipei2(void);
void code_lock_lvgl(void);
void create_password_lock_screen(void);
#endif /* __AS608_TEST_H */
