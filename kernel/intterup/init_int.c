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
 * init_int.c
 *
 * 初始化中断
 *
 */


#include <tayhuang/ports.h>
#include <tayhuang/int_vectors.h>
#include <tayhuang/io.h>
#include <tayhuang/descs.h>

#include <intterup/init_int.h>
#include <intterup/exception.h>
#include <intterup/irq_handler.h>

//初始化PIC
PUBLIC void init_pic(void) {
    //ICW1
    outb (M_PIC_CONTROL, 0x11);
    outb (S_PIC_CONTROL, 0x11);

    //ICW2
    outb (M_PIC_DATA, INT_VECTOR_IRQ0);
    outb (S_PIC_DATA, INT_VECTOR_IRQ8); 

    //ICW3
    outb (M_PIC_DATA, 0x4);
    outb (S_PIC_DATA, 0x2); 

    //ICW4
    outb (M_PIC_DATA, 0x1);
    outb (S_PIC_DATA, 0x1); 

    //OCW1
    outb (M_PIC_DATA, 0xFF); 
    outb (S_PIC_DATA, 0xFF);
}

//禁用/启用 IRQ
PUBLIC void disable_irq(int irq) {
    if (irq < 8) {
        //主片
        byte i = inb(M_PIC_DATA); 
        i |= (1 << irq);
        outb(M_PIC_DATA, i);
    }
    else {
        //从片
        byte i = inb(S_PIC_DATA); 
        i |= (1 << (irq - 8));
        outb(S_PIC_DATA, i);
    }
}

PUBLIC void enable_irq(int irq) {
    if (irq < 8) {
        //主片
        byte i = inb(M_PIC_DATA); 
        i &= ~(1 << irq);
        outb(M_PIC_DATA, i);
    }
    else {
        //从片
        byte i = inb(S_PIC_DATA); 
        i &= ~(1 << (irq - 8));
        outb(S_PIC_DATA, i);
    }
}

#define PIC_EOI (0x20)

//发送EOI
PUBLIC void send_eoi(int irq) {
    if (irq > 8) {
        //从片EOI
        outb (S_PIC_CONTROL, PIC_EOI); 
    }
    //主片EOI
    outb (M_PIC_CONTROL, PIC_EOI); 
}

#define IDT_SIZE (128)

//IDT与IDTR
PRIVATE gate_desc IDT[IDT_SIZE];
PRIVATE struct desc_ptr IDTR;

//初始化一个IDT描述符
PRIVATE void init_idt_desc_ist(byte vector, byte type, int_handler handler, byte privilege, byte ist) {
    qword base = (qword)handler;
    //偏移
    IDT[vector].offset_low = base & 0xFFFF; 
    //段
    IDT[vector].segment = rdcs(); 
    //栈
    IDT[vector].bits.ist = ist;
    IDT[vector].bits.zero = 0;
    //类型
    IDT[vector].bits.type = type; 
    //权限
    IDT[vector].bits.dpl = privilege; 
    //存在
    IDT[vector].bits.p = 1; 
    //偏移
    IDT[vector].offset_middle = base >> 16; 
    //偏移
    IDT[vector].offset_high = base >> 32;
    IDT[vector].reserved = 0;
}

//初始化一个IDT描述符
PRIVATE void init_idt_desc(byte vector, byte type, int_handler handler, byte privilege) {
    qword base = (qword)handler;
    //偏移
    IDT[vector].offset_low = base & 0xFFFF; 
    //段
    IDT[vector].segment = rdcs(); 
    //栈
    IDT[vector].bits.ist = 0;
    IDT[vector].bits.zero = 0;
    //类型
    IDT[vector].bits.type = type; 
    //权限
    IDT[vector].bits.dpl = privilege; 
    //存在
    IDT[vector].bits.p = 1; 
    //偏移
    IDT[vector].offset_middle = base >> 16;
    //偏移
    IDT[vector].offset_high = base >> 32;
    IDT[vector].reserved = 0;
}

//初始化IDT
PRIVATE void __init_descs(void) {
    //异常的IDT
    init_idt_desc(0, GATE_INTERRUPT, divide_by_zero_error, 0);
    init_idt_desc(1, GATE_INTERRUPT, single_step_debug, 0);
    init_idt_desc(2, GATE_INTERRUPT, non_maskable_interrup, 0);
    init_idt_desc(3, GATE_INTERRUPT, breakpoint, 0);
    init_idt_desc(4, GATE_INTERRUPT, overflow, 0);
    init_idt_desc(5, GATE_INTERRUPT, bound_range_exceeded, 0);
    init_idt_desc(6, GATE_INTERRUPT, invalid_opcode, 0);
    init_idt_desc(7, GATE_INTERRUPT, device_not_available, 0);
    init_idt_desc(8, GATE_INTERRUPT, double_fault, 0);
    init_idt_desc(9, GATE_INTERRUPT, coprocessor_segment_overrun, 0);
    init_idt_desc(10, GATE_INTERRUPT, invalid_tss, 0);
    init_idt_desc(11, GATE_INTERRUPT, segment_not_present, 0);
    init_idt_desc(12, GATE_INTERRUPT, stack_segment_fault, 0);
    init_idt_desc(13, GATE_INTERRUPT, general_protection_fault, 0);
    init_idt_desc(14, GATE_INTERRUPT, page_fault, 0);
    init_idt_desc(15, GATE_INTERRUPT, reserved1_excepetion, 0);
    init_idt_desc(16, GATE_INTERRUPT, x87_floating_point_exception, 0);
    init_idt_desc(17, GATE_INTERRUPT, alignment_check, 0);
    init_idt_desc(18, GATE_INTERRUPT, machine_check, 0);
    init_idt_desc(19, GATE_INTERRUPT, simd_floating_point_exception, 0);
    init_idt_desc(20, GATE_INTERRUPT, virtualization_exception, 0);
    init_idt_desc(21, GATE_INTERRUPT, control_protection_exception, 0);
    init_idt_desc(22, GATE_INTERRUPT, reserved2_excepetion, 0);
    init_idt_desc(23, GATE_INTERRUPT, reserved3_excepetion, 0);
    init_idt_desc(24, GATE_INTERRUPT, reserved4_excepetion, 0);
    init_idt_desc(25, GATE_INTERRUPT, reserved5_excepetion, 0);
    init_idt_desc(26, GATE_INTERRUPT, reserved6_excepetion, 0);
    init_idt_desc(27, GATE_INTERRUPT, reserved7_excepetion, 0);
    init_idt_desc(28, GATE_INTERRUPT, hypervisor_injection_exception, 0);
    init_idt_desc(29, GATE_INTERRUPT, vmm_communication_exception, 0);
    init_idt_desc(30, GATE_INTERRUPT, security_exception, 0);
    init_idt_desc(31, GATE_INTERRUPT, reserved8_excepetion, 0);

    //IRQ处理器
    init_idt_desc_ist(CALC_IRQ(0), GATE_INTERRUPT, irq0_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(1), GATE_INTERRUPT, irq1_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(2), GATE_INTERRUPT, irq2_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(3), GATE_INTERRUPT, irq3_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(4), GATE_INTERRUPT, irq4_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(5), GATE_INTERRUPT, irq5_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(6), GATE_INTERRUPT, irq6_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(7), GATE_INTERRUPT, irq7_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(8), GATE_INTERRUPT, irq8_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(9), GATE_INTERRUPT, irq9_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(10), GATE_INTERRUPT, irq10_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(11), GATE_INTERRUPT, irq11_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(12), GATE_INTERRUPT, irq12_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(13), GATE_INTERRUPT, irq13_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(14), GATE_INTERRUPT, irq14_handler, 0, 1);
    init_idt_desc_ist(CALC_IRQ(15), GATE_INTERRUPT, irq15_handler, 0, 1);

    init_idt_desc_ist(0x40, GATE_INTERRUPT, syscall_handler, 3, 1);
}

PUBLIC void init_idt(void) {
    __init_descs();
    IDTR.address = IDT;
    IDTR.size = sizeof(IDT);
    asmv ("lidt %0" : : "m"(IDTR));
}