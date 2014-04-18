/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2011; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32L
*                                              on the
*
*                                          STM32L152-EVAL
*                                         Evaluation Board
*
* Filename      : bsp_int.c
* Version       : V1.00
* Programmer(s) : FF
*		          DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_INT_MODULE
#include  <cpu.h>
#include  <bsp_os.h>
#include  <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_SRC_NBR                                 57


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_IntHandler     (CPU_DATA  int_id);
static  void  BSP_IntHandlerDummy(void); //若有static则只有本文件函数可调用这个函数，其他文件函数不可访问


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntClr()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id      Interrupt to clear.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClr (CPU_DATA  int_id)
{

}


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcDis(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcEn(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (CPU_DATA    int_id,
                      CPU_INT08U  prio)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        CPU_IntSrcPrioSet(int_id + 16, prio);
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandlerWWDG               (void)  { BSP_IntHandler(BSP_INT_ID_WWDG);                }
void  BSP_IntHandlerPVD                (void)  { BSP_IntHandler(BSP_INT_ID_PVD);                 }
void  BSP_IntHandlerTAMP_STAMP         (void)  { BSP_IntHandler(BSP_INT_ID_TAMP_STAMP);          }
void  BSP_IntHandlerRTC_WKUP           (void)  { BSP_IntHandler(BSP_INT_ID_RTC_WKUP);            }
void  BSP_IntHandlerFLASH              (void)  { BSP_IntHandler(BSP_INT_ID_FLASH);               }
void  BSP_IntHandlerRCC                (void)  { BSP_IntHandler(BSP_INT_ID_RCC);                 }
void  BSP_IntHandlerEXTI0              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI0);               }
void  BSP_IntHandlerEXTI1              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI1);               }
void  BSP_IntHandlerEXTI2              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI2);               }
void  BSP_IntHandlerEXTI3              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI3);               }
void  BSP_IntHandlerEXTI4              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI4);               }
void  BSP_IntHandlerDMA1_CH1           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH1);            }
void  BSP_IntHandlerDMA1_CH2           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH2);            }
void  BSP_IntHandlerDMA1_CH3           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH3);            }
void  BSP_IntHandlerDMA1_CH4           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH4);            }
void  BSP_IntHandlerDMA1_CH5           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH5);            }
void  BSP_IntHandlerDMA1_CH6           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH6);            }
void  BSP_IntHandlerDMA1_CH7           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH1);            }
void  BSP_IntHandlerADC1               (void)  { BSP_IntHandler(BSP_INT_ID_ADC1);                }
void  BSP_IntHandlerUSB_HP             (void)  { BSP_IntHandler(BSP_INT_ID_USB_HP);              }
void  BSP_IntHandlerUSB_LP             (void)  { BSP_IntHandler(BSP_INT_ID_USB_LP);              }
void  BSP_IntHandlerDAC                (void)  { BSP_IntHandler(BSP_INT_ID_DAC);                 }
void  BSP_IntHandlerCOMP               (void)  { BSP_IntHandler(BSP_INT_ID_COMP);                }
void  BSP_IntHandlerEXTI9_5            (void)  { BSP_IntHandler(BSP_INT_ID_EXTI9_5);             }
void  BSP_IntHandlerLCD                (void)  { BSP_IntHandler(BSP_INT_ID_LCD);                 }
void  BSP_IntHandlerTIM9               (void)  { BSP_IntHandler(BSP_INT_ID_TIM9);                }
void  BSP_IntHandlerTIM10              (void)  { BSP_IntHandler(BSP_INT_ID_TIM10);               }
void  BSP_IntHandlerTIM11              (void)  { BSP_IntHandler(BSP_INT_ID_TIM11);               }
void  BSP_IntHandlerTIM2               (void)  { BSP_IntHandler(BSP_INT_ID_TIM2);                }
void  BSP_IntHandlerTIM3               (void)  { BSP_IntHandler(BSP_INT_ID_TIM3);                }
void  BSP_IntHandlerTIM4               (void)  { BSP_IntHandler(BSP_INT_ID_TIM4);                }
void  BSP_IntHandlerI2C1_EV            (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_EV);             }
void  BSP_IntHandlerI2C1_ER            (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_ER);             }
void  BSP_IntHandlerI2C2_EV            (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_EV);             }
void  BSP_IntHandlerI2C2_ER            (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_ER);             }
void  BSP_IntHandlerSPI1               (void)  { BSP_IntHandler(BSP_INT_ID_SPI1);                }
void  BSP_IntHandlerSPI2               (void)  { BSP_IntHandler(BSP_INT_ID_SPI2);                }
void  BSP_IntHandlerUSART1             (void)  { BSP_IntHandler(BSP_INT_ID_USART1);              }
void  BSP_IntHandlerUSART2             (void)  { BSP_IntHandler(BSP_INT_ID_USART2);              }
void  BSP_IntHandlerUSART3             (void)  { BSP_IntHandler(BSP_INT_ID_USART3);              }
void  BSP_IntHandlerEXTI15_10          (void)  { BSP_IntHandler(BSP_INT_ID_EXTI15_10);           }
void  BSP_IntHandlerRTCAlarm           (void)  { BSP_IntHandler(BSP_INT_ID_RTC_ALARM);           }
void  BSP_IntHandlerUSB_FS_WKUP        (void)  { BSP_IntHandler(BSP_INT_ID_USB_FS_WKUP);         }
void  BSP_IntHandlerTIM6               (void)  { BSP_IntHandler(BSP_INT_ID_TIM6);                }
void  BSP_IntHandlerTIM7               (void)  { BSP_IntHandler(BSP_INT_ID_TIM7);                }

void  BSP_IntHandlerSDIO               (void)  { BSP_IntHandler(BSP_INT_ID_SDIO);                }
void  BSP_IntHandlerTIM5               (void)  { BSP_IntHandler(BSP_INT_ID_TIM5);                }
void  BSP_IntHandlerSPI3               (void)  { BSP_IntHandler(BSP_INT_ID_SPI3);                }
void  BSP_IntHandlerUART4              (void)  { BSP_IntHandler(BSP_INT_ID_UART4);               }
void  BSP_IntHandlerUART5              (void)  { BSP_IntHandler(BSP_INT_ID_UART5);               }
void  BSP_IntHandlerDMA2_CH1           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH1);            }
void  BSP_IntHandlerDMA2_CH2           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH2);            }
void  BSP_IntHandlerDMA2_CH3           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH3);            }
void  BSP_IntHandlerDMA2_CH4           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH4);            }
void  BSP_IntHandlerDMA2_CH5           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH5);            }
void  BSP_IntHandlerAES                (void)  { BSP_IntHandler(BSP_INT_ID_AES);                 }
void  BSP_IntHandlerCOMP_ACQ           (void)  { BSP_IntHandler(BSP_INT_ID_COMP_ACQ);            }

/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandler (CPU_DATA  int_id)
{
    CPU_FNCT_VOID  isr;
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();                                       /* Tell the OS that we are starting an ISR            */

    OSIntEnter();

    CPU_CRITICAL_EXIT();

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

    OSIntExit();                                                /* Tell the OS that we are leaving the ISR            */
}


/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{

}

/*
*********************************************************************************************************
*                                        BSP_IntHandler_EXTI0_PA()
*
* Description : EXTI0_PA handler
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandler_EXTI0_PA0 (void)
{
BSP_LED_Off(2);
EXTI_ClearITPendingBit(EXTI_Line0);//清除EXTI_PR外部中断挂起标志
NVIC_ClearPendingIRQ(EXTI0_IRQn);//清除IPR    NVIC中断挂起标志
}
