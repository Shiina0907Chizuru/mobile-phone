#ifndef __BSP_AS608_H
#define	__BSP_AS608_H


#include "stm32f4xx.h"
#include <stdio.h>
#include "delay.h"
#include "bsp_debug_usart.h"
#include "rx_data_queue.h"

#define CHAR_BUFFER1 0x01
#define CHAR_BUFFER2 0x02

/*����������ʱ*/
#define AS608_DELAY_MS(x) delay_ms(x)

#define PS_MAXNUM   240           /*ָ��ģ�����洢ָ��ģ����*/
/** 
  * ���ں궨�壬��ͬ�Ĵ��ڹ��ص����ߺ�IO��һ������ֲʱ��Ҫ�޸��⼸����
	* 1-�޸�����ʱ�ӵĺ꣬uart1���ص�apb2���ߣ�����uart���ص�apb1����
	* 2-�޸�GPIO�ĺ�
  */
  
  
  
/*AS608_USART���Ŷ���*/
/*******************************************************/
#define AS608_USART                             UART5
#define AS608_USART_CLK                         RCC_APB1Periph_UART5
#define AS608_USART_BAUDRATE                    57600               //���ڲ�����

#define AS608_USART_RX_GPIO_PORT                GPIOD
#define AS608_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define AS608_USART_RX_PIN                      GPIO_Pin_2
#define AS608_USART_RX_AF                       GPIO_AF_UART5
#define AS608_USART_RX_SOURCE                   GPIO_PinSource2

#define AS608_USART_TX_GPIO_PORT                GPIOC
#define AS608_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define AS608_USART_TX_PIN                      GPIO_Pin_12
#define AS608_USART_TX_AF                       GPIO_AF_UART5
#define AS608_USART_TX_SOURCE                   GPIO_PinSource12

#define AS608_USART_IRQHandler                  UART5_IRQHandler
#define AS608_USART_IRQ                 				UART5_IRQn  
///*����2-USART2*/
//#define  AS608_USART                    USART2
//#define  AS608_USART_CLK                RCC_APB1Periph_USART2
//#define  AS608_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
//#define  AS608_USART_BAUDRATE           57600

///*USART GPIO ���ź궨��*/
//#define  AS608_USART_GPIO_CLK           (RCC_AHB1Periph_GPIOA)
//#define  AS608_USART_GPIO_APBxClkCmd    RCC_AHB1PeriphClockCmd
//    
//#define  AS608_USART_TX_GPIO_PORT       GPIOA   
//#define  AS608_USART_TX_GPIO_PIN        GPIO_Pin_2
//#define  AS608_USART_RX_GPIO_PORT       GPIOA
//#define  AS608_USART_RX_GPIO_PIN        GPIO_Pin_3

//#define  AS608_USART_IRQ                USART2_IRQn
//#define  AS608_USART_IRQHandler         USART2_IRQHandler

/*TouchOut���Ŷ���*/
#define AS608_TouchOut_INT_GPIO_PORT                GPIOB
#define AS608_TouchOut_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define AS608_TouchOut_INT_GPIO_PIN                 GPIO_Pin_8
#define AS608_TouchOut_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOB
#define AS608_TouchOut_INT_EXTI_PINSOURCE           EXTI_PinSource8
#define AS608_TouchOut_INT_EXTI_LINE                EXTI_Line8
#define AS608_TouchOut_INT_EXTI_IRQ                 EXTI9_5_IRQn

#define AS608_TouchOut_IRQHandler                   EXTI9_5_IRQHandler


/*��Ϣ���?*/
#define AS608_AS608_ON          0
#define AS608_AS608_ARRAY_ON		0

#define AS608_INFO(fmt,arg...)           printf("<<-AS608-INFO->> "fmt"\n",##arg)
#define AS608_ERROR(fmt,arg...)          printf("<<-AS608-ERROR->> "fmt"\n",##arg)
#define AS608_AS608(fmt,arg...)          do{\
                                          if(AS608_AS608_ON)\
                                          printf("<<-AS608-AS608->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define AS608_AS608_ARRAY(array, num)    do{\
																									 int32_t i;\
																									 uint8_t* a = array;\
																									 if(AS608_AS608_ARRAY_ON)\
																									 {\
																											printf("\n<<AS608-AS608-ARRAY->>\n");\
																											for (i = 0; i < (num); i++)\
																											{\
																													printf("%02x   ", (a)[i]);\
																													if ((i + 1 ) %10 == 0)\
																													{\
																															printf("\n");\
																													}\
																											}\
																											printf("\n");\
																									}\
																								 }while(0)		
 
extern uint32_t AS608_Addr ;       /*ָ��ʶ��ģ��Ĭ�ϵ�ַ*/ 
                                                 
void AS608_Config(void);
void ShowErrMessage( uint16_t  ensure);
uint16_t ReturnFlag( uint16_t *i);
uint16_t PS_GetImage(void);
uint16_t PS_GenChar(uint8_t BufferID);
uint16_t PS_Connect(uint32_t *PS_Addr);
uint16_t PS_Match(void);
uint16_t PS_RegModel(void);
uint32_t GET_NUM(void);
uint16_t PS_StoreChar(uint8_t BufferID,uint16_t PageID);
uint16_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,uint16_t *p);
uint16_t PS_DeletChar(uint16_t PageID,uint16_t N);
uint16_t PS_Empty(void);
 






#endif
