@/*
@*********************************************************************************************************
@*                                              EXAMPLE CODE
@*
@*                             (c) Copyright 2011; Micrium, Inc.; Weston, FL
@*
@*                   All rights reserved.  Protected by international copyright laws.
@*                   Knowledge of the source code may not be used to write a similar
@*                   product.  This file may only be used in accordance with a license
@*                   and should not be redistributed in any way.
@*********************************************************************************************************
@*/


@/*
@*********************************************************************************************************
@*
@*                                         EXCEPTION VECTORS
@*
@*                                     ST Microelectronics STM32L
@*                                              on the
@*
@*                                          STM32L152-EVAL
@*                                         Evaluation Board
@*
@* Filename      : cstartup.c
@* Version       : V1.00
@* Programmer(s) : FT
@*                 FF
@*				   DC
@*********************************************************************************************************
@*/


@/*
@********************************************************************************************************
@*                                              STACK DEFINITIONS
@*
@* Configuration Wizard Menu:
@* // <h> Stack Configuration
@* //   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
@* // </h>;
@*********************************************************************************************************
@*/

  .syntax unified
    .cpu cortex-m3
    .fpu softvfp
    .thumb

.global g_pfnVectors
.global Default_Handler

@ Start address for the initialization values of the .data section.
.word   _sidata

@ Start address for the .data section.
.word   _sdata

@ End address for the .data section.
.word   _edata

@ Start address for the .bss section.
.word   _sbss

@ End address for the .bss section.
.word   _ebss

@ NOTE: All previous words are defined in the Linker Script.


.equ  BootRAM, 0xF108F85F
@/*
@**********************************************************************************************************
@ Description:      This is the code that gets called when the processor first
@                   starts execution following a reset event. Only the absolutely
@                   necessary set is performed, after which the application
@                   supplied main() routine is called.
@ Parameters:  None
@
@ Return Value: None
@**********************************************************************************************************
@*/

@ Reset Handler
    .section    .text.Reset_Handler
    .weak   Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:

@ Copy the data segment initializers from flash to SRAM.
  movs  r1, #0
  b LoopCopyDataInit

CopyDataInit:
    ldr r3, =_sidata
    ldr r3, [r3, r1]
    str r3, [r0, r1]
    adds    r1, r1, #4

LoopCopyDataInit:
    ldr r0, =_sdata
    ldr r3, =_edata
    adds    r2, r0, r1
    cmp r2, r3
    bcc CopyDataInit
    ldr r2, =_sbss
    b   LoopFillZerobss


FillZerobss:                                            @ Zero fill the bss segment.
    movs    r3, #0
    str r3, [r2], #4

LoopFillZerobss:
    ldr r3, = _ebss
    cmp r2, r3
    bcc FillZerobss

@ Call the clock system intitialization function.
    @bl  SystemInit

    bl __libc_init_array                                @ Call static constructors

@ Call the application's entry point.
    bl  main
    bx  lr
.size   Reset_Handler, .-Reset_Handler



@/*
@*********************************************************************************************************
@*                                  EXCEPTION / INTERRUPT VECTOR TABLE
@*
@* Note(s) : (1) The Cortex-M3 may have up to 256 external interrupts, which are the final entries in the
@*               vector table.  The STM32L1xxx has 81 external interrupt vectors.
@*
@*           (2) Interrupts vector 2-13 are implemented in this file as infinite loop for debuging
@*               purposes only. The application might implement a recover procedure if it is needed.
@*
@*           (3) OS_CPU_PendSVHandler() and OS_CPU_SysTickHandler() are implemented in the generic OS
@*               port.
@*********************************************************************************************************
@*/

    .section    .isr_vector,"a",%progbits
    .type   g_pfnVectors, %object
    .size   g_pfnVectors, .-g_pfnVectors

    
                                                                @ Vector Table Mapped to Address 0 at Reset
                    .extern  BSP_IntHandlerWWDG
                    .extern  BSP_IntHandlerPVD
                    .extern  BSP_IntHandlerTAMP_STAMP
                    .extern  BSP_IntHandlerRTC_WKUP
                    .extern  BSP_IntHandlerFLASH
                    .extern  BSP_IntHandlerRCC
                    .extern  BSP_IntHandlerEXTI0
                    .extern  BSP_IntHandlerEXTI1
                    .extern  BSP_IntHandlerEXTI2
                    .extern  BSP_IntHandlerEXTI3
                    .extern  BSP_IntHandlerEXTI4
                    .extern  BSP_IntHandlerDMA1_CH1
                    .extern  BSP_IntHandlerDMA1_CH2
                    .extern  BSP_IntHandlerDMA1_CH3
                    .extern  BSP_IntHandlerDMA1_CH4
                    .extern  BSP_IntHandlerDMA1_CH5
                    .extern  BSP_IntHandlerDMA1_CH6
                    .extern  BSP_IntHandlerDMA1_CH7
                    .extern  BSP_IntHandlerADC1
                    .extern  BSP_IntHandlerUSB_HP
                    .extern  BSP_IntHandlerUSB_LP
                    .extern  BSP_IntHandlerDAC
                    .extern  BSP_IntHandlerCOMP

                    .extern  BSP_IntHandlerEXTI9_5
                    .extern  BSP_IntHandlerLCD
                    .extern  BSP_IntHandlerTIM9
                    .extern  BSP_IntHandlerTIM10
                    .extern  BSP_IntHandlerTIM11
                    .extern  BSP_IntHandlerTIM2
                    .extern  BSP_IntHandlerTIM3
                    .extern  BSP_IntHandlerTIM4
                    .extern  BSP_IntHandlerI2C1_EV
                    .extern  BSP_IntHandlerI2C1_ER
                    .extern  BSP_IntHandlerI2C2_EV
                    .extern  BSP_IntHandlerI2C2_ER
                    .extern  BSP_IntHandlerSPI1
                    .extern  BSP_IntHandlerSPI2
                    .extern  BSP_IntHandlerUSART1
                    .extern  BSP_IntHandlerUSART2
                    .extern  BSP_IntHandlerUSART3
                    .extern  BSP_IntHandlerEXTI15_10
                    .extern  BSP_IntHandlerRTCAlarm
                    .extern  BSP_IntHandlerUSB_FS_WKUP
                    .extern  BSP_IntHandlerTIM6
                    .extern  BSP_IntHandlerTIM7

                    .extern  BSP_IntHandlerSDIO
                    .extern  BSP_IntHandlerTIM5
                    .extern  BSP_IntHandlerSPI3
                    .extern  BSP_IntHandlerUART4
                    .extern  BSP_IntHandlerUART5
                    .extern  BSP_IntHandlerDMA2_CH1
                    .extern  BSP_IntHandlerDMA2_CH2
                    .extern  BSP_IntHandlerDMA2_CH3
                    .extern  BSP_IntHandlerDMA2_CH4
                    .extern  BSP_IntHandlerDMA2_CH5
                    .extern  BSP_IntHandlerAES
                    .extern  BSP_IntHandlerCOMP_ACQ

                    .extern  OS_CPU_PendSVHandler
                    .extern  OS_CPU_SysTickHandler


g_pfnVectors:                                               @/* External Interrupts                                        */
                    .word     _estack                           @/*  Top of Stack                                          */
                    .word     Reset_Handler                     @/*  Reset Handler                                         */
                    .word     App_NMI_ISR                       @/*  2, NMI.                                               */
                    .word     App_Fault_ISR                     @/*  3, Hard Fault.                                        */
                    .word     App_MemFault_ISR                  @/*  4, Memory Management.                                 */
                    .word     App_BusFault_ISR                  @/*  5, Bus Fault.                                         */
                    .word     App_UsageFault_ISR                @/*  6, Usage Fault.                                       */
                    .word     App_Spurious_ISR                  @/*  7, Reserved.                                          */
                    .word     App_Spurious_ISR                  @/*  8, Reserved.                                          */
                    .word     App_Spurious_ISR                  @/*  9, Reserved.                                          */
                    .word     App_Spurious_ISR                  @/* 10, Reserved.                                          */
                    .word     App_Spurious_ISR                  @/* 11, SVCall.                                            */
                    .word     App_Spurious_ISR                  @/* 12, Debug Monitor.                                     */
                    .word     App_Spurious_ISR                  @/* 13, Reserved.                                          */
                    .word     OS_CPU_PendSVHandler              @/* 14, PendSV Handler.                                    */
                    .word     OS_CPU_SysTickHandler             @/* 15, uC/OS-III Tick ISR Handler.                        */

                    .word     BSP_IntHandlerWWDG                @/* 16, INTISR[  0]  Window Watchdog.                      */
                    .word     BSP_IntHandlerPVD                 @/* 17, INTISR[  1]  PVD through EXTI Line Detection.      */
                    .word     BSP_IntHandlerTAMP_STAMP          @/* 18, INTISR[  2]  Tamper and TimeStamp Interrupt.       */
                    .word     BSP_IntHandlerRTC_WKUP            @/* 19, INTISR[  3]  RTC Wakeup Global Interrupt.          */
                    .word     BSP_IntHandlerFLASH               @/* 20, INTISR[  4]  FLASH Global Interrupt.               */
                    .word     BSP_IntHandlerRCC                 @/* 21, INTISR[  5]  RCC Global Interrupt.                 */
                    .word     BSP_IntHandlerEXTI0               @/* 22, INTISR[  6]  EXTI Line0 Interrupt.                 */
                    .word     BSP_IntHandlerEXTI1               @/* 23, INTISR[  7]  EXTI Line1 Interrupt.                 */
                    .word     BSP_IntHandlerEXTI2               @/* 24, INTISR[  8]  EXTI Line2 Interrupt.                 */
                    .word     BSP_IntHandlerEXTI3               @/* 25, INTISR[  9]  EXTI Line3 Interrupt.                 */
                    .word     BSP_IntHandlerEXTI4               @/* 26, INTISR[ 10]  EXTI Line4 Interrupt.                 */
                    .word     BSP_IntHandlerDMA1_CH1            @/* 27, INTISR[ 11]  DMA Channel1 Global Interrupt.        */
                    .word     BSP_IntHandlerDMA1_CH2            @/* 28, INTISR[ 12]  DMA Channel2 Global Interrupt.        */
                    .word     BSP_IntHandlerDMA1_CH3            @/* 29, INTISR[ 13]  DMA Channel3 Global Interrupt.        */
                    .word     BSP_IntHandlerDMA1_CH4            @/* 30, INTISR[ 14]  DMA Channel4 Global Interrupt.        */
                    .word     BSP_IntHandlerDMA1_CH5            @/* 31, INTISR[ 15]  DMA Channel5 Global Interrupt.        */
                    .word     BSP_IntHandlerDMA1_CH6            @/* 32, INTISR[ 16]  DMA Channel6 Global Interrupt.        */
                    .word     BSP_IntHandlerDMA1_CH7            @/* 33, INTISR[ 17]  DMA Channel7 Global Interrupt.        */
                    .word     BSP_IntHandlerADC1                @/* 34, INTISR[ 18]  ADC1 Global Interrupt.                */
                    .word     BSP_IntHandlerUSB_HP              @/* 35, INTISR[ 19]  USB High priority interrupt           */
                    .word     BSP_IntHandlerUSB_LP              @/* 36, INTISR[ 20]  USB Low priority interrupt            */
                    .word     BSP_IntHandlerDAC                 @/* 37, INTISR[ 21]  DAC Interrupt                         */
                    .word     BSP_IntHandlerCOMP                @/* 38, INTISR[ 22]  Comparator wakeup through ...         */
                                                                @/* ...              EXTI line (21 & 22) int.              */
                    .word     BSP_IntHandlerEXTI9_5             @/* 39, INTISR[ 23]  EXTI Line[9:5] Interrupt.             */
                    .word     BSP_IntHandlerLCD                 @/* 40, INTISR[ 24]  LCD global interrupt                  */
                    .word     BSP_IntHandlerTIM9                @/* 41, INTISR[ 25]  TIM9 Global Interrupt.                */
                    .word     BSP_IntHandlerTIM10               @/* 42, INTISR[ 26]  TIM10 Global Interrupt.               */
                    .word     BSP_IntHandlerTIM11               @/* 43, INTISR[ 27]  TIM11 Global Interrupt.               */
                    .word     BSP_IntHandlerTIM2                @/* 44, INTISR[ 28]  TIM2 Global Interrupt.                */
                    .word     BSP_IntHandlerTIM3                @/* 45, INTISR[ 29]  TIM3 Global Interrupt.                */
                    .word     BSP_IntHandlerTIM4                @/* 46, INTISR[ 30]  TIM4 Global Interrupt.                */
                    .word     BSP_IntHandlerI2C1_EV             @/* 47, INTISR[ 31]  I2C1 Event  Interrupt.                */
                    .word     BSP_IntHandlerI2C1_ER             @/* 48, INTISR[ 32]  I2C1 Error  Interrupt.                */
                    .word     BSP_IntHandlerI2C2_EV             @/* 49, INTISR[ 33]  I2C2 Event  Interrupt.                */
                    .word     BSP_IntHandlerI2C2_ER             @/* 50, INTISR[ 34]  I2C2 Error  Interrupt.                */
                    .word     BSP_IntHandlerSPI1                @/* 51, INTISR[ 35]  SPI1 Global Interrupt.                */
                    .word     BSP_IntHandlerSPI2                @/* 52, INTISR[ 36]  SPI2 Global Interrupt.                */
                    .word     BSP_IntHandlerUSART1              @/* 53, INTISR[ 37]  USART1 Global Interrupt.              */
                    .word     BSP_IntHandlerUSART2              @/* 54, INTISR[ 38]  USART2 Global Interrupt.              */
                    .word     BSP_IntHandlerUSART3              @/* 55, INTISR[ 39]  USART3 Global Interrupt.              */
                    .word     BSP_IntHandlerEXTI15_10           @/* 56, INTISR[ 40]  EXTI Line [15:10] Interrupts.         */
                    .word     BSP_IntHandlerRTCAlarm            @/* 57, INTISR[ 41]  RTC Alarm EXT Line Interrupt.         */
                    .word     BSP_IntHandlerUSB_FS_WKUP         @/* 58, INTISR[ 42]  USBD FS Wakeup through EXTI line int. */
                    .word     BSP_IntHandlerTIM6                @/* 59, INTISR[ 43]  TIM6 Global Interrupt.                */
                    .word     BSP_IntHandlerTIM7                @/* 60, INTISR[ 44]  TIM7 Global Interrupt.                */

                    .word     BSP_IntHandlerSDIO                @/* 61, INTISR[ 45]  SDIO Global Interrupt.                */
                    .word     BSP_IntHandlerTIM5                @/* 62, INTISR[ 46]  TIM5 Global Interrupt.                */
                    .word     BSP_IntHandlerSPI3                @/* 63, INTISR[ 47]  SPI3 Global Interrupt.                */
                    .word     BSP_IntHandlerUART4               @/* 64, INTISR[ 48]  UART4 Global Interrupt.               */
                    .word     BSP_IntHandlerUART5               @/* 65, INTISR[ 49]  UART5 Global Interrupt.               */
                    .word     BSP_IntHandlerDMA2_CH1            @/* 66, INTISR[ 50]  DMA2 Channel 1 Interrupt.             */
                    .word     BSP_IntHandlerDMA2_CH2            @/* 67, INTISR[ 51]  DMA2 Channel 2 Interrupt.             */
                    .word     BSP_IntHandlerDMA2_CH3            @/* 68, INTISR[ 52]  DMA2 Channel 3 Interrupt.             */
                    .word     BSP_IntHandlerDMA2_CH4            @/* 69, INTISR[ 53]  DMA2 Channel 4 Interrupt.             */
                    .word     BSP_IntHandlerDMA2_CH5            @/* 70, INTISR[ 54]  DMA2 Channel 5 Interrupt.             */
                    .word     BSP_IntHandlerAES                 @/* 71, INTISR[ 55]  AES Global Interrupt.                 */
                    .word     BSP_IntHandlerCOMP_ACQ            @/* 72, INTISR[ 56]  Comparator Channel Aquisition Int.    */

@/*
@                    .word 0
@                    .word 0
@                    .word 0
@                    .word 0
@                    .word 0
@                    .word BootRAM                               @/*  0x108. This is for boot in RAM mode for               */
                                                                @/*  STM32L15x ULtra Low Power Medium-density devices.     */

@*/
@/*
@*********************************************************************************************************
@ Provide weak aliases for each Exception handler to the Default_Handler.
@ As they are weak aliases, any function with the same name will override
@ this definition.
@
@*********************************************************************************************************
@*/


    .section    .text.App_NMI_ISR
App_NMI_ISR:
    b   App_NMI_ISR
    .size   App_NMI_ISR, .-App_NMI_ISR


    .section    .text.App_Fault_ISR
App_Fault_ISR:
    b   App_Fault_ISR
    .size   App_Fault_ISR, .-App_Fault_ISR


    .section    .text.App_MemFault_ISR
App_MemFault_ISR:
    b   App_MemFault_ISR
    .size   App_MemFault_ISR, .-App_MemFault_ISR


    .section    .text.App_BusFault_ISR
App_BusFault_ISR:
    b   App_BusFault_ISR
    .size   App_BusFault_ISR, .-App_BusFault_ISR


    .section    .text.App_UsageFault_ISR
App_UsageFault_ISR:
    b   App_UsageFault_ISR
    .size   App_UsageFault_ISR, .-App_UsageFault_ISR

    .section    .text.App_Spurious_ISR
App_Spurious_ISR:
    b   App_Spurious_ISR
    .size   App_Spurious_ISR, .-App_Spurious_ISR


    .section    .text.App_Reserved_ISR
App_Reserved_ISR:
    b   App_Reserved_ISR
    .size   App_Reserved_ISR, .-App_Reserved_ISR

