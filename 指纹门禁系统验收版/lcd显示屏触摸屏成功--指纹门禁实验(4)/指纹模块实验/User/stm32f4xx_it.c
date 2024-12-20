/**
  ******************************************************************************
  * @file    EXTI/EXTI_Example/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "USART.h"
#include "delay.h"
#include "rx_data_queue.h"
#include "LED.h"
#include "bsp_as608.h"
#include "mp3.h" 
extern u8 num_a;
extern u8 PWMflag;
extern u8 flag;
u32 PWMcnt = 0;
u32 opendoortime=3000;
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup EXTI_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{     
	static unsigned int cnt = 0;

	cnt++;
	if (cnt >= 1000)
	{
		cnt = 0;
		LED1_REVERSE;
        flag = 1;
	}
  if(PWMflag==1){
		TIM_SetCompare1(TIM1, 90);
    PWMcnt++;
    if(PWMcnt>opendoortime){
      PWMflag=0;
      PWMcnt=0;
      TIM_SetCompare1(TIM1, 30);
    }
  }
    TimingDelay_Decrement();
  //USART2串口接收	
	if (0 != Uart3.Time)
	{  
		Uart3.Time--;
        
		if ( Uart3.Time == 0 )
		{
			Uart3.ReceiveFinish = 1;
		}
	}
    
}


//Usart3串口中断程序USART3_IRQHandler **************************************************************************
void USART3_IRQHandler(void)
{	    
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)              
	{	
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);        
        
        Uart3.Rxbuf[Uart3.RXlenth++] = USART_ReceiveData(USART3);  
        num_a=USART_ReceiveData(USART3)-48;     
        printf("num_a:%d",num_a);
        
		Uart3.Time = 3;
	}
} 

void USART6_IRQHandler(void)
{
    if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)                //  若接收数据寄存器满
	{
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
        
        Uart6.Rxbuf[Uart6.RXlenth] = USART_ReceiveData(USART6);
        
        if (Uart6.RXlenth == 0 && Uart6.Rxbuf[0] != 0x55)
        {
            Uart6.RXlenth = 0;
            return;
        }
        if (Uart6.RXlenth == 1 && Uart6.Rxbuf[1] != 0x53)
        {
            Uart6.RXlenth = 0;
            return;
        }
        
        
        Uart6.RXlenth++;
        
        if(Uart6.RXlenth == 11)
        {
            memcpy(databuf, &Uart6.Rxbuf[0], 20);
            
            display_flag = 1;
            Uart6.RXlenth = 0;
        }
    }
}

void AS608_TouchOut_IRQHandler(void)
{
  /*确保是否产生了EXTI Line中断*/
	if(EXTI_GetITStatus(AS608_TouchOut_INT_EXTI_LINE) != RESET) 
	{
		/*LED反转*/	
//		LED1_TOGGLE;

    EXTI_ClearITPendingBit(AS608_TouchOut_INT_EXTI_LINE);   
  }
}


  /**
  * @brief  串口中断服务函数,把接收到的数据写入缓冲区，
            在main函数中轮询缓冲区输出数据
  * @param  None
  * @retval None
  */
  void AS608_USART_IRQHandler(void)
{
 	uint8_t ucCh;
	QUEUE_DATA_TYPE *data_p; 
	
	if(USART_GetITStatus(AS608_USART,USART_IT_RXNE)!=RESET)
	{	
		ucCh  = USART_ReceiveData( AS608_USART );
		
						/*获取写缓冲区指针，准备写入新数据*/
		data_p = cbWrite(&rx_queue); 
		
		if (data_p != NULL)	//若缓冲队列未满，开始传输
		{		

			//往缓冲区写入数据，如使用串口接收、dma写入等方式
			*(data_p->head + data_p->len) = ucCh;
				
			if( ++data_p->len >= QUEUE_NODE_DATA_LEN)
			{
				cbWriteFinish(&rx_queue);
			}
		}else return;	
	}
	
	if ( USART_GetITStatus( AS608_USART, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
	{
			/*写入缓冲区完毕*/
			cbWriteFinish(&rx_queue);
		ucCh = USART_ReceiveData( AS608_USART );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)

	}
}	
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
