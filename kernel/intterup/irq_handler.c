/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: Flysong
 *
 * irq_handler.c
 *
 * IRQ处理
 *
 */



#include <intterup/irq_handler.h>
#include <intterup/init_int.h>

#include <tayhuang/control_registers.h>

PUBLIC short IRQ_FLAGS[16];

PRIVATE irq_handler IRQ_HANDLERS[16] = {};

//注册IRQ处理器
PUBLIC void register_irq_handler(int irq, irq_handler handler) {
    IRQ_FLAGS[irq] = 0;
    IRQ_HANDLERS[irq] = handler;
}

PRIVATE bool entered_handler = false;

//通用IRQ处理器
PUBLIC void general_irq_handler(int irq, struct intterup_args *args) {
    bool flag = entered_handler;
    if (! flag) {
        entered_handler = true;
    }

    disable_irq(irq); //禁止同类中断接收

    send_eoi(irq); //发送EOI

    if (IRQ_HANDLERS[irq] != NULL) {
        IRQ_FLAGS[irq] = IRQ_HANDLERS[irq](irq, args, flag);
    }

    enable_irq(irq); //同意同类中断接收

    if (! flag) {
        entered_handler = false;
    }
}

void after_syscall(struct intterup_args *regs);

//系统调用处理器
PUBLIC void syscall_int_handler(struct intterup_args *regs) {
    bool flag = entered_handler;
    if (! flag) {
        entered_handler = true;
    }

    // regs->rax = syscall(regs->rax, regs->rbx, regs->rcx, regs->rdx, (void*)regs->rsi, (void*)regs->rdi,
    //  regs->r8, regs->r9, regs->r10, regs->r11, regs->r12, regs->r13, regs->r14, regs->r15);

    if (! flag) {
        entered_handler = false;
    }
}