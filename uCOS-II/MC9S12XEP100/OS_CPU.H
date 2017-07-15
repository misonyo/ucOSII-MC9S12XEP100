/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                          All Rights Reserved
*
*                                       MC9S12DP256/DG128 Specific code
*                                          SMALL MEMORY MODEL
*
* File : OS_CPU.H
* By   : Jean J. Labrosse
* Update By : Shao Senlong
************** Tsinghua University Motorola MCU & DSP Application Center
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/
#ifdef OS_CPU_GLOBALS 
#define OS_CPU_EXT
#else
#define OS_CPU_EXT extern
#endif


#ifndef _TYPES
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned int   INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   int   INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* 32 bit floatpoint quantity                         */
typedef double         FP64;                     /* 64 bit floatpoint quantity                         */
typedef unsigned char  OS_CPU_SR;                /* CPU CCR quantity                                   */
#define _TYPES
#endif
#define  OS_STK               INT8U



#define  OS_STK_GROWTH        1                  /* Stack grows from HIGH to LOW memory on MC9S12      */
#define  OS_TASK_SW()         asm swi      /*OSCtxSw()*/

#define  OS_CRITICAL_METHOD   3

#if      OS_CRITICAL_METHOD == 3
#define  OS_ENTER_CRITICAL()  asm tpa; asm sei; asm staa cpu_sr
#define  OS_EXIT_CRITICAL()   asm ldaa cpu_sr; asm tap
#endif

#if      OS_CRITICAL_METHOD == 2
#define  OS_ENTER_CRITICAL()  asm pshc; asm sei;
#define  OS_EXIT_CRITICAL()   asm pulc;
#endif

#if      OS_CRITICAL_METHOD == 1
#define  OS_ENTER_CRITICAL()  asm sei;
#define  OS_EXIT_CRITICAL()   asm cli;
#endif

#define  OS_SAVE_SP();        if(OSIntNesting==1){asm ldx OSTCBCur; asm sts 0,x;}

