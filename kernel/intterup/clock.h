/*
 * SPDX-License-Identifier: LGPL-2.1-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 * 
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 * 
 * --------------------------------------------------------------------------------
 * 
 * 作者: theflysong
 * 
 * clock.h
 * 
 * 时钟中断
 * 
 */



#pragma once

#include <intterup/init_int.h>

//ticks
EXTERN PUBLIC VOLATILE int ticks;
//初始化pit
PUBLIC bool init_pit(float frequency);
PUBLIC void clock_int_handler(int irq, struct intterup_args *regs, bool entered_handler);