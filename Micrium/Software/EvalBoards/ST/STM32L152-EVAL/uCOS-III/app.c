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
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM32L152-EVAL
*                                          Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* ----------------- APPLICATION GLOBALS ------------------ */
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  CPU_BOOLEAN     AppSw;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    
    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
    
    (void)&err;
    
    return (0);
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
    EXIT_ConfigTypeDef EXIT_Config;

   (void)p_arg;


    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services  测量中断响应时间   ，读取时间戳等                 */
    
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OSCfg_TickRate_Hz;  

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).  设置时钟滴答时长为 OSCfg_TickRate_Hz           */

    Mem_Init();                                                 /* Initialize memory managment module                   */
    Math_Init();                                                /* Initialize mathematical module                       */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)    
    App_SerialInit();                                           /* Initialize Serial communication for application ...  */
#endif
        
                                                                /* ... tracing                                          */
    APP_TRACE_DBG(("\n\n\r"));
    APP_TRACE_DBG(("Creating Application kernel objects\n\r"));
    AppObjCreate();                                             /* Create Applicaiton kernel objects                    */
    
    APP_TRACE_DBG(("Creating Application Tasks\n\r"));
    AppTaskCreate();                                            /* Create Application tasks                             */
    
	
/*
****************************************************************************************************
*
*
*
*
*
*****************************************************************************************************
*/   
	
	BSP_IntVectSet(BSP_INT_ID_EXTI0,BSP_IntHandler_EXTI0_PA0);
    EXIT_Config.USER_EXTI_PinSourcex=EXTI_PinSource0;//引脚
	EXIT_Config.USER_EXTI_PortSourceGPIOx=EXTI_PortSourceGPIOA;//端口
	
	EXIT_Config.USER_NVIC_PriorityGroup=NVIC_PriorityGroup_2;//优先级组，这里是2位抢占级两位子优先级
	
	EXIT_Config.USER_EXTI_InitStruct.EXTI_Line=EXTI_Line0;//设置中断线路
	EXIT_Config.USER_EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXIT_Config.USER_EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXIT_Config.USER_EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;//设置模式
	
	EXIT_Config.USER_NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn; 
	EXIT_Config.USER_NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=4;
	EXIT_Config.USER_NVIC_InitStruct.NVIC_IRQChannelSubPriority=4;
    EXIT_Config.USER_NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//中断号及对应优先级
	
	BSP_EXTI_Init(&EXIT_Config);//自定义的一个配置函数
	AppSw = DEF_TRUE;
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        BSP_LED_Toggle(0);                                      /* Toggle LEDs on board                                 */
    BSP_LED_On(2);
	//   data=GPIO_ReadInputData(GPIOA);
	//	data&=0x01u;
	//	if(data==0x01u)
	//	{
	//		BSP_LED_Off(2);
	//	}
	//	else
	//	{
	//		BSP_LED_On(2);
	//	}
		OSTimeDlyHMSM(0, 0, 1, 0,                             /* Delay for 100 milliseconds                           */
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err); 
	BSP_LED_On(1);  
	OSTimeDlyHMSM(0, 0, 1, 0,                             /* Delay for 100 milliseconds                           */
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err); 
        if (AppSw != DEF_FALSE) {
            AppSw = DEF_FALSE;
            CPU_Init();
            OSStatReset(&err);             
        }
    }
}

/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{

}


/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{

}

