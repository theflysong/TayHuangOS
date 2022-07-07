/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 * 
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 * 
 * --------------------------------------------------------------------------------
 * 
 * 作者: theflysong
 * 
 * syscalls.c
 * 
 * 系统调用的实现
 * 
 */



#include <syscall/syscalls.h>
#include <syscall/syscall.h>

#include <task/task_scheduler.h>
#include <task/task_manager.h>

#include <logging.h>

#include <memory/pmm.h>

#include <string.h>

PUBLIC void __moo(void) {
    linfo ("COW", "                       (__)");
    linfo ("COW", "                       (oo)");
    linfo ("COW", "            /-----------\\/");
    linfo ("COW", "           / |  MOO     ||");
    linfo ("COW", "          /  |    COW   ||");
    linfo ("COW", "         *  /\\----------/\\");
    linfo ("COW", "            ~~          ~~");
    linfo ("COW", "Moo~Have you mooed today?Moo with me, moo~");
}

PUBLIC void moo(void) {
    dosyscall(MOO_SN, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
}

//-------------------

PUBLIC bool __sendmsg(void *src, qword size, int dst) {
    task_struct *target = get_task_by_pid(dst);

    //剩余空间不足
    if (target->ipc_info.used_size + sizeof(msgpack_struct) + size > target->ipc_info.mail_size) {
        return false;
    }

    //获取消息包地址
    msgpack_struct *pack = (msgpack_struct*)target->ipc_info.current_write_ptr;
    //更新写入指针
    target->ipc_info.current_write_ptr += sizeof(msgpack_struct) + size; 

    //更新已使用大小
    target->ipc_info.used_size += sizeof(msgpack_struct) + size; 

    //写入指针超出队列
    if (target->ipc_info.current_write_ptr >= (target->ipc_info.mail + target->ipc_info.mail_size)) { 
        //剩余空间不足
        if (target->ipc_info.mail + sizeof(msgpack_struct) + size > target->ipc_info.current_read_ptr) { 
            //还原
            target->ipc_info.current_write_ptr -= sizeof(msgpack_struct) + size; 
            target->ipc_info.used_size -= sizeof(msgpack_struct) + size;
            return false;
        }

        //设置新地址
        pack = target->ipc_info.mail; 
        //设置新写入指针
        target->ipc_info.current_write_ptr = target->ipc_info.mail + sizeof(msgpack_struct) + size; 
        //更新已使用大小
        target->ipc_info.used_size = target->ipc_info.mail_size - (target->ipc_info.current_read_ptr - target->ipc_info.current_write_ptr); 
    }

    //临时变量(不能直接操作pack(为虚拟地址))
    msgpack_struct _pack; 

    _pack.source = current_task->pid;
    _pack.length = size;
    _pack.next = NULL;

    //复制到缓冲区
    pvmemcpy(pack, target->mm_info.pgd, &_pack, sizeof(msgpack_struct)); 
    
    //有上一条消息
    if (target->ipc_info.lastest_msg != NULL) { 
        //从缓冲区复制
        vpmemcpy(&_pack, target->mm_info.pgd, target->ipc_info.lastest_msg, sizeof(msgpack_struct)); 
        _pack.next = pack;
        //复制到缓冲区
        pvmemcpy(target->mm_info.pgd, target->ipc_info.lastest_msg, &_pack, sizeof(msgpack_struct)); 
    }
    
    //复制正文
    vvmemcpy(target->mm_info.pgd, pack->body, current_task->mm_info.pgd, src, size);

    return true;
}

PUBLIC bool sendmsg(void *src, qword size, int dst) {
    return dosyscall(SENDMSG_SN, 0, size, dst, src, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
}

//-------------------

PUBLIC void __wait_ipc(int pid) {
    current_task->state = WAITING_IPC;
    current_task->wait_pid = pid;
}

PUBLIC void wait_ipc(int pid) {
    dosyscall(WAIT_IPC_SN, 0, 0, pid, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

//-------------------

PUBLIC void __setmail_buffer(void *buffer, qword size) {
    //设置指针
    current_task->ipc_info.current_read_ptr = current_task->ipc_info.current_write_ptr = current_task->ipc_info.mail = buffer;
    //设置邮箱大小
    current_task->ipc_info.mail_size = size;
    //设置已使用大小
    current_task->ipc_info.used_size = 0;
}

PUBLIC void setmail_buffer(void *buffer, qword size) {
    dosyscall(SETMAIL_BUFFER_SN, 0, size, 0, NULL, buffer, 0, 0, 0, 0, 0, 0, 0, 0);
}