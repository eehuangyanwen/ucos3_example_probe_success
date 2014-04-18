/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
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
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FF
*		  DC
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   BSP_MODULE
#include  <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define  LED_BLUE                                     0x0040u
#define  LED_GREEN                                   0x0080u
#define  BSP_GPIOB_LED1                        DEF_BIT_00
#define  BSP_GPIOB_LED2                        DEF_BIT_01
#define  BSP_USER_BOTTOM                      0x0001u


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


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  BSP_REG_DEM_CR                       (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                       (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                   (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                    (*(CPU_REG32 *)0xE0042004)


/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  BSP_DBGMCU_CR_TRACE_IOEN_MASK                   0x10
#define  BSP_DBGMCU_CR_TRACE_MODE_ASYNC                  0x00
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_01                0x40
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_02                0x80
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04                0xC0
#define  BSP_DBGMCU_CR_TRACE_MODE_MASK                   0xC0

#define  BSP_BIT_DEM_CR_TRCENA                    DEF_BIT_24

#define  BSP_BIT_DWT_CR_CYCCNTENA                 DEF_BIT_00


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_LED_Init        (void);
static  void  BSP_BOTTOM_Init    (void);



/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*
*               (2) CPU instruction / data tracing requires the use of the following pins :
*                   (a) (1) Aysynchronous     :  PB[3]
*                       (2) Synchronous 1-bit :  PE[3:2]
*                       (3) Synchronous 2-bit :  PE[4:2]
*                       (4) Synchronous 4-bit :  PE[6:2]
*
*                   (c) The application may wish to adjust the trace bus width depending on I/O
*                       requirements.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    BSP_IntInit();

    RCC_DeInit();

    RCC_HSEConfig(RCC_HSE_ON);                                  /* Enable HSE Clock                                     */
#ifdef origin
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) {       /* Wait till HSE is ready                               */
        ;
    }
#endif
    FLASH_ReadAccess64Cmd(ENABLE);                              /* Enable 64-bit access                                 */
    FLASH_PrefetchBufferCmd(ENABLE);                            /* Enable Prefetch Buffer                               */
    FLASH_SetLatency(FLASH_Latency_1);                          /* Flash 1 wait state                                   */

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);         /* Enable the PWR APB1 Clock                            */
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);        /* Select the Voltage Range 1 (1.8V)                    */
    while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) {
        ;
    }
    RCC_HCLKConfig(RCC_SYSCLK_Div1);                            /* HCLK = SYSCLK = AHBCLK               = 32MHz.        */
    RCC_PCLK2Config(RCC_HCLK_Div1);                             /* APB2CLK = AHBCLK  / APB2DIV(1)       = 32MHz.        */
    RCC_PCLK1Config(RCC_HCLK_Div1);                             /* APB1CLK = AHBCLK  / APB1DIV(1)       = 32MHz.        */

                                                                /* PLLCLK = (HSE * PLLMUL) / PLLDIV     = 32Mhz.        */
    RCC_PLLConfig(RCC_PLLSource_HSE,
                  RCC_PLLMul_12,
                  RCC_PLLDiv_3);

    RCC_PLLCmd(ENABLE);                                         /* Enable PLL                                           */
#ifdef origin
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {       /* Wait for PLL to lock.                                */
       ;
   }
#endif
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                  /* Select PLL as system clock source                    */
#ifdef origin
    while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL) {         /* Wait till PLL is used as system clock source         */
       ;
   }
#endif
    
    BSP_LED_Init();                                             /* Init LEDs.                                           */
    BSP_BOTTOM_Init();                                         /*Init the user bottom                              */
#ifdef TRACE_EN                                                 /* See project / compiler preprocessor options.         */
    BSP_CPU_REG_DBGMCU_CR |=  BSP_DBGMCU_CR_TRACE_IOEN_MASK;    /* Enable tracing (see Note #2).                        */
    BSP_CPU_REG_DBGMCU_CR &= ~BSP_DBGMCU_CR_TRACE_MODE_MASK;    /* Clr trace mode sel bits.                             */
    BSP_CPU_REG_DBGMCU_CR |=  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04; /* Cfg trace mode to synch 4-bit.                       */
#endif
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}


/*
*********************************************************************************************************
*                                           BSP_LED_Init()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    inialize ALL  LEDs
*                       1    inialize user LED1
*                       2    inialize user LED2
*                       3    inialize user LED3
*                       4    inialize user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : LED3 and LED4 are disabled by default; JP18 & JP19 must be closed to enable these LEDs.
*               See User Manual to disable JP18 & JP19.
*********************************************************************************************************
*/

static void  BSP_LED_Init()
{
    GPIO_InitTypeDef  gpio_init;


 //   BSP_PeriphEn(BSP_PERIPH_ID_GPIOD);                          /* Configure GPIOG for LED1 and LED2                    */
    BSP_PeriphEn(BSP_PERIPH_ID_GPIOB);                          /* Configure GPIOG for LED1 and LED2                    */
    gpio_init.GPIO_Pin   = LED_BLUE | LED_GREEN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_40MHz;

    GPIO_Init(GPIOB, &gpio_init);

}


/*
*********************************************************************************************************
*                                              BSP_LED_Off()
*
* Description : Turn OFF any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns OFF ALL the LEDs
*                       1    turns OFF user LED1
*                       2    turns OFF user LED2
*                       3    turns OFF user LED3
*                       4    turns OFF user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : LEDs on this board are inversly controlled. Thus Setting them turns them off.
*********************************************************************************************************
*/

void  BSP_LED_Off (CPU_INT08U  led)
{
    switch (led) {
        case 0:
             GPIO_SetBits(GPIOB, (LED_BLUE | LED_GREEN));
             break;


        case 1:
             GPIO_SetBits(GPIOB, LED_BLUE);
             break;


        case 2:
             GPIO_SetBits(GPIOB, LED_GREEN);
             break;


        case 3:
        case 4:
        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turn ON any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns ON ALL  LEDs
*                       1    turns ON user LED1
*                       2    turns ON user LED2
*                       3    turns ON user LED3
*                       4    turns ON user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : LEDs on this board are inversly controlled. Thus Resetting them turns them on.
*********************************************************************************************************
*/

void  BSP_LED_On (CPU_INT08U led)
{
    switch (led) {
        case 0:
             GPIO_ResetBits(GPIOB, (LED_BLUE | LED_GREEN));
             break;


        case 1:
             GPIO_ResetBits(GPIOB, LED_BLUE);
             break;


        case 2:
             GPIO_ResetBits(GPIOB, LED_GREEN);
             break;


        case 3:
        case 4:
        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                            BSP_LED_Toggle()
*
* Description : TOGGLE any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    TOGGLE ALL the LEDs
*                       1    TOGGLE user LED1
*                       2    TOGGLE user LED2
*                       3    TOGGLE user LED3
*                       4    TOGGLE user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT08U  led)
{
    CPU_INT32U  pins;


    switch (led) {
        case 0:
             pins  =  GPIO_ReadOutputData(GPIOB);
             pins ^= (LED_BLUE | LED_GREEN);
             //GPIO_SetBits(  GPIOB,   pins  & (LED_BLUE | LED_GREEN));
			 //GPIO_SetBits(  GPIOB,   LED_BLUE | LED_GREEN);
			   GPIO_SetBits(  GPIOB,   LED_BLUE );
             //GPIO_ResetBits(GPIOB, (~pins) & (LED_BLUE | LED_GREEN));
             break;


        case 1:
             pins  =  GPIO_ReadOutputData(GPIOB);
             pins ^= BSP_GPIOB_LED1;
             GPIO_SetBits(  GPIOB,   pins  & LED_BLUE);
             GPIO_ResetBits(GPIOB, (~pins) & LED_BLUE);
             break;


        case 2:
             pins  =  GPIO_ReadOutputData(GPIOB);
             pins ^= BSP_GPIOB_LED2;
             GPIO_SetBits(  GPIOB,   pins  & LED_GREEN);
             GPIO_ResetBits(GPIOB, (~pins) & LED_GREEN);
             break;


        case 3:
        case 4:
        default:
             break;
    }
}
/*
*********************************************************************************************************
*                                           BSP_BOTTOM_Init()
*
* Description : Initialize any or all the buttoms on the board.
*
* Argument(s) : void
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : the buttom connect to GPA0.
*********************************************************************************************************
*/

void BSP_BOTTOM_Init(void)
{
	
    GPIO_InitTypeDef  gpio_init;


 //   BSP_PeriphEn(BSP_PERIPH_ID_GPIOD);                          /* Configure GPIOA for USER BUTTOM                 */
    BSP_PeriphEn(BSP_PERIPH_ID_GPIOA);                          
    gpio_init.GPIO_Pin   = BSP_USER_BOTTOM;
    gpio_init.GPIO_Mode  = GPIO_Mode_IN;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_40MHz;

    GPIO_Init(GPIOA, &gpio_init);
}

/*
*********************************************************************************************************
*                                           BSP_EXTI_Init()
*
* Description : Initialize any or the exit on the board.
*
* Argument(s) : *EXIT_ConfigTypeDef
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_EXTI_Init(EXIT_ConfigTypeDef *USER_EXIT_ConfigType)
{   
	SYSCFG_EXTILineConfig(USER_EXIT_ConfigType->USER_EXTI_PortSourceGPIOx,USER_EXIT_ConfigType->USER_EXTI_PinSourcex);//设置中断引脚
	EXTI_Init(&(USER_EXIT_ConfigType->USER_EXTI_InitStruct));
	NVIC_PriorityGroupConfig(USER_EXIT_ConfigType->USER_NVIC_PriorityGroup);//4级抢占4级子优先级
	NVIC_Init(&(USER_EXIT_ConfigType->USER_NVIC_InitStruct));
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//开启中断时钟源
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          CPU_TS_TmrInit()
*
* Description : Initialize & start CPU timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_TS_Init().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                   (c) When applicable, timer period SHOULD be less than the typical measured time
*                       but MUST be less than the maximum measured time; otherwise, timer resolution
*                       inadequate to measure desired times.
*
*                   See also 'CPU_TS_TmrRd()  Note #2'.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();

    BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;    /* Enable Cortex-M3's DWT CYCCNT reg.                   */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           CPU_TS_TmrRd()
*
* Description : Get current CPU timestamp timer count value.
*
* Argument(s) : none.
*
* Return(s)   : Timestamp timer count (see Notes #2a & #2b).
*
* Caller(s)   : CPU_TS_Init(),
*               CPU_TS_Get32(),
*               CPU_TS_Get64(),
*               CPU_IntDisMeasStart(),
*               CPU_IntDisMeasStop().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                       (1) If timer is a 'down' counter whose values decrease with each time count,
*                           then the returned timer value MUST be ones-complemented.
*
*                   (c) (1) When applicable, the amount of time measured by CPU timestamps is
*                           calculated by either of the following equations :
*
*                           (A) Time measured  =  Number timer counts  *  Timer period
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer period            Timer's period in some units of
*                                                                   (fractional) seconds
*                                       Time measured           Amount of time measured, in same
*                                                                   units of (fractional) seconds
*                                                                   as the Timer period
*
*                                                  Number timer counts
*                           (B) Time measured  =  ---------------------
*                                                    Timer frequency
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer frequency         Timer's frequency in some units
*                                                                   of counts per second
*                                       Time measured           Amount of time measured, in seconds
*
*                       (2) Timer period SHOULD be less than the typical measured time but MUST be less
*                           than the maximum measured time; otherwise, timer resolution inadequate to
*                           measure desired times.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;


    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         CPU_TSxx_to_uSec()
*
* Description : Convert a 32-/64-bit CPU timestamp from timer counts to microseconds.
*
* Argument(s) : ts_cnts   CPU timestamp (in timestamp timer counts [see Note #2aA]).
*
* Return(s)   : Converted CPU timestamp (in microseconds           [see Note #2aD]).
*
* Caller(s)   : Application.
*
*               This function is an (optional) CPU module application programming interface (API)
*               function which MAY be implemented by application/BSP function(s) [see Note #1] &
*               MAY be called by application function(s).
*
* Note(s)     : (1) CPU_TS32_to_uSec()/CPU_TS64_to_uSec() are application/BSP functions that MAY be
*                   optionally defined by the developer when either of the following CPU features is
*                   enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) The amount of time measured by CPU timestamps is calculated by either of
*                       the following equations :
*
*                                                                        10^6 microseconds
*                       (1) Time measured  =   Number timer counts   *  -------------------  *  Timer period
*                                                                            1 second
*
*                                              Number timer counts       10^6 microseconds
*                       (2) Time measured  =  ---------------------  *  -------------------
*                                                Timer frequency             1 second
*
*                               where
*
*                                   (A) Number timer counts     Number of timer counts measured
*                                   (B) Timer frequency         Timer's frequency in some units
*                                                                   of counts per second
*                                   (C) Timer period            Timer's period in some units of
*                                                                   (fractional)  seconds
*                                   (D) Time measured           Amount of time measured,
*                                                                   in microseconds
*
*                   (b) Timer period SHOULD be less than the typical measured time but MUST be less
*                       than the maximum measured time; otherwise, timer resolution inadequate to
*                       measure desired times.
*
*                   (c) Specific implementations may convert any number of CPU_TS32 or CPU_TS64 bits
*                       -- up to 32 or 64, respectively -- into microseconds.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif


#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif
