/*
*********************************************************************************************************
*                                              EXAMPLE CODE
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
*                                         EXCEPTION VECTORS
*
*                                     ST Microelectronics STM32L
*                                              on the
*
*                                          STM32L152-EVAL
*                                         Evaluation Board
*
* Filename      : cstartup.c
* Version       : V1.00
* Programmer(s) : FT
*                 FF
*********************************************************************************************************
*/

#include <lib_def.h>
#include <bsp.h>
#include <bsp_os.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

typedef  union {
    CPU_FNCT_VOID   Fnct;
    void           *Ptr;
} APP_INTVECT_ELEM;

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


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

#pragma language=extended
#pragma segment="CSTACK"

static  void  App_NMI_ISR         (void);

static  void  App_Fault_ISR       (void);

static  void  App_BusFault_ISR    (void);

static  void  App_UsageFault_ISR  (void);

static  void  App_MemFault_ISR    (void);

static  void  App_Spurious_ISR    (void);

extern  void  __iar_program_start (void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  EXCEPTION / INTERRUPT VECTOR TABLE
*
* Note(s) : (1) The Cortex-M3 may have up to 256 external interrupts, which are the final entries in the
*               vector table.  The STM32L1xxx has 81 external interrupt vectors.
*
*           (2) Interrupts vector 2-13 are implemented in this file as infinite loop for debuging
*               purposes only. The application might implement a recover procedure if it is needed.
*
*           (3) OS_CPU_PendSVHandler() and OS_CPU_SysTickHandler() are implemented in the generic OS
*               port.
*********************************************************************************************************
*/

__root  const  APP_INTVECT_ELEM  __vector_table[] @ ".intvec" = {
    { .Ptr = (void *)__sfe( "CSTACK" )},                        /*  0, SP start value.                                    */
    __iar_program_start,                                        /*  1, PC start value.                                    */
    App_NMI_ISR,                                                /*  2, NMI.                                               */
    App_Fault_ISR,                                              /*  3, Hard Fault.                                        */
    App_MemFault_ISR,                                           /*  4, Memory Management.                                 */
    App_BusFault_ISR,                                           /*  5, Bus Fault.                                         */
    App_UsageFault_ISR,                                         /*  6, Usage Fault.                                       */
    App_Spurious_ISR,                                           /*  7, Reserved.                                          */
    App_Spurious_ISR,                                           /*  8, Reserved.                                          */
    App_Spurious_ISR,                                           /*  9, Reserved.                                          */
    App_Spurious_ISR,                                           /* 10, Reserved.                                          */
    App_Spurious_ISR,                                           /* 11, SVCall.                                            */
    App_Spurious_ISR,                                           /* 12, Debug Monitor.                                     */
    App_Spurious_ISR,                                           /* 13, Reserved.                                          */
    OS_CPU_PendSVHandler,                                       /* 14, PendSV Handler.                                    */
    OS_CPU_SysTickHandler,                                      /* 15, uC/OS-III Tick ISR Handler.                        */

    BSP_IntHandlerWWDG,                                         /* 16, INTISR[  0]  Window Watchdog.                      */
    BSP_IntHandlerPVD,                                          /* 17, INTISR[  1]  PVD through EXTI Line Detection.      */
    BSP_IntHandlerTAMP_STAMP,                                   /* 18, INTISR[  2]  Tamper and TimeStamp Interrupt.       */
    BSP_IntHandlerRTC_WKUP,                                     /* 19, INTISR[  3]  RTC Wakeup Global Interrupt.          */
    BSP_IntHandlerFLASH,                                        /* 20, INTISR[  4]  FLASH Global Interrupt.               */
    BSP_IntHandlerRCC,                                          /* 21, INTISR[  5]  RCC Global Interrupt.                 */
    BSP_IntHandlerEXTI0,                                        /* 22, INTISR[  6]  EXTI Line0 Interrupt.                 */
    BSP_IntHandlerEXTI1,                                        /* 23, INTISR[  7]  EXTI Line1 Interrupt.                 */
    BSP_IntHandlerEXTI2,                                        /* 24, INTISR[  8]  EXTI Line2 Interrupt.                 */
    BSP_IntHandlerEXTI3,                                        /* 25, INTISR[  9]  EXTI Line3 Interrupt.                 */
    BSP_IntHandlerEXTI4,                                        /* 26, INTISR[ 10]  EXTI Line4 Interrupt.                 */
    BSP_IntHandlerDMA1_CH1,                                     /* 27, INTISR[ 11]  DMA Channel1 Global Interrupt.        */
    BSP_IntHandlerDMA1_CH2,                                     /* 28, INTISR[ 12]  DMA Channel2 Global Interrupt.        */
    BSP_IntHandlerDMA1_CH3,                                     /* 29, INTISR[ 13]  DMA Channel3 Global Interrupt.        */
    BSP_IntHandlerDMA1_CH4,                                     /* 30, INTISR[ 14]  DMA Channel4 Global Interrupt.        */
    BSP_IntHandlerDMA1_CH5,                                     /* 31, INTISR[ 15]  DMA Channel5 Global Interrupt.        */
    BSP_IntHandlerDMA1_CH6,                                     /* 32, INTISR[ 16]  DMA Channel6 Global Interrupt.        */
    BSP_IntHandlerDMA1_CH7,                                     /* 33, INTISR[ 17]  DMA Channel7 Global Interrupt.        */
    BSP_IntHandlerADC1,                                         /* 34, INTISR[ 18]  ADC1 Global Interrupt.                */
    BSP_IntHandlerUSB_HP,                                       /* 35, INTISR[ 19]  USB High priority interrupt           */
    BSP_IntHandlerUSB_LP,                                       /* 36, INTISR[ 20]  USB Low priority interrupt            */
    BSP_IntHandlerDAC,                                          /* 37, INTISR[ 21]  DAC Interrupt                         */
    BSP_IntHandlerCOMP,                                         /* 38, INTISR[ 22]  Comparator wakeup through ...         */
                                                                /* ...              EXTI line (21 & 22) int.              */
    BSP_IntHandlerEXTI9_5,                                      /* 39, INTISR[ 23]  EXTI Line[9:5] Interrupt.             */
    BSP_IntHandlerLCD,                                          /* 40, INTISR[ 24]  LCD global interrupt                  */
    BSP_IntHandlerTIM9,                                         /* 41, INTISR[ 25]  TIM9 Global Interrupt.                */
    BSP_IntHandlerTIM10,                                        /* 42, INTISR[ 26]  TIM10 Global Interrupt.               */
    BSP_IntHandlerTIM11,                                        /* 43, INTISR[ 27]  TIM11 Global Interrupt.               */
    BSP_IntHandlerTIM2,                                         /* 44, INTISR[ 28]  TIM2 Global Interrupt.                */
    BSP_IntHandlerTIM3,                                         /* 45, INTISR[ 29]  TIM3 Global Interrupt.                */
    BSP_IntHandlerTIM4,                                         /* 46, INTISR[ 30]  TIM4 Global Interrupt.                */
    BSP_IntHandlerI2C1_EV,                                      /* 47, INTISR[ 31]  I2C1 Event  Interrupt.                */
    BSP_IntHandlerI2C1_ER,                                      /* 48, INTISR[ 32]  I2C1 Error  Interrupt.                */
    BSP_IntHandlerI2C2_EV,                                      /* 49, INTISR[ 33]  I2C2 Event  Interrupt.                */
    BSP_IntHandlerI2C2_ER,                                      /* 50, INTISR[ 34]  I2C2 Error  Interrupt.                */
    BSP_IntHandlerSPI1,                                         /* 51, INTISR[ 35]  SPI1 Global Interrupt.                */
    BSP_IntHandlerSPI2,                                         /* 52, INTISR[ 36]  SPI2 Global Interrupt.                */
    BSP_IntHandlerUSART1,                                       /* 53, INTISR[ 37]  USART1 Global Interrupt.              */
    BSP_IntHandlerUSART2,                                       /* 54, INTISR[ 38]  USART2 Global Interrupt.              */
    BSP_IntHandlerUSART3,                                       /* 55, INTISR[ 39]  USART3 Global Interrupt.              */
    BSP_IntHandlerEXTI15_10,                                    /* 56, INTISR[ 40]  EXTI Line [15:10] Interrupts.         */
    BSP_IntHandlerRTCAlarm,                                     /* 57, INTISR[ 41]  RTC Alarm EXT Line Interrupt.         */
    BSP_IntHandlerUSB_FS_WKUP,                                  /* 58, INTISR[ 42]  USBD FS Wakeup through EXTI line int. */
    BSP_IntHandlerTIM6,                                         /* 59, INTISR[ 43]  TIM6 Global Interrupt.                */
    BSP_IntHandlerTIM7,                                         /* 60, INTISR[ 44]  TIM7 Global Interrupt.                */

    BSP_IntHandlerSDIO,                                         /* 61, INTISR[ 45]  SDIO Global Interrupt.                */
    BSP_IntHandlerTIM5,                                         /* 62, INTISR[ 46]  TIM5 Global Interrupt.                */
    BSP_IntHandlerSPI3,                                         /* 63, INTISR[ 47]  SPI3 Global Interrupt.                */
    BSP_IntHandlerUART4,                                        /* 64, INTISR[ 48]  UART4 Global Interrupt.               */
    BSP_IntHandlerUART5,                                        /* 65, INTISR[ 49]  UART5 Global Interrupt.               */
    BSP_IntHandlerDMA2_CH1,                                     /* 66, INTISR[ 50]  DMA2 Channel 1 Interrupt.             */
    BSP_IntHandlerDMA2_CH2,                                     /* 67, INTISR[ 51]  DMA2 Channel 2 Interrupt.             */
    BSP_IntHandlerDMA2_CH3,                                     /* 68, INTISR[ 52]  DMA2 Channel 3 Interrupt.             */
    BSP_IntHandlerDMA2_CH4,                                     /* 69, INTISR[ 53]  DMA2 Channel 4 Interrupt.             */
    BSP_IntHandlerDMA2_CH5,                                     /* 70, INTISR[ 54]  DMA2 Channel 5 Interrupt.             */
    BSP_IntHandlerAES,                                          /* 71, INTISR[ 55]  AES Global Interrupt.                 */
    BSP_IntHandlerCOMP_ACQ,                                     /* 72, INTISR[ 56]  Comparator Channel Aquisition Int.    */
};


/*
*********************************************************************************************************
*                                            App_NMI_ISR()
*
* Description : Handle Non-Maskable Interrupt (NMI).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : (1) Since the NMI is not being used, this serves merely as a catch for a spurious
*                   exception.
*********************************************************************************************************
*/

static  void  App_NMI_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                             App_Fault_ISR()
*
* Description : Handle hard fault.
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

static  void  App_Fault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_BusFault_ISR()
*
* Description : Handle bus fault.
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

static  void  App_BusFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                          App_UsageFault_ISR()
*
* Description : Handle usage fault.
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

static  void  App_UsageFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_MemFault_ISR()
*
* Description : Handle memory fault.
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

static  void  App_MemFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_Spurious_ISR()
*
* Description : Handle spurious interrupt.
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

static  void  App_Spurious_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}
