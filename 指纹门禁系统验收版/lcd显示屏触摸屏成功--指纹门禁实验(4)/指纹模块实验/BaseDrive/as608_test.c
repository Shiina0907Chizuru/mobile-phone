/**
  ******************************************************************************
  * @file    bsp_as608.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   指纹识别模块实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 F103-霸道 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
unsigned char mode=1;					//定义界面标志位，主界面
unsigned char flag1=0,flag2=0,flag3=0,flag4=0,flag5=0,flag6=0,flag7=0,flag8=0,flag9=0,flag10=0,flag11=0,flag12=0;	//进入界面的标志位
unsigned char Password_count=1; 		//密码位数
unsigned char Password[3]={0,0,0};      //存放按键输入管理员密码数组
unsigned char Password1[3]={2,3,1};     //管理员密码
unsigned char x,y,a;  					//匹配密码的返回值
int flag0=0;                            //第一次进入输入密码的标志位
int flag00=0;                           //解锁成功标志位
void key_scan(void);//按键输入管理员密码
unsigned char pipei(void);//管理员密码匹配
void code_lock(void);//密码锁
/****************************************************************************************************/
static void btn_light_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        // 调用“开关灯”函数
        LED6_REVERSE;
    }
}
void zhujiemian(void)	//欢迎界面显示
{
	LCD_Clear(WHITE);
    // LCD_ShowString(200,280, (u8 *)"指纹门禁系统",BLACK,WHITE);
    // LCD_ShowString(180,380, (u8 *)"1.解锁",BLACK,WHITE);
    // LCD_ShowString(280,380, (u8 *)"3.设置",BLACK,WHITE);
   // 获取当前活动屏幕
    lv_obj_t *scr = lv_scr_act();
    // 如果需要先清理以前的组件，可以使用 lv_obj_clean(scr);
    lv_obj_clean(scr);

    // -------------------------
    // 0. 可选：为屏幕或背景设置颜色等样式
    // -------------------------
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // -------------------------
    // 1. 主标题
    // -------------------------
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Fingerprint Access System");
    // 将主标题放在屏幕正中稍靠上位置，例如Y偏移 -50
    lv_obj_align(label_title, LV_ALIGN_CENTER, 0, -50);

    // 创建并应用一个标题样式，使文字变大
    static lv_style_t style_title;
    lv_style_init(&style_title);
    // 使用内置的 Montserrat 28 号字体（需在配置中启用）
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    // 如果想改颜色，也可加 lv_style_set_text_color(...)
    lv_obj_add_style(label_title, &style_title, 0);

    // -------------------------
    // 2. 选项1：Unlock（需显示蓝色）
    // -------------------------
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Unlock");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -10);

    // 将文字设置为蓝色
    lv_obj_set_style_text_color(label_option1, lv_palette_main(LV_PALETTE_BLUE), 0);

    // lv_anim_t blink_anim;
    // lv_anim_init(&blink_anim);
    // lv_anim_set_var(&blink_anim, label_option1);  // 设置动画对象
    // lv_anim_set_values(&blink_anim, LV_OPA_TRANSP, LV_OPA_COVER);  // 在透明和完全可见之间切换
    // lv_anim_set_time(&blink_anim, 500);  // 每次变化时间为 500ms
    // lv_anim_set_playback_time(&blink_anim, 500);  // 回退时间也为 500ms
    // lv_anim_set_repeat_count(&blink_anim, LV_ANIM_REPEAT_INFINITE);  // 无限循环
    // lv_anim_set_exec_cb(&blink_anim, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);  // 修改透明度
    // lv_anim_start(&blink_anim);
    // 或者使用 lv_color_blue():
    // lv_obj_set_style_text_color(label_option1, lv_color_blue(), 0);

    // -------------------------
    // 3. 选项2：Enroll Fingerprint
    // -------------------------
    // lv_obj_t *label_option2 = lv_label_create(scr);
    // lv_label_set_text(label_option2, "2. Enroll Fingerprint");
    // lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 20);

    // -------------------------
    // 4. 选项3：Settings
    // -------------------------
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Settings");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);

     // ------------------------------------------------------------------------
    // 6. 创建一个按钮，并在上面显示文字 "Toggle Light"
    // ------------------------------------------------------------------------
    // 6.1. 创建按钮对象
    lv_obj_t *btn_light = lv_btn_create(scr);
    // 将按钮对齐到屏幕中心，Y 偏移 +90
    lv_obj_align(btn_light, LV_ALIGN_CENTER, 0, 90);

    // 6.2. 在按钮上创建一个标签用于显示文字
    lv_obj_t *label_btn = lv_label_create(btn_light);
    lv_label_set_text(label_btn, "Toggle Light");
    // lv_obj_center(label_btn); // 把标签居中到按钮里

    // 6.3. 给按钮添加点击事件回调
    //      当用户点击按钮时，调用 toggle_light() 来开/关灯
    lv_obj_add_event_cb(btn_light, btn_light_event_cb, LV_EVENT_CLICKED, NULL);

    // 如果想做进一步美化，例如修改文字颜色、背景颜色，
    // 可以对各标签或屏幕设置 style
    // 示例：改变文字颜色 -> lv_obj_set_style_text_color(label_title, lv_color_hex(0x000000), 0);
    // 示例：改变屏幕背景色 -> lv_obj_set_style_bg_color(scr, lv_color_white(), 0);	
}
/****************************************************************************************************/
void jiesuo(void)	//解锁界面
{
	LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"解锁方式",BLACK,WHITE);
    // LCD_ShowString(180,380, (u8 *)"1.指纹",BLACK,WHITE);
    // LCD_ShowString(230,380, (u8 *)"2.密码",BLACK,WHITE);
    // LCD_ShowString(280,380, (u8 *)"3.返回",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 设置屏幕背景为白色
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 标题：解锁方式
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Unlock Method");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // 选项 1: 指纹解锁
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Fingerprint Unlock");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // 选项 2: 密码
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Password");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // 选项 3: 返回（红色字体）
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
/****************************************************************************************************/
void zhujiemian1(void)		//显示管理员设置界面
{
	LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"管理员设置",BLACK,WHITE);
    // LCD_ShowString(100,380, (u8 *)"1.修改解锁密码",BLACK,WHITE);
    // LCD_ShowString(230,380, (u8 *)"2.指纹管理",BLACK,WHITE);
    // LCD_ShowString(330,380, (u8 *)"3.退出",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 设置屏幕背景为白色
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 标题：管理员设置
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Admin Settings");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // 选项 1: 修改解锁密码
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Change Unlock Password");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // 选项 2: 指纹管理
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Fingerprint Management");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // 选项 3: 返回（红色字体）
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
/****************************************************************************************************/
void shanzhiwen(void)	//删指纹界面
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"删除指纹",BLACK,WHITE);
    // LCD_ShowString(100,380, (u8 *)"1.删除单个指纹",BLACK,WHITE);
    // LCD_ShowString(230,380, (u8 *)"2.清空指纹库",BLACK,WHITE);
    // LCD_ShowString(340,380, (u8 *)"3.返回",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 设置屏幕背景为白色
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 标题：删除指纹
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Delete Fingerprint");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // 选项 1: 删除单个指纹
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Delete Single Fingerprint");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // 选项 2: 删除所有指纹
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Delete All Fingerprints");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // 选项 3: 返回（红色字体）
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
/****************************************************************************************************/
void zhiwenguanli(void)		//指纹管理系统界面
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(220,280, (u8 *)"管理指纹",BLACK,WHITE);
    // LCD_ShowString(100,380, (u8 *)"1.添加指纹",BLACK,WHITE);
    // LCD_ShowString(200,380, (u8 *)"2.删除指纹",BLACK,WHITE);
    // LCD_ShowString(300,380, (u8 *)"3.返回",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 设置屏幕背景为白色
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 标题：指纹管理
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Fingerprint Management");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // 选项 1: 添加指纹
    lv_obj_t *label_option1 = lv_label_create(scr);
    lv_label_set_text(label_option1, "1. Add Fingerprint");
    lv_obj_align(label_option1, LV_ALIGN_CENTER, 0, -30);

    // 选项 2: 删除指纹
    lv_obj_t *label_option2 = lv_label_create(scr);
    lv_label_set_text(label_option2, "2. Delete Fingerprint");
    lv_obj_align(label_option2, LV_ALIGN_CENTER, 0, 10);

    // 选项 3: 返回（红色字体）
    lv_obj_t *label_option3 = lv_label_create(scr);
    lv_label_set_text(label_option3, "3. Back");
    lv_obj_align(label_option3, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_text_color(label_option3, lv_palette_main(LV_PALETTE_RED), 0);
}
void shezhi(void)	//显示输入管理员密码界面
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(200,280, (u8 *)"输入管理员密码",BLACK,WHITE);
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
void xiugai(void)	//显示输入管理员密码界面
{
	// LCD_Clear(WHITE);
    // LCD_ShowString(200,280, (u8 *)"输入修改后管理员密码",BLACK,WHITE);
    // LCD_ShowString(120,380, (u8 *)"-",BLACK,WHITE);
    // LCD_ShowString(220,380, (u8 *)"-",BLACK,WHITE);
    // LCD_ShowString(320,380, (u8 *)"-",BLACK,WHITE);
    // 1. 获取当前活动屏幕并清空
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 2. 设置背景颜色为白色
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 3. 创建并设置标题 “输入修改后管理员密码”
    lv_obj_t *label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Input New Admin Password"); 
    // 如果想要中文，可以用 "输入修改后管理员密码"
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    // 如果想要更大字体，可使用样式
    static lv_style_t style_title;
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_28);
    lv_obj_add_style(label_title, &style_title, 0);

    // 4. 创建 3 条“横线”标签，可视为占位符
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

void Show_Message(void)     //退出到主界面
{   
    // LCD_Clear(WHITE);
    // LCD_ShowString(220,380, (u8 *)"退出",BLACK,WHITE);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);  // 清空屏幕

    // 设置屏幕背景为白色
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 创建一个 Label 显示 "Exit"
    lv_obj_t *label_exit = lv_label_create(scr);
    lv_label_set_text(label_exit, "Exit");
    lv_obj_set_style_text_color(label_exit, lv_palette_main(LV_PALETTE_RED), 0);  // 设置为红色字体

    // 设置 Label 使用 lv_font_montserrat_28 字体
    static lv_style_t style_exit;
    lv_style_init(&style_exit);
    lv_style_set_text_font(&style_exit, &lv_font_montserrat_28);  // 设置字体为 28
    lv_obj_add_style(label_exit, &style_exit, 0);

    // 初始位置在屏幕左侧
    lv_obj_set_pos(label_exit, -lv_obj_get_width(label_exit), lv_obj_get_height(scr) / 2);

    // 获取屏幕宽度，用于计算终点位置
    int32_t screen_center_x = (lv_obj_get_width(scr) - lv_obj_get_width(label_exit)) / 2;

    // 创建滑动动画
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, label_exit);
    lv_anim_set_values(&anim, -lv_obj_get_width(label_exit), screen_center_x);  // 从左滑动到屏幕中间
    lv_anim_set_time(&anim, 1500);  // 动画时长 1500ms
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_x);  // 修改 X 坐标
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);  // 平滑动画曲线
    lv_anim_start(&anim);

    // // 创建淡入动画，并设置为呼吸灯效果
    // lv_anim_t fade;
    // lv_anim_init(&fade);
    // lv_anim_set_var(&fade, label_exit);
    // lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);  // 从透明到完全可见
    // lv_anim_set_time(&fade, 1000);  // 单次淡入时间 1000ms
    // lv_anim_set_playback_time(&fade, 1000);  // 添加播放回退时间，让亮度从完全可见回到透明
    // // lv_anim_set_repeat_count(&fade, LV_ANIM_REPEAT_INFINITE);  // 无限循环
    // lv_anim_set_exec_cb(&fade, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    // lv_anim_start(&fade);
}



/**
  * @brief  指纹模块操作
  * @param  无
  * @retval 无
  */
void  FR_Task(void)
{	
    uint32_t   ch;
    unsigned char key=0;			//定义缓存键值
	key = KeyScan();
	switch(mode)
	{
//显示欢迎界面------------------------------------------------------------------------------------/			
            case 1:		
				if(flag1==0)				//只进来一次
				{
					zhujiemian();
					flag1=1;					
				}
				if(key==1)mode=2,flag2=1;//进选择解锁方式界面
				if(key==3)mode=3,flag3=1;//进管理员解锁界面
				break; 
//显示选择解锁方式------------------------------------------------------------------------------------/				
			case 2:		
				if(flag2==1)
				{
					jiesuo();					
					flag2=0;
				}
				if(key==1) mode=4,flag4=1;	//进去指纹解锁
				if(key==2) mode=5,flag5=1;	//进去密码解锁
                if(key==3) mode=1,flag1=0;	//返回上一级
				break;
//显示输入管理员密码界面------------------------------------------------------------------------------/					
			case 3:		
				if(flag3==1)
				{
					shezhi();				
					flag3=0;
                    flag0=0;//表面第一次从上一级界面进入密码输入界面（防止key3长按）
				}
                delay_ms(10);
//                if(key==3)  key=0;          //
                read_password();
				key_scan();					//输入密码
				if(Password_count==4)		//如果输入密码位满三位数
				{
					delay_ms(100);			//延迟一会
					x=pipei();      		 //返回值1:密码正确
					if(x==1) 
					{
						Password_count=1;	//密码位清零方便下次输入密码
						mode=6;				//密码正确 跳转到管理员设置界面
						flag6=1;
					}
					else 
					{
						Password_count=1;		//密码个数清零,重新输入密码
						flag3=1;			//显示界面刷一遍
					}
				}
				break;
//显示刷指纹--------------------------------------------------------------------------------------/				
			case 4:
				if(flag4==1)
				{
                    Compare_FR();					//对比指纹
                    Show_Message();
					flag4=0;

				}
                if(flag00==1)   {
                    OpenDoor(30);
                    flag00=0;
                }
				if(key==1) mode=1,flag1=0;	//退出
				break;
//显示输入解锁密码界面-------------------------------------------------------------------------------/					
			case 5:	
				if(flag5==1)
				{
					create_password_lock_screen();
                    	
					flag5=0;					
					
				}   
                if(passflag==1){
                    OpenDoor(30);
                    mode=1,flag1=0;	//退回主界面
                }
                
				break;
//显示管理员设置界面-----------------------------------------------------------------------------------/					
			case 6:	
				if(flag6==1)
				{
					flag6=0;
					zhujiemian1();					
				}
				if(key==1) mode=7,flag7=1;	//进去修改解锁密码界面
				if(key==2) mode=8,flag8=1;	//进去指纹管理界面
				if(key==3) mode=1,flag1=0;	//退回主界面			
				break;
//修改解锁密码界面-----------------------------------------------------------------------------------/	
			case 7:		
				if(flag7==1)
				{
					flag7=0;
                    xiugai();
                    flag0=0;//表面第一次从上一级界面进入密码输入界面（防止key3长按）
				}
                delay_ms(10);
                read_password();
//                if(key==3)  key=0;          //
				key_scan();					//输入密码
				if(Password_count==4)		//如果输入密码位满三位数
				{
					delay_ms(100);			//延迟一会
					x=pipei2();      		 //返回值1:密码与初始密码一致
					if(x==1) 
					{
						Password_count=1;	//密码位清零方便下次输入密码
                        LCD_ShowString(200,330, (u8 *)"与上次密码输入一致，请重新输入",RED,WHITE);   
                        delay_ms(1000);
                        flag7=1;			//显示界面刷一遍
					}
					else 
					{
                        //将输入的新密码赋给管理员密码数组
                        for(int i=0;i<3;i++){
                            Password1[i]=Password[i];
                        }
                        //将新密码写入flash里面
                        save_password();
						Password_count=1;		//密码个数清零,重新输入密码
						mode=6;				//密码正确 跳转到管理员设置界面
						flag6=1;
					}
				}

//					if(a==1) 
//					{
//						Password_count=1;	//密码位清零方便下次输入密码
//						mode=1;				//密码修改成功   跳转到最初主界面
//						flag1=0;
//					}
//					else 					//密码一致重新输入
//					{
//						Password_count=1;		//密码个数清零,重新输入密码
//						flag7=1;			//输入解锁密码界面刷一遍
//					}
				break;                
//指纹管理界面---------------------------------------------------------------------------------------/	
			case 8:
				if(flag8==1)
				{
					zhiwenguanli();
					flag8=0;					
				}
				if(key==1) mode=9,flag9=1;		//进去添加指纹界面
				if(key==2) mode=10,flag10=1;		//进去删除指纹界面
				if(key==3) mode=6,flag6=1;		//返回
				break;	
//录指纹界面---------------------------------------------------------------------------------------/				
			case 9:
				if(flag9==1)
				{
					Add_FR();         //添加指纹
                    Show_Message();			
					flag9=0;
				}
				if(key==1) mode=1,flag1=0;	//退出
				break;
////删除指纹界面---------------------------------------------------------------------------------------/				
			case 10:
				if(flag10==1)
				{
					shanzhiwen();
					flag10=0;	
				}
				if(key==1) mode=11,flag11=1;		//进去删除单个指纹界面
				if(key==2) mode=12,flag12=1;		//进去清空指纹库
				if(key==3) mode=6,flag6=1;		//返回
				break;
////删除单个指纹------------------------------------------------------------------------------/				
			case 11:
				if(flag11==1)
				{
					Del_FR();
                    Show_Message();
					flag11=0;
				}
                if(key==1) mode=1,flag1=0;	//退出
				break;
//清空指纹库----------------------------------------------------------------------------/				
			case 12:
				if(flag12==1)		
				{
					Clean_FR();
                    Show_Message();
					flag12=0;
				}
                if(key==1) mode=1,flag1=0;	//退出
				break;        
    }
    return;		
}


/**
  * @brief  检测与指纹模块的通信
  * @param  无
  * @retval 无
  */
void  Connect_Test(void)
{   	
  if(PS_Connect(&AS608_Addr))                      /*与AS608串口通信*/
  { 
    AS608_INFO("未检测到指纹模块请检查连接\r\n");			  
  }
  else
  {
	AS608_INFO("通讯成功\r\n");
	Show_Message();
  }
}


/**
  * @brief  录指纹
  * @param  无
  * @retval 无
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
      case 0:                            /*执行第1步*/
        
        i++;
        LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"******命令：请按手指******",BLACK,WHITE);
//        AS608_INFO("\r\n");
        
        sure=PS_GetImage();              /*录入图像*/      
        if(sure == 0x00)
        {
          sure=PS_GenChar(CHAR_BUFFER1);  /*生成特征1*/
          if(sure==0x00)
          { 
              LCD_Clear(WHITE);
              LCD_ShowString(100,280, (u8 *)"输入指纹1正常!!!",BLACK,WHITE);
//              AS608_INFO("\r\n");
            
               sure=PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
            if(sure==0x00)
            {
             sprintf(IDS,"%d",ID);
                LCD_Clear(WHITE);
             LCD_ShowString(100,280, (u8 *)"指纹已存在,指纹ID:",BLACK,WHITE);
             LCD_ShowString(300,280, (u8 *)IDS ,BLACK,WHITE);  
             delay_ms(2000);          
//             AS608_INFO ("%d！！！\r\n\r\n",ID);             
             return ;
            }              
            else 
            {
              i=0;
              j=1;                   /*跳转到第2步*/
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
        LCD_ShowString(100,280, (u8 *)"******命令：请再按一次手指******",BLACK,WHITE);
//        AS608_INFO("\r\n");
      
        sure=PS_GetImage();
        if(sure==0x00)
        { 
          sure=PS_GenChar(CHAR_BUFFER2);  /*生成特征2*/
          if(sure==0x00)
          { 
			LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"输入指纹2正常!!!",BLACK,WHITE);    
//              AS608_INFO("\r\n");
            i=0;
            j=2;                   /*跳转到第3步*/
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
         LCD_ShowString(100,280, (u8 *)"正在对比两次输入的指纹",BLACK,WHITE);
//         AS608_INFO("\r\n");
       
         sure=PS_Match();                /*精确比对两枚指纹特征*/
         if(sure==0x00) 
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"输入指纹对比成功！！！",BLACK,WHITE);
//				   AS608_INFO("\r\n");
           
           j=3;                         /*跳转到第4步*/
         }
         else
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"对比失败，请重新录入指纹！！！",BLACK,WHITE);
//           AS608_INFO("\r\n");
           ShowErrMessage(sure);
           i=0;
           j=0;
         }
        
       break;
         
         
       case 3:
         LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"正在生成指纹模块",BLACK,WHITE);
//         AS608_INFO("\r\n");
       
         sure=PS_RegModel();            /*合并特征（生成模板）*/
         if(sure==0x00) 	
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"生成指纹模块成功！！！",BLACK,WHITE);
//           AS608_INFO("\r\n");
           j=4;                        /*跳转到第5步*/
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
            LCD_ShowString(100,280, (u8 *)"***命令：请输入存储ID，范围为0~239***",BLACK,WHITE);
            delay_ms(2000);
//           AS608_INFO ("\r\n");
        
           ID=GET_NUM();
         }while(!(ID<PS_MAXNUM));
        
         sure=PS_StoreChar(CHAR_BUFFER2,ID);  /*储存模板*/
         if(sure==0x00) 	
         {
             LCD_Clear(WHITE);
            LCD_ShowString(100,280, (u8 *)"录入指纹模块成功！！！",BLACK,WHITE);
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
      
      if(i==4)                       /*超过4次没有按手指则退出*/
      { 
          LCD_Clear(WHITE);
          LCD_ShowString(100,280, (u8 *)"录指纹失败！",BLACK,WHITE);
//        AS608_INFO("\r\n\r\n\r\n");
        break;
      }
  }
}


/**
  * @brief  比对指纹（识别指纹）
  * @param  无
  * @retval 无
  */
void Compare_FR(void)  
{  
  uint16_t  ID=0;                    /*初始化ID值*/  
  uint16_t  sure;
  char IDS[5];  
  
  LCD_Clear(WHITE);
  LCD_ShowString(100,280, (u8 *)"******命令：对比指纹，请按手指******",BLACK,WHITE);
//  AS608_INFO("******命令：对比指纹，请按手指******\r\n");

  sure=PS_GetImage();
  if(sure == 0x00)
  {
    sure=PS_GenChar(CHAR_BUFFER1);    /*生成特征1*/
    if(sure==0x00)
    {  
			/*高速搜索指纹库*/
      sure=PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
      if(sure==0x00)
      {
        sprintf(IDS,"%d",ID);
        LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"对比指纹成功，指纹ID：",BLACK,WHITE); 
        LCD_ShowString(300,280, (u8 *)IDS,BLACK,WHITE);
        flag00=1;
        delay_ms(2000);
//        AS608_INFO ("对比指纹成功，指纹ID：%d！！！\r\n\r\n",ID);
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
  * @brief  清空指纹库
  * @param  无
  * @retval 无
  */
void Clean_FR(void) 
{
  uint16_t  sure;
    LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"清空指纹库",BLACK,WHITE);
//	AS608_INFO("\r\n");
  
	sure=PS_Empty();     
	if(sure == 0x00)
	{
        LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"清空指纹库成功！！！",BLACK,WHITE);
        delay_ms(2000);
	}
	else 
	{
		ShowErrMessage(sure);
	}

}


/**
  * @brief  删除指定用户指纹
  * @param  无
  * @retval 无
  */
void Del_FR(void)
{ 
	uint32_t  ID;
	uint16_t  sure;
    LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"删除指定用户ID",BLACK,WHITE);
//	AS608_INFO("删除指定用户ID\r\n");
  
	do
	{ 
		LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"***命令：请输入存储ID，范围为0~239***",BLACK,WHITE);
//        AS608_INFO ("***命令：请输入存储ID，范围为0~239***\r\n");
        delay_ms(2000);
		ID=GET_NUM();
	}while(!(ID<PS_MAXNUM));         /*输入ID不能超过指纹模块最大容量*/
	
	sure=PS_DeletChar(ID,1);         /*删除指定用户的指纹模板*/
	
	if(sure == 0x00)                
	{
		LCD_Clear(WHITE);
        LCD_ShowString(100,280, (u8 *)"删除指定用户指纹成功！！！",BLACK,WHITE);
        delay_ms(3000);
//        AS608_INFO("\r\n\r\n");
	}
	else 
	{
		ShowErrMessage(sure);
	}
}

/**
  * @brief  密码锁
  * @param  无
  * @retval 无
  */

#define MAX_PASS_LEN  8                  /* 最大输入位数 */
static const char * correct_pass = "1234";  /* 正确密码，可根据需要修改 */

/* 用于存储用户输入 */
static char input_buffer[MAX_PASS_LEN + 1] = {0};
static uint8_t input_len = 0;

/* 用于显示输入和提示信息的控件 */
static lv_obj_t * label_input;
static lv_obj_t * label_hint;

/*
 * 按钮矩阵文本映射
 * - "Del" 删除
 * - "OK"  确认
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
        // TODO: 密码正确后的操作
    } else {
        lv_label_set_text(label_hint, "Incorrect password, please try again!");
    }

    memset(input_buffer, 0, sizeof(input_buffer));
    input_len = 0;
    update_input_label();
}

/* 按钮矩阵事件回调 */
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
    /* 1. 创建一个新的屏幕对象 */
    lv_obj_t * scr = lv_obj_create(NULL);

    /* 2. 创建标题标签 */
    lv_obj_t * label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Please enter password:");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 20);

    /* 3. 用于显示当前输入的标签 */
    label_input = lv_label_create(scr);
    lv_label_set_text(label_input, "");
    lv_obj_set_style_text_font(label_input, &lv_font_montserrat_28, 0);  
    lv_obj_align(label_input, LV_ALIGN_TOP_MID, 0, 60);

    /* 4. 用于显示提示/结果的标签 */
    label_hint = lv_label_create(scr);
    lv_label_set_text(label_hint, "Awaiting input...");
    lv_obj_align(label_hint, LV_ALIGN_TOP_MID, 0, 100);

    /* 5. 创建按钮矩阵（输入数字、删除和确认） */
    lv_obj_t * btnm = lv_btnmatrix_create(scr);
    lv_btnmatrix_set_map(btnm, btnm_map);
    lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 40);

    /* ---------- 关键部分：让按钮尺寸变大 ---------- */
    lv_obj_set_style_pad_all(btnm, 10, 0);       /* 矩阵四周边距 */
    lv_obj_set_style_pad_row(btnm, 10, 0);       /* 行间距 */
    lv_obj_set_style_pad_column(btnm, 10, 0);    /* 列间距 */
    lv_obj_set_style_width(btnm, 80, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_height(btnm, 60, LV_PART_ITEMS | LV_STATE_DEFAULT);

    /* 如果需要更大的文字，可以启用并设置大字体 */
    // lv_obj_set_style_text_font(btnm, &lv_font_montserrat_28, LV_PART_ITEMS | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 6. 切换到此新屏幕 */
    lv_scr_load(scr);
}



void code_lock(void)
{
    u8 t = 0;
    u8 i = 0;
    int j =11;
    
    u16 Password[4] = {0,0,0,0}; //存放输入密码的数组
    u16 CorrectPass[4] = {0,3,1,9}; //正确的密码
    int index = 0;
    u8 full_flag = 0;
    // 定义标志位
    u8 confirm_flag = 0;
    u16 lastpos[5][2];		//最后一次的数据

//    delay_init(168);			//延时初始化
    tp_dev.init();				//触摸屏初始化
    LCD_Clear(WHITE);
    LCD_ShowString(135, 155, (u8 *)"按键前先按下确认键",BLACK,WHITE);
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
    LCD_DrawRectangle(65,675,155,765,BLACK);//确认
    LCD_ShowString(360, 710, (u8 *)"删除",BLACK,WHITE);
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
    LCD_ShowString(105, 710, (u8 *)"确认",BLACK,WHITE);
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
            //确认
            if((tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>675 && tp_dev.y[t]<765)&&(confirm_flag==0)){
                confirm_flag=1;
            }
            //0
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>675 && tp_dev.y[t]<765)
            {
                //确认键方案
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 0;
                full_flag = 0;
                confirm_flag=0;//确认
                //消抖方案
                // while(confirm_flag==0){
                //     tp_dev.scan(0);
                //     if(!(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>675 && tp_dev.y[t]<765)) confirm_flag=1;
                //     delay_ms(50);
                // } 
                // confirm_flag=2;
                // while(GT9147_Scan(0) == 1) ;//当触摸屏按下时候卡住，松开继续
            }
            //1
            if(tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>285 && tp_dev.y[t]<375)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 1;
                full_flag = 0;
                confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //2
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>285 && tp_dev.y[t]<375)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 2;
                full_flag = 0;
                confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //3
            if(tp_dev.x[t]>325 &&tp_dev.x[t]<415 && tp_dev.y[t]>285 && tp_dev.y[t]<375)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 3;
                full_flag = 0;
                confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //4
            if(tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>415 && tp_dev.y[t]<505)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 4;
                full_flag = 0;
                confirm_flag=0;//确认
                // while((int)tp_dev.scan== 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //5
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>415 && tp_dev.y[t]<505)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 5;
                full_flag = 0;
                confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //6
            if(tp_dev.x[t]>325 && tp_dev.x[t]<415 && tp_dev.y[t]>415 && tp_dev.y[t]<505)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 6;
                full_flag = 0;
                confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //7
            if(tp_dev.x[t]>65 && tp_dev.x[t]<155 && tp_dev.y[t]>545 && tp_dev.y[t]<635)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 7;
                full_flag = 0;
                 confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //8
            if(tp_dev.x[t]>195 && tp_dev.x[t]<285 && tp_dev.y[t]>545 && tp_dev.y[t]<635)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 8;
                full_flag = 0;
                 confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //9
            if(tp_dev.x[t]>325 && tp_dev.x[t]<415 && tp_dev.y[t]>545 && tp_dev.y[t]<635)
            {
                if(confirm_flag==0) continue;//确认
                index ++;
                if(index > 4)
                    index = 4;
                Password[index - 1] = 9;
                full_flag = 0;
                 confirm_flag=0;//确认
                // while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
                delay_ms(50);
            }
            //删除
            if(tp_dev.x[t]>325 && tp_dev.x[t]<415 && tp_dev.y[t]>675 && tp_dev.y[t]<765)
            {
                index --;
                if(index < 0)
                    index = 0;
                Password[index] = 0;
                full_flag = 0;
                while((int)tp_dev.scan == 1) ;//当触摸屏按下时候卡住，松开继续
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
                LCD_ShowString(220,120,(u8*)"解锁成功",BLACK,WHITE);
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
    // 创建一个新的 label
    lv_obj_t *label_num = lv_label_create(lv_scr_act());
    lv_label_set_text(label_num, str);

    // 根据 index 决定坐标
    // 参考 dash1/dash2/dash3:
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

    // 如果需要更大字体或不同颜色，可以再添加 style
    // static lv_style_t style_digits;
    // lv_style_init(&style_digits);
    // lv_style_set_text_font(&style_digits, &lv_font_montserrat_28);
    // lv_obj_add_style(label_num, &style_digits, 0);
}
/**
  * @brief  按键输入管理员密码
  * @param  无
  * @retval 无
  */
void key_scan(void)	
{
	// unsigned char num=0;
    // int flag1=0,flag2=0,flag3=0;//防止长按输入多位密码，清除标志位
	// unsigned int i=0;
    // char nums[4];
    // if((flag0==0)&&(KEY_S3_READ()!=0)){//第一次进入输入密码界面，直到k3按键松开，flag0变1，密码3才能输入
    //     flag0=1;
    // }
	// if((KEY_S3_READ()==0)&&(flag3==0)&&(flag0==1))
	// {        
	// 	delay_ms(50);
    //     flag3=0;
	// 		while(KEY_S3_READ()==0)
	// 		{
	// 			if(Password_count<=3)		//如果密码位数小于3
	// 			{
    //                 num=3;
    //                 sprintf(nums,"%d",num);
    //                 LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
	// 				Password[Password_count-1]=num;			 //把输入密码存入数组
	// 				if(KEY_S3_READ()!=0)							//按键松开 输入下一位密码
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
	// 			if(Password_count<=3)		//如果密码位数小于3
	// 			{
    //                 num=2;
    //                 sprintf(nums,"%d",num);
    //                 LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
	// 				Password[Password_count-1]=num;			 //把输入密码存入数组
	// 				if(KEY_S2_READ()!=0)							//按键松开 输入下一位密码
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
	// 			if(Password_count<=3)		//如果密码位数小于3
	// 			{
    //                 num=1;
    //                 sprintf(nums,"%d",num);
    //                 LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
	// 				Password[Password_count-1]=num;			 //把输入密码存入数组
	// 				if(KEY_S1_READ()!=0)							//按键松开 输入下一位密码
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

    // 这里假设已经有全局变量:
    // extern unsigned char Password[4];    // 存放密码的数组
    // extern unsigned char Password_count; // 当前输入了多少位 (1~4)
    // extern int flag0;
    // 以及按键检测函数 KEY_S1_READ(), KEY_S2_READ(), KEY_S3_READ()
    // 延时函数 delay_ms()

    // 如果 flag0==0, 并且 S3 已经松开, 则允许输入:
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
            // 如果当前还没输入满 3 位(或 4 位)，这里假设最多 3 位
            if (Password_count <= 3)
            {
                num = 3;
                sprintf(nums, "%d", num);

                // 显示数字到指定位置
                show_num_with_dash_alignment(Password_count, nums);

                // 存入密码数组
                Password[Password_count - 1] = num;

                // 松开按键后，下一位
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
  * @brief  进来管理员密码匹配
  * @param  无
  * @retval 无
  */
unsigned char pipei(void)
{
	if((Password[0]==Password1[0]&&Password[1]==Password1[1]&&Password[2]==Password1[2]))
	{
        LCD_ShowString(200,180, (u8 *)"密码正确！！！",BLACK,WHITE);
		delay_ms(1000);
		return 1;		//1代表正确，0代表错误
	}
	else
	{
        LCD_ShowString(200,180, (u8 *)"密码错误！！！",BLACK,WHITE);
		delay_ms(1000);
		return 0;
	}
}

/**
  * @brief  进来管理员密码匹配2
  * @param  无
  * @retval 无
  */
unsigned char pipei2(void)
{
	// if((Password[0]==Password1[0]&&Password[1]==Password1[1]&&Password[2]==Password1[2]))
	// {
    //     LCD_ShowString(200,180, (u8 *)"与上次密码相同！！！",BLACK,WHITE);
	// 	delay_ms(1000);
	// 	return 1;		//1代表正确，0代表错误
	// }
	// else
	// {
    //     LCD_ShowString(200,180, (u8 *)"修改成功！！！",BLACK,WHITE);
	// 	delay_ms(1000);
	// 	return 0;
	// }
    lv_obj_t *scr = lv_scr_act();  // 获取当前活动屏幕
    lv_obj_clean(scr);            // 清空屏幕

    if ((Password[0] == Password1[0]) &&
        (Password[1] == Password1[1]) &&
        (Password[2] == Password1[2]))
    {
        // 创建 Label 显示 "与上次密码相同！！！"
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, "Same as Previous Password!!!");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);  // 顶部居中，Y 偏移 20 像素

        delay_ms(1000);  // 延时 1 秒
        return 1;        // 1 代表正确
    }
    else
    {
        // 创建 Label 显示 "修改成功！！！"
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, "Password Changed Successfully!!!");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);  // 顶部居中，Y 偏移 20 像素

        delay_ms(1000);  // 延时 1 秒
        return 0;        // 0 代表错误
    }
}
/*********************************************END OF FILE**********************/
