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
 * main.c
 *
 * testbench1主函数
 *
 */



#include <debug/logging.h>

#include <assert.h>

#include <memory/malloc.h>

#include <tayhuang/services.h>

#include <syscall/ipc.h>
#include <syscall/rpc.h>

#include <memory/sharemem.h>

#include <export/__video_driver_fn.h>

PUBLIC void normal_ipc_handler(int caller, void *msg) {
    set_allow(ANY_TASK);
}

PUBLIC void kmod_main(void) {
    set_logging_name("Testbench2");

    linfo ("Hi!I'm testbench2!");
    
    register_normal_ipc_handler(normal_ipc_handler);
    set_allow(ANY_TASK);

    void *addr = create_share_memory(1);
    *(qword*)(addr) = 8;

    void *new_addr = share_memory(addr, 1, 2);

    linfo ("%p", new_addr);

    send_msg(MSG_NORMAL_IPC, &new_addr, sizeof(void*), 2);

    write_string(0, 0, 0x0A, "I'm testbench2");

    void *framebuffer = create_framebuffer(4, 4, 4, 4);
    linfo ("%p", framebuffer);
    *(byte*)(framebuffer + 0) = 'A';
    *(byte*)(framebuffer + 1) = 0x0A;
    *(byte*)(framebuffer + 2) = 'B';
    *(byte*)(framebuffer + 3) = 0x0A;
    *(byte*)(framebuffer + 4) = 'C';
    *(byte*)(framebuffer + 5) = 0x0A;
    swap_framebuffer(false);

    message_loop();

    while (true);
}