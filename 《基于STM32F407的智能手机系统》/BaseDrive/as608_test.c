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
void zhujiemian(void)	//欢迎界面显示
{
	LCD_Clear(WHITE);
    LCD_ShowString(200,280, (u8 *)"指纹门禁系统",BLACK,WHITE);
    LCD_ShowString(180,380, (u8 *)"1.解锁",BLACK,WHITE);
    LCD_ShowString(280,380, (u8 *)"3.设置",BLACK,WHITE);	
}
/****************************************************************************************************/
void jiesuo(void)	//解锁界面
{
	LCD_Clear(WHITE);
    LCD_ShowString(220,280, (u8 *)"解锁方式",BLACK,WHITE);
    LCD_ShowString(180,380, (u8 *)"1.指纹",BLACK,WHITE);
    LCD_ShowString(230,380, (u8 *)"2.密码",BLACK,WHITE);
    LCD_ShowString(280,380, (u8 *)"3.返回",BLACK,WHITE);
}
/****************************************************************************************************/
void zhujiemian1(void)		//显示管理员设置界面
{
	LCD_Clear(WHITE);
    LCD_ShowString(220,280, (u8 *)"管理员设置",BLACK,WHITE);
    LCD_ShowString(100,380, (u8 *)"1.修改解锁密码",BLACK,WHITE);
    LCD_ShowString(230,380, (u8 *)"2.指纹管理",BLACK,WHITE);
    LCD_ShowString(330,380, (u8 *)"3.退出",BLACK,WHITE);
}
/****************************************************************************************************/
void shanzhiwen(void)	//删指纹界面
{
	LCD_Clear(WHITE);
    LCD_ShowString(220,280, (u8 *)"删除指纹",BLACK,WHITE);
    LCD_ShowString(100,380, (u8 *)"1.删除单个指纹",BLACK,WHITE);
    LCD_ShowString(230,380, (u8 *)"2.清空指纹库",BLACK,WHITE);
    LCD_ShowString(340,380, (u8 *)"3.返回",BLACK,WHITE);
}
/****************************************************************************************************/
void zhiwenguanli(void)		//指纹管理系统界面
{
	LCD_Clear(WHITE);
    LCD_ShowString(220,280, (u8 *)"管理指纹",BLACK,WHITE);
    LCD_ShowString(100,380, (u8 *)"1.添加指纹",BLACK,WHITE);
    LCD_ShowString(200,380, (u8 *)"2.删除指纹",BLACK,WHITE);
    LCD_ShowString(300,380, (u8 *)"3.返回",BLACK,WHITE);
}
void shezhi(void)	//显示输入管理员密码界面
{
	LCD_Clear(WHITE);
    LCD_ShowString(200,280, (u8 *)"输入管理员密码",BLACK,WHITE);
    LCD_ShowString(120,380, (u8 *)"-",BLACK,WHITE);
    LCD_ShowString(220,380, (u8 *)"-",BLACK,WHITE);
    LCD_ShowString(320,380, (u8 *)"-",BLACK,WHITE);
}
/****************************************************************************************************/
void xiugai(void)	//显示输入管理员密码界面
{
	LCD_Clear(WHITE);
    LCD_ShowString(200,280, (u8 *)"输入修改后管理员密码",BLACK,WHITE);
    LCD_ShowString(120,380, (u8 *)"-",BLACK,WHITE);
    LCD_ShowString(220,380, (u8 *)"-",BLACK,WHITE);
    LCD_ShowString(320,380, (u8 *)"-",BLACK,WHITE);
}
/****************************************************************************************************/

void Show_Message(void)     //退出到主界面
{   
    LCD_Clear(WHITE);
    LCD_ShowString(220,380, (u8 *)"退出",BLACK,WHITE);
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
                    // OpenDoor(30);
                    flag00=0;
                }
				if(key==1) mode=1,flag1=0;	//退出
				break;
//显示输入解锁密码界面-------------------------------------------------------------------------------/					
			case 5:	
				if(flag5==1)
				{
					code_lock();					
					flag5=0;					
				}
                // OpenDoor(30);
                mode=1,flag1=0;	//退回主界面
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
              LCD_ShowString(100,280, (u8 *)"输入指纹1正常！！！",BLACK,WHITE);
//              AS608_INFO("\r\n");
            
               sure=PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
            if(sure==0x00)
            {
             sprintf(IDS,"%d",ID);
                LCD_Clear(WHITE);
             LCD_ShowString(100,280, (u8 *)"指纹已存在，指纹ID：",BLACK,WHITE);
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
            LCD_ShowString(100,280, (u8 *)"输入指纹2正常！！！",BLACK,WHITE);    
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

/**
  * @brief  按键输入管理员密码
  * @param  无
  * @retval 无
  */
void key_scan(void)	
{
	unsigned char num=0;
    int flag1=0,flag2=0,flag3=0;//防止长按输入多位密码，清除标志位
	unsigned int i=0;
    char nums[4];
    if((flag0==0)&&(KEY_S3_READ()!=0)){//第一次进入输入密码界面，直到k3按键松开，flag0变1，密码3才能输入
        flag0=1;
    }
	if((KEY_S3_READ()==0)&&(flag3==0)&&(flag0==1))
	{        
		delay_ms(50);
        flag3=0;
			while(KEY_S3_READ()==0)
			{
				if(Password_count<=3)		//如果密码位数小于3
				{
                    num=3;
                    sprintf(nums,"%d",num);
                    LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
					Password[Password_count-1]=num;			 //把输入密码存入数组
					if(KEY_S3_READ()!=0)							//按键松开 输入下一位密码
					{
						Password_count++;
                        flag3=1;
						break;
					}
				}		
		}
	}
    if((KEY_S2_READ()==0)&&(flag2==0))
	{
		delay_ms(50);
        flag2=0;
			while(KEY_S2_READ()==0)
			{
				if(Password_count<=3)		//如果密码位数小于3
				{
                    num=2;
                    sprintf(nums,"%d",num);
                    LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
					Password[Password_count-1]=num;			 //把输入密码存入数组
					if(KEY_S2_READ()!=0)							//按键松开 输入下一位密码
					{
						Password_count++;
                        flag2=0;                        
						break;
					}
				}		
		}
	}
    if((KEY_S1_READ()==0)&&(flag1==0))
	{
		delay_ms(50);
        flag1=1;
			while(KEY_S1_READ()==0)
			{
				if(Password_count<=3)		//如果密码位数小于3
				{
                    num=1;
                    sprintf(nums,"%d",num);
                    LCD_ShowString(Password_count*100+20,380,(u8*)nums,BLACK,WHITE);
					Password[Password_count-1]=num;			 //把输入密码存入数组
					if(KEY_S1_READ()!=0)							//按键松开 输入下一位密码
					{
						Password_count++;
                        flag1=1;                        
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
	if((Password[0]==Password1[0]&&Password[1]==Password1[1]&&Password[2]==Password1[2]))
	{
        LCD_ShowString(200,180, (u8 *)"与上次密码相同！！！",BLACK,WHITE);
		delay_ms(1000);
		return 1;		//1代表正确，0代表错误
	}
	else
	{
        LCD_ShowString(200,180, (u8 *)"修改成功！！！",BLACK,WHITE);
		delay_ms(1000);
		return 0;
	}
}
/*********************************************END OF FILE**********************/
