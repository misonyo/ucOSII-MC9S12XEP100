/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                           MASTER INCLUDE FILE
*********************************************************************************************************
*/

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdint.h>
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#include    "os_cpu.h"
#include    "os_cfg.h"
#include    "ucos_ii.h"
#include    "pc.h"
#include  "SD.h"

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)*/


extern OS_FLAG_GRP* TaskEventFlag;
extern OS_EVENT *mbox;

