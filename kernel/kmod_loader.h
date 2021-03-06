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
 * kmod_loader.h
 * 
 * 内核模块加载器
 * 
 */



#pragma once

#include <tayhuang/defs.h>
#include <task/task_struct.h>

//程序信息
typedef struct {
    void *start;
    void *end;
    void *entry;
    void *pgd;
    void *stack_top;
    void *stack_bottom;
    void *heap_bottom;
    void *heap_top;
} program_info;

//从内存中加载kmod
PUBLIC program_info load_kmod_from_memory(void *addr);
//初始化kmod进程
PUBLIC task_struct *initialize_kmod_task(task_struct *kmod_task);