/****************************************Copyright (c)***************************************
**-------------------------------------------------------------------------------------------
** Created by:          qinyx
** Last modified Date:  2014-02-28
** Last Version:        V1.00
** Descriptions:        STM32F407Ƕ��ʽʵ����
**-------------------------------------------------------------------------------------------*/
//********************************************************************************************/
#include "stm32f4xx.h"
#include "USART.h"
#include "delay.h"
#include "LED.h"
#include "ADC.h"
#include "DAC.h"
#include "KEY.h"
#include "CH455I2C.h"
#include "bsp_as608.h"
#include "as608_test.h"
#include "LCD.h"
#include "w25qxx.h" 
#include "spi.h"
#include "mp3.h"
#include "touch.h"
// #include "Servo.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

// u8 flag = 0;
u8 num_a=0;//��λ������ָ��
const u8 BCD_decode_tab[0x10] = { 0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F, 
									 0X77, 0X7C, 0X58, 0X5E, 0X79, 0X71 };//BCD
u8 ch455_key;//CH455�İ�����ֵ
	
int main(void)
{

  SysTick_Init();										//  ϵͳ�δ�ʱ����ʼ��
	LEDGpio_Init();
	KEYGpio_Init();
  W25QXX_Init();
	UART3_Configuration();
  volatile unsigned short temp = 0; 
  temp = W25QXX_ReadID();
  printf("SPI test\r\n");
  // Servo_init();
	UART6_Configuration(9600);
  // W25Q16:0XEF14 XT25F32B:0X0B15
  
  if(temp == 0XEF14 || temp == 0X0B15)
  {
      LED8_ON;
  }
//  W25QXX_TEST();
//    Adc_Init();
//    Dac1_Init();	
      /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
//    Debug_USART_Config();
  
  
    /*��ʼ�����λ�����*/
    rx_queue_init();
//   
//    /*��ʼ��ָ��ģ������*/
    AS608_Config();
//  
//	/*����STM32��ָ��ģ���ͨ��*/
    Connect_Test();
    LCD_Init();
    LCD_Clear(WHITE);
    tp_dev.init(); 
    lv_init();                             // LVGL ��ʼ��
    lv_port_disp_init();                   // ע��LVGL����ʾ����
    lv_port_indev_init();                  // ע��LVGL�Ĵ����������
    //  // ��ť
    // lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // ������ť; �����󣺵�ǰ���Ļ
    // lv_obj_set_pos(myBtn, 10, 10);                                               // ��������
    // lv_obj_set_size(myBtn, 120, 50);                                             // ���ô�С
   
    // // ��ť�ϵ��ı�
    // lv_obj_t *label_btn = lv_label_create(myBtn);                                // �����ı���ǩ�������������btn��ť
    // lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // �����ڣ�������
    // lv_label_set_text(label_btn, "Test");                                        // ���ñ�ǩ���ı�
 
    // // �����ı�ǩ
    // lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // �����ı���ǩ; �����󣺵�ǰ���Ļ
    // lv_label_set_text(myLabel, "Hello world!");                                  // ���ñ�ǩ���ı�
    // lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // �����ڣ�������
    // lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // �����ڣ�ĳ����
  
  unsigned char vol = 0;
    //���ſ�������ͬʱִ�п��Ŷ��ת�������������λ��������
    // OpenDoor(vol);
  while(1)
  {
//      if (Uart3.ReceiveFinish)
//        {
            FR_Task();//ָ��ģ�����
//            printf("�ɹ������ָ��Ϊ<|pad|>d",num_a);//num_aͨ��������������
//            printf("\r\n");
//            Uart3.ReceiveFinish = 0;
//        }
//      printf("%d",num_a);
//      delay_ms(500);
       
      tp_dev.scan(0);
				delay_ms(1);

  //       static uint8_t msLVGL = 0;   // ���ڿ���LVGL�����ִ��Ƶ�� = 0;  
  //       if (msLVGL++ == 5) 
  //       {
  //           lv_timer_handler(); // LVGL����
  //           msLVGL = 0;
  //       }
  } 
    

}