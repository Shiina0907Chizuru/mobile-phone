/**
  ******************************************************************************
  * @file    bsp_as608.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ָ��ʶ��ģ��ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103-�Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "as608_test.h"
#include "bsp_as608.h"
#include "bsp_debug_usart.h"
#include "LCD.h"
#include "touch.h"
#include "KEY.h"
#include "delay.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "LED.h"


static uint16_t passflag=0;
extern uint8_t TouchOut_int_flag;
extern u8 num_a;
unsigned char mode=1;					//��������־λ��������
unsigned char flag1=0,flag2=0,flag3=0,flag4=0,flag5=0,flag6=0,flag7=0,flag8=0,flag9=0,flag10=0,flag11=0,flag12=0;	//�������ı�־λ
unsigned char Password_count=1; 		//����λ��
unsigned char Password[3]={0,0,0};      //��Ű����������Ա��������
unsigned char Password1[3]={2,3,1};     //����Ա����
unsigned char x,y,a;  					//ƥ������ķ���ֵ
int flag0=0;                            //��һ�ν�����������ı�־λ
int flag00=0;                           //�����ɹ���־λ
void key_scan(void);//�����������Ա����
unsigned char pipei(void);//����Ա����ƥ��
void code_lock(void);//������
/****************************************************************************************************/
static void btn_light_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        // ���á����صơ�����
        LED6_REVERSE;
    }
}
void zhujiemian(void)	//��ӭ������ʾ
{
	LCD_Clear(WHITE);
    // LCD_ShowString(200,280, (u8 *)"ָ���Ž�ϵͳ",BLACK,WHITE);
    // LCD_ShowString(180,380, (u8 *)"1.����",BLACK,WHITE);
    // LCD_ShowString(280,380, (u8 *)"3.����",BLACK,WHITE);
   // ��ȡ��ǰ���Ļ
    lv_obj_t *scr = lv_scr_act();
    // �����Ҫ��������ǰ�����������ʹ�� lv_obj_clean(scr);
    lv_obj_clean(scr);

    // -------------------------
    // 0. ��ѡ��Ϊ��Ļ�򱳾�������ɫ����ʽ
    // -------------------------
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // -------------------------
    // 1. ������
    // -------------------------
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Fingerprint Access System");
    // �������������Ļ�����Կ���λ�ã�����Yƫ�� -50
    lv_obj_align(label_title, LV_ALIGN_CENTER, 0, -50);

    // ������Ӧ��һ��������ʽ��ʹ���ֱ��
    static lv_style_t style_title;
    lv_style_init(&style_title);
    // ʹ�����õ� Montserrat 28 �����壨�������������ã�
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    // ��������ɫ��Ҳ�ɼ� lv_style_set_text_color(...)
    lv_obj_add_style(label_title, &style_title, 0);

    // -------------------------
    // 2. ѡ��1��Unlock������ʾ��ɫ��
    // -------------------------
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Unlock");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -10);

    // ����������Ϊ��ɫ
    lv_obj_set_style_text_color(label_option1, lv_palette_main(LV_PALETTE_BLUE), 0);

    // lv_anim_t blink_anim;
    // lv_anim_init(&blink_anim);
    // lv_anim_set_var(&blink_anim, label_option1);  // ���ö�������
    // lv_anim_set_values(&blink_anim, LV_OPA_TRANSP, LV_OPA_COVER);  // ��͸������ȫ�ɼ�֮���л�
    // lv_anim_set_time(&blink_anim, 500);  // ÿ�α仯ʱ��Ϊ 500ms
    // lv_anim_set_playback_time(&blink_anim, 500);  // ����ʱ��ҲΪ 500ms
    // lv_anim_set_repeat_count(&blink_anim, LV_ANIM_REPEAT_INFINITE);  // ����ѭ��
    // lv_anim_set_exec_cb(&blink_anim, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);  // �޸�͸����
    // lv_anim_start(&blink_anim);
    // ����ʹ�� lv_color_blue():
    // lv_obj_set_style_text_color(label_option1, lv_color_blue(), 0);

    // -------------------------
    // 3. ѡ��2��Enroll Fingerprint
    // -------------------------
    // lv_obj_t *label_option2 = lv_label_create(scr);
    // lv_label_set_text(label_option2, "2. Enroll Fingerprint");
    // lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 20);

    // -------------------------
    // 4. ѡ��3��Settings
    // -------------------------
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Settings");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);

     // ------------------------------------------------------------------------
    // 6. ����һ����ť������������ʾ���� "Toggle Light"
    // ------------------------------------------------------------------------
    // 6.1. ������ť����
    lv_obj_t *btn_light = lv_btn_create(scr);
    // ����ť���뵽��Ļ���ģ�Y ƫ�� +90
    lv_obj_align(btn_light, LV_ALIGN_CENTER, 0, 90);

    // 6.2. �ڰ�ť�ϴ���һ����ǩ������ʾ����
    lv_obj_t *label_btn = lv_label_create(btn_light);
    lv_label_set_text(label_btn, "Toggle Light");
    // lv_obj_center(label_btn); // �ѱ�ǩ���е���ť��

    // 6.3. ����ť��ӵ���¼��ص�
    //      ���û������ťʱ������ toggle_light() ����/�ص�
    lv_obj_add_event_cb(btn_light, btn_light_event_cb, LV_EVENT_CLICKED, NULL);

    // ���������һ�������������޸�������ɫ��������ɫ��
    // ���ԶԸ���ǩ����Ļ���� style
    // ʾ�����ı�������ɫ -> lv_obj_set_style_text_color(label_title, lv_color_hex(0x000000), 0);
    // ʾ�����ı���Ļ����ɫ -> lv_obj_set_style_bg_color(scr, lv_color_white(), 0);	
}
/****************************************************************************************************/
void jiesuo(void)	//��������
{
	LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"������ʽ",BLACK,WHITE);
    // LCD_ShowString(180,380, (u8 *)"1.ָ��",BLACK,WHITE);
    // LCD_ShowString(230,380, (u8 *)"2.����",BLACK,WHITE);
    // LCD_ShowString(280,380, (u8 *)"3.����",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // ������Ļ����Ϊ��ɫ
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // ���⣺������ʽ
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Unlock Method");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // ѡ�� 1: ָ�ƽ���
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Fingerprint Unlock");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // ѡ�� 2: ����
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Password");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // ѡ�� 3: ���أ���ɫ���壩
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
/****************************************************************************************************/
void zhujiemian1(void)		//��ʾ����Ա���ý���
{
	LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"����Ա����",BLACK,WHITE);
    // LCD_ShowString(100,380, (u8 *)"1.�޸Ľ�������",BLACK,WHITE);
    // LCD_ShowString(230,380, (u8 *)"2.ָ�ƹ���",BLACK,WHITE);
    // LCD_ShowString(330,380, (u8 *)"3.�˳�",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // ������Ļ����Ϊ��ɫ
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // ���⣺����Ա����
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Admin Settings");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // ѡ�� 1: �޸Ľ�������
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Change Unlock Password");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // ѡ�� 2: ָ�ƹ���
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Fingerprint Management");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // ѡ�� 3: ���أ���ɫ���壩
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
/****************************************************************************************************/
void shanzhiwen(void)	//ɾָ�ƽ���
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"ɾ��ָ��",BLACK,WHITE);
    // LCD_ShowString(100,380, (u8 *)"1.ɾ������ָ��",BLACK,WHITE);
    // LCD_ShowString(230,380, (u8 *)"2.���ָ�ƿ�",BLACK,WHITE);
    // LCD_ShowString(340,380, (u8 *)"3.����",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // ������Ļ����Ϊ��ɫ
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // ���⣺ɾ��ָ��
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Delete Fingerprint");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // ѡ�� 1: ɾ������ָ��
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Delete Single Fingerprint");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // ѡ�� 2: ɾ������ָ��
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Delete All Fingerprints");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // ѡ�� 3: ���أ���ɫ���壩
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
/****************************************************************************************************/
void zhiwenguanli(void)		//ָ�ƹ���ϵͳ����
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"����ָ��",BLACK,WHITE);
    // LCD_ShowString(100,380, (u8 *)"1.���ָ��",BLACK,WHITE);
    // LCD_ShowString(200,380, (u8 *)"2.ɾ��ָ��",BLACK,WHITE);
    // LCD_ShowString(300,380, (u8 *)"3.����",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // ������Ļ����Ϊ��ɫ
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // ���⣺ָ�ƹ���
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Fingerprint Management");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // ѡ�� 1: ���ָ��
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Add Fingerprint");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // ѡ�� 2: ɾ��ָ��
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Delete Fingerprint");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // ѡ�� 3: ���أ���ɫ���壩
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
void shezhi(void)	//��ʾ�������Ա�������
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(200,280, (u8 *)"�������Ա����",BLACK,WHITE);
    // LCD_ShowString(120,380, (u8 *)"-",BLACK,WHITE);
    // LCD_ShowString(220,380, (u8 *)"-",BLACK,WHITE);
    // LCD_ShowString(320,380, (u8 *)"-",BLACK,WHITE);

    // 1. Get current active screen and clear it
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 2. Set background to white
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 3. Create a label for "Input Admin Password"
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Input Admin Password");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // 4. Create three dash labels (as placeholders)
    lv_obj_t *label_dash1 = lv_label_create(scr);
    lv_label_set_text(label_dash1, "-");
    lv_obj_align(label_dash1, LV_ALIGN_CENTER, -60, 40);

    lv_obj_t *label_dash2 = lv_label_create(scr);
    lv_label_set_text(label_dash2, "-");
    lv_obj_align(label_dash2, LV_ALIGN_CENTER, 0, 40);

    lv_obj_t *label_dash3 = lv_label_create(scr);
    lv_label_set_text(label_dash3, "-");
    lv_obj_align(label_dash3, LV_ALIGN_CENTER, 60, 40);
}
/****************************************************************************************************/
void xiugai(void)	//��ʾ�������Ա�������
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(200,280, (u8 *)"�����޸ĺ����Ա����",BLACK,WHITE);
    // LCD_ShowString(120,380, (u8 *)"-",BLACK,WHITE);
    // LCD_ShowString(220,380, (u8 *)"-",BLACK,WHITE);
    // LCD_ShowString(320,380, (u8 *)"-",BLACK,WHITE);
    // 1. ��ȡ��ǰ���Ļ�����
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 2. ���ñ�����ɫΪ��ɫ
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 3. ���������ñ��� �������޸ĺ����Ա���롱
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Input New Admin Password"); 
    // �����Ҫ���ģ������� "�����޸ĺ����Ա����"
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    // �����Ҫ�������壬��ʹ����ʽ
    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // 4. ���� 3 �������ߡ���ǩ������Ϊռλ��
    lv_obj_t *label_dash1 = lv_label_create(scr);
    lv_label_set_text(label_dash1, "-");
    lv_obj_align(label_dash1, LV_ALIGN_CENTER, -60, 40);

    lv_obj_t *label_dash2 = lv_label_create(scr);
    lv_label_set_text(label_dash2, "-");
    lv_obj_align(label_dash2, LV_ALIGN_CENTER, 0, 40);

    lv_obj_t *label_dash3 = lv_label_create(scr);
    lv_label_set_text(label_dash3, "-");
    lv_obj_align(label_dash3, LV_ALIGN_CENTER, 60, 40);
}
/****************************************************************************************************/

void Show_Message(void)     //�˳���������
{   
    // LCD_Clear(WHITE);
    // LCD_ShowString(220,380, (u8 *)"�˳�",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);  // �����Ļ

    // ������Ļ����Ϊ��ɫ
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // ����һ�� Label ��ʾ "Exit"
    lv_obj_t *label_exit = lv_label_create(scr);
    lv_label_set_text(label_exit, "Exit");
    lv_obj_set_style_text_color(label_exit, lv_palette_main(LV_PALETTE_RED), 0);  // ����Ϊ��ɫ����

    // ���� Label ʹ�� lv_font_montserrat_28 ����
    static lv_style_t style_exit;
    lv_style_init(&style_exit);
    lv_style_set_text_font(&style_exit, &lv_font_montserrat_28);  // ��������Ϊ 28
    lv_obj_add_style(label_exit, &style_exit, 0);

    // ��ʼλ������Ļ���
    lv_obj_set_pos(label_exit, -lv_obj_get_width(label_exit), lv_obj_get_height(scr) / 2);

    // ��ȡ��Ļ��ȣ����ڼ����յ�λ��
    int32_t screen_center_x = (lv_obj_get_width(scr) - lv_obj_get_width(label_exit)) / 2;

    // ������������
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, label_exit);
    lv_anim_set_values(&anim, -lv_obj_get_width(label_exit), screen_center_x);  // ���󻬶�����Ļ�м�
    lv_anim_set_time(&anim, 1500);  // ����ʱ�� 1500ms
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_x);  // �޸� X ����
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);  // ƽ����������
    lv_anim_start(&anim);

    // // �������붯����������Ϊ������Ч��
    // lv_anim_t fade;
    // lv_anim_init(&fade);
    // lv_anim_set_var(&fade, label_exit);
    // lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);  // ��͸������ȫ�ɼ�
    // lv_anim_set_time(&fade, 1000);  // ���ε���ʱ�� 1000ms
    // lv_anim_set_playback_time(&fade, 1000);  // ��Ӳ��Ż���ʱ�䣬�����ȴ���ȫ�ɼ��ص�͸��
    // // lv_anim_set_repeat_count(&fade, LV_ANIM_REPEAT_INFINITE);  // ����ѭ��
    // lv_anim_set_exec_cb(&fade, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    // lv_anim_start(&fade);
}



/**
  * @brief  ָ��ģ�����
  * @param  ��
  * @retval ��
  */
void  FR_Task(void)
{	
    uint32_t   ch;
    unsigned char key=0;			//���建���ֵ
	key = KeyScan();
	switch(mode)
	{
//��ʾ��ӭ����------------------------------------------------------------------------------------/			
            case 1:		
				if(flag1==0)				//ֻ����һ��
				{
					zhujiemian();
					flag1=1;					
				}
				if(key==1)mode=2,flag2=1;//��ѡ�������ʽ����
				if(key==3)mode=3,flag3=1;//������Ա��������
				break; 
//��ʾѡ�������ʽ------------------------------------------------------------------------------------/				
			case 2:		
				if(flag2==1)
				{
					jiesuo();					
					flag2=0;
				}
				if(key==1) mode=4,flag4=1;	//��ȥָ�ƽ���
				if(key==2) mode=5,flag5=1;	//��ȥ�������
                if(key==3) mode=1,flag1=0;	//������һ��
				break;
//��ʾ�������Ա�������------------------------------------------------------------------------------/					
			case 3:		
				if(flag3==1)
				{
					shezhi();				
					flag3=0;
                    flag0=0;//�����һ�δ���һ�������������������棨��ֹkey3������
				}
                delay_ms(10);
//                if(key==3)  key=0;          //
                read_password();
				key_scan();					//��������
				if(Password_count==4)		//�����������λ����λ��
				{
					delay_ms(100);			//�ӳ�һ��
					x=pipei();      		 //����ֵ1:������ȷ
					if(x==1) 
					{
						Password_count=1;	//����λ���㷽���´���������
						mode=6;				//������ȷ ��ת������Ա���ý���
						flag6=1;
					}
					else 
					{
						Password_count=1;		//�����������,������������
						flag3=1;			//��ʾ����ˢһ��
					}
				}
				break;
//��ʾˢָ��--------------------------------------------------------------------------------------/				
			case 4:
				if(flag4==1)
				{
                    Compare_FR();					//�Ա�ָ��
                    Show_Message();
					flag4=0;

				}
                if(flag00==1)   {
                    OpenDoor(30);
                    flag00=0;
                }
				if(key==1) mode=1,flag1=0;	//�˳�
				break;
//��ʾ��������������-------------------------------------------------------------------------------/					
			case 5:	
				if(flag5==1)
				{
					create_password_lock_screen();
                    	
					flag5=0;					
					
				}   
                if(passflag==1){
                    OpenDoor(30);
                    mode=1,flag1=0;	//�˻�������
                }
                
				break;
//��ʾ����Ա���ý���-----------------------------------------------------------------------------------/					
			case 6:	
				if(flag6==1)
				{
					flag6=0;
					zhujiemian1();					
				}
				if(key==1) mode=7,flag7=1;	//��ȥ�޸Ľ����������
				if(key==2) mode=8,flag8=1;	//��ȥָ�ƹ������
				if(key==3) mode=1,flag1=0;	//�˻�������			
				break;
//�޸Ľ����������-----------------------------------------------------------------------------------/	
			case 7:		
				if(flag7==1)
				{
					flag7=0;
                    xiugai();
                    flag0=0;//�����һ�δ���һ�������������������棨��ֹkey3������
				}
                delay_ms(10);
                read_password();
//                if(key==3)  key=0;          //
				key_scan();					//��������
				if(Password_count==4)		//�����������λ����λ��
				{
					delay_ms(100);			//�ӳ�һ��
					x=pipei2();      		 //����ֵ1:�������ʼ����һ��
					if(x==1) 
					{
						Password_count=1;	//����λ���㷽���´���������
                        LCD_ShowString(200,330, (u8 *)"���ϴ���������һ�£�����������",RED,WHITE);   
                        delay_ms(1000);
                        flag7=1;			//��ʾ����ˢһ��
					}
					else 
					{
                        //������������븳������Ա��������
                        for(int i=0;i<3;i++){
                            Password1[i]=Password[i];
                        }
                        //��������д��flash����
                        save_password();
						Password_count=1;		//�����������,������������
						mode=6;				//������ȷ ��ת������Ա���ý���
						flag6=1;
					}
				}

//					if(a==1) 
//					{
//						Password_count=1;	//����λ���㷽���´���������
//						mode=1;				//�����޸ĳɹ�   ��ת�����������
//						flag1=0;
//					}
//					else 					//����һ����������
//					{
//						Password_count=1;		//�����������,������������
//						flag7=1;			//��������������ˢһ��
//					}
				break;                
//ָ�ƹ������---------------------------------------------------------------------------------------/	
			case 8:
				if(flag8==1)
				{
					zhiwenguanli();
					flag8=0;					
				}
				if(key==1) mode=9,flag9=1;		//��ȥ���ָ�ƽ���
				if(key==2) mode=10,flag10=1;		//��ȥɾ��ָ�ƽ���
				if(key==3) mode=6,flag6=1;		//����
				break;	
//¼ָ�ƽ���---------------------------------------------------------------------------------------/				
			case 9:
				if(flag9==1)
				{
					Add_FR();         //���ָ��
                    Show_Message();			
					flag9=0;
				}
				if(key==1) mode=1,flag1=0;	//�˳�
				break;
////ɾ��ָ�ƽ���---------------------------------------------------------------------------------------/				
			case 10:
				if(flag10==1)
				{
					shanzhiwen();
					flag10=0;	
				}
				if(key==1) mode=11,flag11=1;		//��ȥɾ������ָ�ƽ���
				if(key==2) mode=12,flag12=1;		//��ȥ���ָ�ƿ�
				if(key==3) mode=6,flag6=1;		//����
				break;
////ɾ������ָ��------------------------------------------------------------------------------/				
			case 11:
				if(flag11==1)
				{
					Del_FR();
                    Show_Message();
					flag11=0;
				}
                if(key==1) mode=1,flag1=0;	//�˳�
				break;
//���ָ�ƿ�----------------------------------------------------------------------------/				
			case 12:
				if(flag12==1)		
				{
					Clean_FR();
                    Show_Message();
					flag12=0;
				}
                if(key==1) mode=1,flag1=0;	//�˳�
				break;        
    }
    return;		
}


/**
  * @brief  �����ָ��ģ���ͨ��
  * @param  ��
  * @retval ��
  */
void  Connect_Test(void)
{   	
  if(PS_Connect(&AS608_Addr))                      /*��AS608����ͨ��*/
  { 
    AS608_INFO("δ��⵽ָ��ģ����������\r\n");			  
  }
  else
  {
	AS608_INFO("ͨѶ�ɹ�\r\n");
	Show_Message();
  }
}


/**
  * @brief  ¼ָ��
  * @param  ��
  * @retval ��
  */
void  Add_FR(void)
{
  uint16_t  i,j,sure,ID;
  char IDS[5];
    
  i=j=0;
	
  while(1)
  {
    switch (j)
    {
      case 0:                            /*ִ�е�1��*/
        
        i++;
        LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"******����밴��ָ******",BLACK,WHITE);
//        AS608_INFO("\r\n");
        
        sure=PS_GetImage();              /*¼��ͼ��*/      
        if(sure == 0x00)
        {
          sure=PS_GenChar(CHAR_BUFFER1);  /*��������1*/
          if(sure==0x00)
          { 
              LCD_Clear(WHITE);
              LCD_ShowString(100,280, (u8 *)"����ָ��1����!!!",BLACK,WHITE);
//              AS608_INFO("\r\n");
            
               sure=PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
            if(sure==0x00)
            {
             sprintf(IDS,"%d",ID);
                LCD_Clear(WHITE);
             LCD_ShowString(100,280, (u8 *)"ָ���Ѵ���,ָ��ID:",BLACK,WHITE);
             LCD_ShowString(300,280, (u8 *)IDS ,BLACK,WHITE);  
             delay_ms(2000);          
//             AS608_INFO ("%d������\r\n\r\n",ID);             
             return ;
            }              
            else 
            {
              i=0;
              j=1;                   /*��ת����2��*/
             }
           }
				 else
				 {
					 ShowErrMessage(sure);	       
				 }
        }
			  else 
			  {
				  ShowErrMessage(sure);	
			  }
      break;


      case 1:
        
        i++;
        LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"******������ٰ�һ����ָ******",BLACK,WHITE);
//        AS608_INFO("\r\n");
      
        sure=PS_GetImage();
        if(sure==0x00)
        { 
          sure=PS_GenChar(CHAR_BUFFER2);  /*��������2*/
          if(sure==0x00)
          { 
			LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"����ָ��2����!!!",BLACK,WHITE);    
//              AS608_INFO("\r\n");
            i=0;
            j=2;                   /*��ת����3��*/
          }
				  else 
				  {
					  ShowErrMessage(sure);
				  }

         }
			   else 
			   {
			   ShowErrMessage(sure);
         }				
       break;

			
       case 2:
         LCD_Clear(WHITE);
         LCD_ShowString(100,280, (u8 *)"���ڶԱ����������ָ��",BLACK,WHITE);
//         AS608_INFO("\r\n");
       
         sure=PS_Match();                /*��ȷ�ȶ���öָ������*/
         if(sure==0x00) 
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"����ָ�ƶԱȳɹ�������",BLACK,WHITE);
//				   AS608_INFO("\r\n");
           
           j=3;                         /*��ת����4��*/
         }
         else
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"�Ա�ʧ�ܣ�������¼��ָ�ƣ�����",BLACK,WHITE);
//           AS608_INFO("\r\n");
           ShowErrMessage(sure);
           i=0;
           j=0;
         }
        
       break;
         
         
       case 3:
         LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"��������ָ��ģ��",BLACK,WHITE);
//         AS608_INFO("\r\n");
       
         sure=PS_RegModel();            /*�ϲ�����������ģ�壩*/
         if(sure==0x00) 	
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"����ָ��ģ��ɹ�������",BLACK,WHITE);
//           AS608_INFO("\r\n");
           j=4;                        /*��ת����5��*/
         } 
         else 
        {
         j =0;
         ShowErrMessage(sure);
        }
        
       break;
        

       case 4:
        
         do
         { 
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"***���������洢ID����ΧΪ0~239***",BLACK,WHITE);
            delay_ms(2000);
//           AS608_INFO ("\r\n");
        
           ID=GET_NUM();
         }while(!(ID<PS_MAXNUM));
        
         sure=PS_StoreChar(CHAR_BUFFER2,ID);  /*����ģ��*/
         if(sure==0x00) 	
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"¼��ָ��ģ��ɹ�������",BLACK,WHITE);
            delay_ms(2000); 
//           AS608_INFO("\r\n\r\n");      
           return ;
         } 
         else 
         {
           j =0;
           ShowErrMessage(sure);
         }
       break;
      }
    
      AS608_DELAY_MS(1000);
      
      if(i==4)                       /*����4��û�а���ָ���˳�*/
      { 
          LCD_Clear(WHITE);
          LCD_ShowString(100,280, (u8 *)"¼ָ��ʧ�ܣ�",BLACK,WHITE);
//        AS608_INFO("\r\n\r\n\r\n");
        break;
      }
  }
}


/**
  * @brief  �ȶ�ָ�ƣ�ʶ��ָ�ƣ�
  * @param  ��
  * @retval ��
  */
void Compare_FR(void)  
{  
  uint16_t  ID=0;                    /*��ʼ��IDֵ*/  
  uint16_t  sure;
  char IDS[5];  
  
  LCD_Clear(WHITE);
  LCD_ShowString(100,280, (u8 *)"******����Ա�ָ�ƣ��밴��ָ******",BLACK,WHITE);
//  AS608_INFO("******����Ա�ָ�ƣ��밴��ָ******\r\n");

  sure=PS_GetImage();
  if(sure == 0x00)
  {
    sure=PS_GenChar(CHAR_BUFFER1);    /*��������1*/
    if(sure==0x00)
    {  
			/*��������ָ�ƿ�*/
      sure=PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
      if(sure==0x00)
      {
        sprintf(IDS,"%d",ID);
        LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"�Ա�ָ�Ƴɹ���ָ��ID��",BLACK,WHITE); 
        LCD_ShowString(300,280, (u8 *)IDS,BLACK,WHITE);
        flag00=1;
        delay_ms(2000);
//        AS608_INFO ("�Ա�ָ�Ƴɹ���ָ��ID��%d������\r\n\r\n",ID);
      }
      else 
      {
			ShowErrMessage(sure);
			}
    }
    else 
    {   
      ShowErrMessage(sure);      
    }
  }
	else  
	{
	  ShowErrMessage(sure);
	}
    
}



/**
  * @brief  ���ָ�ƿ�
  * @param  ��
  * @retval ��
  */
void Clean_FR(void) 
{
  uint16_t  sure;
    LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"���ָ�ƿ�",BLACK,WHITE);
//	AS608_INFO("\r\n");
  
	sure=PS_Empty();     
	if(sure == 0x00)
	{
        LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"���ָ�ƿ�ɹ�������",BLACK,WHITE);
        delay_ms(2000);
	}
	else 
	{
		ShowErrMessage(sure);
	}

}


/**
  * @brief  ɾ��ָ���û�ָ��
  * @param  ��
  * @retval ��
  */
void Del_FR(void)
{ 
	uint32_t  ID;
	uint16_t  sure;
    LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"ɾ��ָ���û�ID",BLACK,WHITE);
//	AS608_INFO("ɾ��ָ���û�ID\r\n");
  
	do
	{ 
		LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"***���������洢ID����ΧΪ0~239***",BLACK,WHITE);
//        AS608_INFO ("***���������洢ID����ΧΪ0~239***\r\n");
        delay_ms(2000);
		ID=GET_NUM();
	}while(!(ID<PS_MAXNUM));         /*����ID���ܳ���ָ��ģ���������*/
	
	sure=PS_DeletChar(ID,1);         /*ɾ��ָ���û���ָ��ģ��*/
	
	if(sure == 0x00)                
	{
		LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"ɾ��ָ���û�ָ�Ƴɹ�������",BLACK,WHITE);
        delay_ms(3000);
//        AS608_INFO("\r\n\r\n");
	}
	else 
	{
		ShowErrMessage(sure);
	}
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */

#define MAX_PASS_LEN  8                  /* �������λ�� */
static const char * correct_pass = "1234";  /* ��ȷ���룬�ɸ�����Ҫ�޸� */

/* ���ڴ洢�û����� */
static char input_buffer[MAX_PASS_LEN + 1] = {0};
static uint8_t input_len = 0;

/* ������ʾ�������ʾ��Ϣ�Ŀؼ� */
static lv_obj_t * label_input;
static lv_obj_t * label_hint;

/*
 * ��ť�����ı�ӳ��
 * - "Del" ɾ��
 * - "OK"  ȷ��
 */
static const char * btnm_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "Del", "0", "OK", ""
};

static void add_char_to_input(const char c)
{
    if (input_len < MAX_PASS_LEN) {
        input_buffer[input_len] = c;
        input_len++;
        input_buffer[input_len] = '\0';  
    }
}

static void remove_char_from_input(void)
{
    if (input_len > 0) {
        input_len--;
        input_buffer[input_len] = '\0';
    }
}

static void update_input_label(void)
{
    lv_label_set_text(label_input, input_buffer);
}

static void check_password(void)
{
    if (strcmp(input_buffer, correct_pass) == 0) {
        lv_label_set_text(label_hint, "Unlock successful!");
        passflag=1;
        // TODO: ������ȷ��Ĳ���
    } else {
        lv_label_set_text(label_hint, "Incorrect password, please try again!");
    }

    memset(input_buffer, 0, sizeof(input_buffer));
    input_len = 0;
    update_input_label();
}

/* ��ť�����¼��ص� */
static void btnm_event_cb(lv_event_t * e)
{
    lv_obj_t * btnm = lv_event_get_target(e);
    const char * txt = lv_btnmatrix_get_btn_text(btnm, lv_btnmatrix_get_selected_btn(btnm));

    if (txt == NULL) return;

    if (strcmp(txt, "Del") == 0) {
        remove_char_from_input();
        update_input_label();
    }
    else if (strcmp(txt, "OK") == 0) {
        check_password();
    }
    else {
        add_char_to_input(txt[0]);  
        update_input_label();
    }
}

void create_password_lock_screen(void)
{
    /* 1. ����һ���µ���Ļ���� */
    lv_obj_t * scr = lv_obj_create(NULL);

    /* 2. ���������ǩ */
    lv_obj_t * label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Please enter password:");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    /* 3. ������ʾ��ǰ����ı�ǩ */
    label_input = lv_label_create(scr);
    lv_label_set_text(label_input, "");
    lv_obj_set_style_text_font(label_input, &lv_font_montserrat_28, 0);  
    lv_obj_align(label_input, LV_ALIGN_TOP_MID, 0, 60);

    /* 4. ������ʾ��ʾ/����ı�ǩ */
    label_hint = lv_label_create(scr);
    lv_label_set_text(label_hint, "Awaiting input...");
    lv_obj_align(label_hint, LV_ALIGN_TOP_MID, 0, 100);

    /* 5. ������ť�����������֡�ɾ����ȷ�ϣ� */
    lv_obj_t * btnm = lv_btnmatrix_create(scr);
    lv_btnmatrix_set_map(btnm, btnm_map);
    lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 40);

    /* ---------- �ؼ����֣��ð�ť�ߴ��� ---------- */
    lv_obj_set_style_pad_all(btnm, 10, 0);       /* �������ܱ߾� */
    lv_obj_set_style_pad_row(btnm, 10, 0);       /* �м�� */
    lv_obj_set_style_pad_column(btnm, 10, 0);    /* �м�� */
    lv_obj_set_style_width(btnm, 80, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_height(btnm, 60, LV_PART_ITEMS | LV_STATE_DEFAULT);

    /* �����Ҫ��������֣��������ò����ô����� */
    // lv_obj_set_style_text_font(btnm, &lv_font_montserrat_28, LV_PART_ITEMS | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 6. �л���������Ļ */
    lv_scr_load(scr);
}



void code_lock(void)
{
    u8 t = 0;
    u8 i = 0;
    int j =11;
    
    u16 Password[4] = {0,0,0,0}; //����������������
    u16 CorrectPass[4] = {0,3,1,9}; //��ȷ������
    int index = 0;
    u8 full_flag = 0;
    // �����־λ
    u8 confirm_flag = 0;
    u16 lastpos[5][2];		//���һ�ε�����

//    delay_init(168);			//��ʱ��ʼ��
    tp_dev.init();				//��������ʼ��
    LCD_Clear(WHITE);
    LCD_ShowString(135, 155, (u8 *)"����ǰ�Ȱ���ȷ�ϼ�",BLACK,WHITE);
    LCD_DrawRectangle(325,675,415,765,BLACK);
    // LCD_DrawRectangle(195,675,285,765,BLACK);//0
    LCD_DrawRectangle(195,675,285,765,BLACK);//0
    LCD_DrawRectangle(65,285,155,375,BLACK);//1
    LCD_DrawRectangle(195,285,285,375,BLACK);//2
    LCD_DrawRectangle(325,285,415,375,BLACK);//3
    LCD_DrawRectangle(65,415,155,505,BLACK);//4
    LCD_DrawRectangle(195,415,285,505,BLACK);//5
    LCD_DrawRectangle(325,415,415,505,BLACK);//6
    LCD_DrawRectangle(65,545,155,635,BLACK);//7
    LCD_DrawRectangle(195,545,285,635,BLACK);//8
    LCD_DrawRectangle(325,545,415,635,BLACK);//9
    LCD_DrawRectangle(65,675,155,765,BLACK);//ȷ��
    LCD_ShowString(360, 710, (u8 *)"ɾ��",BLACK,WHITE);
    LCD_ShowString(235, 710, (u8 *)"0",BLACK,WHITE);
    LCD_ShowString(105, 320, (u8 *)"1",BLACK,WHITE);
    LCD_ShowString(235, 320, (u8 *)"2",BLACK,WHITE);
    LCD_ShowString(365, 320, (u8 *)"3",BLACK,WHITE);
    LCD_ShowString(105, 450, (u8 *)"4",BLACK,WHITE);
    LCD_ShowString(235, 450, (u8 *)"5",BLACK,WHITE);
    LCD_ShowString(365, 450, (u8 *)"6",BLACK,WHITE);
    LCD_ShowString(105, 580, (u8 *)"7",BLACK,WHITE);
    LCD_ShowString(235, 580, (u8 *)"8",BLACK,WHITE);
    LCD_ShowString(365, 580, (u8 *)"9",BLACK,WHITE);
    LCD_ShowString(105, 710, (u8 *)"ȷ��",BLACK,WHITE);
    while(1)
    {
        tp_dev.scan(0);

        for(t = 0; t < 5; t++)
        {
            if((tp_dev.sta) & (1 << t))
            {
                if(tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)
                {
                    if(lastpos[t][0] == 0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }
                }
            }
            //ȷ��
            if((tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>675 && tp_dev.y[t]<765)&&(confirm_flag==0)){
                confirm_flag=1;
            }
            //0
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>675 && tp_dev.y[t]<765)
            {
                //ȷ�ϼ�����
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 0;
                full_flag = 0;
                confirm_flag=0;//ȷ��
                //��������
                // while(confirm_flag==0){
                //     tp_dev.scan(0);
                //     if(!(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>675 && tp_dev.y[t]<765)) confirm_flag=1;
                //     delay_ms(50);
                // } 
                // confirm_flag=2;
                // while(GT9147_Scan(0) == 1) ;//������������ʱ��ס���ɿ�����
            }
            //1
            if(tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>285 && tp_dev.y[t]<375)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 1;
                full_flag = 0;
                confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //2
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>285 && tp_dev.y[t]<375)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 2;
                full_flag = 0;
                confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //3
            if(tp_dev.x[t]>325 &&tp_dev.x[t]<415 && tp_dev.y[t]>285 && tp_dev.y[t]<375)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 3;
                full_flag = 0;
                confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //4
            if(tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>415 && tp_dev.y[t]<505)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 4;
                full_flag = 0;
                confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan== 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //5
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>415 && tp_dev.y[t]<505)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 5;
                full_flag = 0;
                confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //6
            if(tp_dev.x[t]>325 && tp_dev.x[t]<415 && tp_dev.y[t]>415 && tp_dev.y[t]<505)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 6;
                full_flag = 0;
                confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //7
            if(tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>545 && tp_dev.y[t]<635)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 7;
                full_flag = 0;
                 confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //8
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>545 && tp_dev.y[t]<635)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 8;
                full_flag = 0;
                 confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //9
            if(tp_dev.x[t]>325 && tp_dev.x[t]<415 && tp_dev.y[t]>545 && tp_dev.y[t]<635)
            {
                if(confirm_flag==0) continue;//ȷ��
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 9;
                full_flag = 0;
                 confirm_flag=0;//ȷ��
                // while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
            //ɾ��
            if(tp_dev.x[t]>325 && tp_dev.x[t]<415 && tp_dev.y[t]>675 && tp_dev.y[t]<765)
            {
                index --;
                if(index < 0)
                    index = 0;
                Password[index] = 0;
                full_flag = 0;
                while((int)tp_dev.scan == 1) ;//������������ʱ��ס���ɿ�����
                delay_ms(50);
            }
        }
        if(index == 0)
        {
            if(full_flag == 0)
            {
                LCD_Fill(100,205,450,235,WHITE);
                full_flag = 1;
            }

            POINT_COLOR = BLACK;
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(180,220,j);
            }
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(220,220,j);
            }
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(260,220,j);
            }
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(300,220,j);
            }
        }
        else if(index == 1)
        {
            if(full_flag == 0)
            {
                LCD_Fill(100,205,450,235,WHITE);
                full_flag = 1;
            }
            POINT_COLOR = BLACK;
            gui_fill_circle(180,220,11,BLACK);
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(220,220,j);
            }
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(260,220,j);
            }
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(300,220,j);
            }
        }
        else if(index == 2)
        {
            if(full_flag == 0)
            {
                LCD_Fill(100,205,450,235,WHITE);
                full_flag = 1;
            }
            POINT_COLOR = BLACK;
            gui_fill_circle(180,220,11,BLACK);
            gui_fill_circle(220,220,11,BLACK);
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(260,220,j);
            }
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(300,220,j);
            }
        }
        else if(index == 3)
        {
            if(full_flag == 0)
            {
                LCD_Fill(100,205,450,235,WHITE);
                full_flag = 1;
            }
            POINT_COLOR = BLACK;
            gui_fill_circle(180,220,11,BLACK);
            gui_fill_circle(220,220,11,BLACK);
            gui_fill_circle(260,220,11,BLACK);
            for(j = 11; j >=9; j--)
            {
                LCD_Draw_Circle(300,220,j);
            }
        }
        else if(index == 4)
        {
            if(full_flag == 0)
            {
                LCD_Fill(100,205,450,235,WHITE);
                full_flag = 1;
            }
            POINT_COLOR = BLACK;
            gui_fill_circle(180,220,11,BLACK);
            gui_fill_circle(220,220,11,BLACK);
            gui_fill_circle(260,220,11,BLACK);
            gui_fill_circle(300,220,11,BLACK);
            if(Password[0]==CorrectPass[0] && Password[1]==CorrectPass[1] && Password[2]==CorrectPass[2] && Password[3]==CorrectPass[3])
            {
//                BEEP1();
                LCD_ShowString(220,120,(u8*)"�����ɹ�",BLACK,WHITE);
//                LED1 = !LED1;
                delay_ms(500);
                break;
            }
            else
            {
//                BEEP2();
                index = 0;
                Password[0] = 0;
                Password[1] = 0;
                Password[2] = 0;
                Password[3] = 0;
                full_flag = 0;
            }
        }
        delay_ms(30);
        i++;
    }
}
void show_num_with_dash_alignment(unsigned char index, const char* str)
{
    // ����һ���µ� label
    lv_obj_t *label_num = lv_label_create(lv_scr_act());
    lv_label_set_text(label_num, str);

    // ���� index ��������
    // �ο� dash1/dash2/dash3:
    // dash1: lv_obj_align(label_dash1, LV_ALIGN_CENTER, -60, 40);
    // dash2: lv_obj_align(label_dash2, LV_ALIGN_CENTER,  0, 40);
    // dash3: lv_obj_align(label_dash3, LV_ALIGN_CENTER, 60, 40);
    int x_ofs = 0;
    int y_ofs = 40; // same y offset as dash1/2/3

    switch (index) {
        case 1: x_ofs = -60; break; // first digit
        case 2: x_ofs = 0;    break; // second digit
        case 3: x_ofs = 60;  break; // third digit
        default: x_ofs = 100; // or ignore
    }

    lv_obj_align(label_num, LV_ALIGN_CENTER, x_ofs, y_ofs);

    // �����Ҫ���������ͬ��ɫ����������� style
    // static lv_style_t style_digits;
    // lv_style_init(&style_digits);
    // lv_style_set_text_font(&style_digits, &lv_font_montserrat_28);
    // lv_obj_add_style(label_num, &style_digits, 0);
}
/**
  * @brief  �����������Ա����
  * @param  ��
  * @retval ��
  */
void key_scan(void)	
{
	// unsigned char num=0;
    // int flag1=0,flag2=0,flag3=0;//��ֹ���������λ���룬�����־λ
	// unsigned int i=0;
    // char nums[4];
    // if((flag0==0)&&(KEY_S3_READ()!=0)){//��һ�ν�������������棬ֱ��k3�����ɿ���flag0��1������3��������
    //     flag0=1;
    // }
	// if((KEY_S3_READ()==0)&&(flag3==0)&&(flag0==1))
	// {        
	// 	delay_ms(50);
    //     flag3=0;
	// 		while(KEY_S3_READ()==0)
	// 		{
	// 			if(Password_count<=3)		//�������λ��С��3
	// 			{
    //                 num=3;
    //                 sprintf(nums,"%d",num);
    //                 LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
	// 				Password[Password_count-1]=num;			 //�����������������
	// 				if(KEY_S3_READ()!=0)							//�����ɿ� ������һλ����
	// 				{
	// 					Password_count++;
    //                     flag3=1;
	// 					break;
	// 				}
	// 			}		
	// 	}
	// }
    // if((KEY_S2_READ()==0)&&(flag2==0))
	// {
	// 	delay_ms(50);
    //     flag2=0;
	// 		while(KEY_S2_READ()==0)
	// 		{
	// 			if(Password_count<=3)		//�������λ��С��3
	// 			{
    //                 num=2;
    //                 sprintf(nums,"%d",num);
    //                 LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
	// 				Password[Password_count-1]=num;			 //�����������������
	// 				if(KEY_S2_READ()!=0)							//�����ɿ� ������һλ����
	// 				{
	// 					Password_count++;
    //                     flag2=0;                        
	// 					break;
	// 				}
	// 			}		
	// 	}
	// }
    // if((KEY_S1_READ()==0)&&(flag1==0))
	// {
	// 	delay_ms(50);
    //     flag1=1;
	// 		while(KEY_S1_READ()==0)
	// 		{
	// 			if(Password_count<=3)		//�������λ��С��3
	// 			{
    //                 num=1;
    //                 sprintf(nums,"%d",num);
    //                 LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
	// 				Password[Password_count-1]=num;			 //�����������������
	// 				if(KEY_S1_READ()!=0)							//�����ɿ� ������һλ����
	// 				{
	// 					Password_count++;
    //                     flag1=1;                        
	// 					break;
	// 				}
	// 			}		
	// 	}
	// }
    
      unsigned char num = 0;
    int flag1 = 0, flag2 = 0, flag3 = 0;  
    char nums[4];

    // ��������Ѿ���ȫ�ֱ���:
    // extern unsigned char Password[4];    // ������������
    // extern unsigned char Password_count; // ��ǰ�����˶���λ (1~4)
    // extern int flag0;
    // �Լ�������⺯�� KEY_S1_READ(), KEY_S2_READ(), KEY_S3_READ()
    // ��ʱ���� delay_ms()

    // ��� flag0==0, ���� S3 �Ѿ��ɿ�, ����������:
    if ((flag0 == 0) && (KEY_S3_READ() != 0))
    {
        flag0 = 1;
    }

    // ========== Key S3 ==========
    if ((KEY_S3_READ() == 0) && (flag3 == 0) && (flag0 == 1))
    {
        delay_ms(50);
        flag3 = 0;
        while (KEY_S3_READ() == 0)
        {
            // �����ǰ��û������ 3 λ(�� 4 λ)������������ 3 λ
            if (Password_count <= 3)
            {
                num = 3;
                sprintf(nums, "%d", num);

                // ��ʾ���ֵ�ָ��λ��
                show_num_with_dash_alignment(Password_count, nums);

                // ������������
                Password[Password_count - 1] = num;

                // �ɿ���������һλ
                if (KEY_S3_READ() != 0)
                {
                    Password_count++;
                    flag3 = 1;
                    break;
                }
            }
        }
    }

    // ========== Key S2 ==========
    if ((KEY_S2_READ() == 0) && (flag2 == 0))
    {
        delay_ms(50);
        flag2 = 0;
        while (KEY_S2_READ() == 0)
        {
            if (Password_count <= 3)
            {
                num = 2;
                sprintf(nums, "%d", num);

                show_num_with_dash_alignment(Password_count, nums);
                Password[Password_count - 1] = num;

                if (KEY_S2_READ() != 0)
                {
                    Password_count++;
                    flag2 = 0;
                    break;
                }
            }
        }
    }

    // ========== Key S1 ==========
    if ((KEY_S1_READ() == 0) && (flag1 == 0))
    {
        delay_ms(50);
        flag1 = 1;
        while (KEY_S1_READ() == 0)
        {
            if (Password_count <= 3)
            {
                num = 1;
                sprintf(nums, "%d", num);

                show_num_with_dash_alignment(Password_count, nums);
                Password[Password_count - 1] = num;

                if (KEY_S1_READ() != 0)
                {
                    Password_count++;
                    flag1 = 1;
                    break;
                }
            }
        }
    }
    
}
/**
  * @brief  ��������Ա����ƥ��
  * @param  ��
  * @retval ��
  */
unsigned char pipei(void)
{
	if((Password[0]==Password1[0]&&Password[1]==Password1[1]&&Password[2]==Password1[2]))
	{
        LCD_ShowString(200,180, (u8 *)"������ȷ������",BLACK,WHITE);
		delay_ms(1000);
		return 1;		//1������ȷ��0�������
	}
	else
	{
        LCD_ShowString(200,180, (u8 *)"������󣡣���",BLACK,WHITE);
		delay_ms(1000);
		return 0;
	}
}

/**
  * @brief  ��������Ա����ƥ��2
  * @param  ��
  * @retval ��
  */
unsigned char pipei2(void)
{
	// if((Password[0]==Password1[0]&&Password[1]==Password1[1]&&Password[2]==Password1[2]))
	// {
    //     LCD_ShowString(200,180, (u8 *)"���ϴ�������ͬ������",BLACK,WHITE);
	// 	delay_ms(1000);
	// 	return 1;		//1������ȷ��0�������
	// }
	// else
	// {
    //     LCD_ShowString(200,180, (u8 *)"�޸ĳɹ�������",BLACK,WHITE);
	// 	delay_ms(1000);
	// 	return 0;
	// }
    lv_obj_t *scr = lv_scr_act();  // ��ȡ��ǰ���Ļ
    lv_obj_clean(scr);            // �����Ļ

    if ((Password[0] == Password1[0]) &&
        (Password[1] == Password1[1]) &&
        (Password[2] == Password1[2]))
    {
        // ���� Label ��ʾ "���ϴ�������ͬ������"
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, "Same as Previous Password!!!");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);  // �������У�Y ƫ�� 20 ����

        delay_ms(1000);  // ��ʱ 1 ��
        return 1;        // 1 ������ȷ
    }
    else
    {
        // ���� Label ��ʾ "�޸ĳɹ�������"
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, "Password Changed Successfully!!!");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);  // �������У�Y ƫ�� 20 ����

        delay_ms(1000);  // ��ʱ 1 ��
        return 0;        // 0 �������
    }
}
/*********************************************END OF FILE**********************/
