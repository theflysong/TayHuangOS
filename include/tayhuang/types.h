/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: theflysong
 *
 * types.h
 *
 * 基础的类型
 *
 */



#pragma once

#include <stddef.h>

typedef unsigned char bool; //布尔
typedef unsigned char byte; //字节
typedef unsigned short word; //字
typedef unsigned int dword; //双字
typedef unsigned long long qword; //四字
typedef byte b8; //8位
typedef word b16; //16位
typedef dword b32; //32位
typedef qword b64; //64位
typedef b16 sreg_t; //段寄存器
typedef b8 reg8_t; //8位寄存器
typedef b16 reg16_t; //16位寄存器
typedef b32 reg32_t; //32位寄存器
typedef b64 reg64_t; //64位寄存器
typedef void *handle_t; //处理器
#define true (1) //真
#define false (0) //假

#define asmv asm volatile //简写
typedef void *addr_t; //地址
typedef dword id_t; //编号
typedef int service_t; //服务号
typedef qword offset_t; //偏移