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
 * framebuffers.c
 * 
 * framebuffer表
 * 
 */



#include <framebuffers.h>

#include <memory/malloc.h>

//TODO: 用红黑树
typedef struct __framebuffer_node {
    int pid;
    frame_t *framebuffer;
    struct __framebuffer_node *next_node;
} framebuffer_node;

PRIVATE framebuffer_node *list_head = NULL;

PRIVATE frame_t *create_frame(void *framebuffer, int column, int line, int width, int height) {
    frame_t *frame = (frame_t*)(malloc(sizeof(frame_t)));
    frame->framebuffer = framebuffer;
    frame->column = column;
    frame->line = line;
    frame->width = width;
    frame->height = height;

    return frame;
}

PUBLIC void add_node(framebuffer_node *node) {
    node->next_node = list_head;
    list_head = node;
}

PUBLIC frame_t *get_framebuffer(int pid) {
    if (list_head == NULL) {
        return NULL;
    }

    framebuffer_node *node = list_head;

    while (node->next_node != NULL && (node->pid != pid)) {
        node = node->next_node;
    }

    if (node->pid != pid) {
        return NULL;
    }

    return node->framebuffer;
}

PUBLIC void set_framebuffer(int pid, void *framebuffer, int column, int line, int width, int height) {
    if (list_head == NULL) {
        framebuffer_node *new_node = (framebuffer_node*)malloc(sizeof(framebuffer));
        new_node->pid = pid;
        new_node->framebuffer = create_frame(framebuffer, column, line, width, height);
        add_node(new_node);
        return;
    }

    framebuffer_node *node = list_head;

    while (node->next_node != NULL && (node->pid != pid)) {
        node = node->next_node;
    }

    if (node->pid != pid) {
        framebuffer_node *new_node = (framebuffer_node*)malloc(sizeof(framebuffer));
        new_node->pid = pid;
        new_node->framebuffer = create_frame(framebuffer, column, line, width, height);
        add_node(new_node);

        return;
    }

    node->pid = pid;
    node->framebuffer = framebuffer;
}

PUBLIC bool has_framebuffer(int pid) {
    if (list_head == NULL) {
        return false;
    }

    framebuffer_node *node = list_head;

    while (node->next_node != NULL && (node->pid != pid)) {
        node = node->next_node;
    }

    return node->pid == pid;
}


PUBLIC void remove_framebuffer(int pid) {
    if (list_head == NULL) {
        return;
    }

    framebuffer_node *last = NULL;
    framebuffer_node *node = list_head;

    while (node->next_node != NULL && (node->pid != pid)) {
        last = node;
        node = node->next_node;
    }

    if (node->pid != pid) {
        return;
    }

    if (last == NULL) {
        last->next_node = node->next_node;
    }
    else {
        list_head = node->next_node;
    }

    free (node->framebuffer);
    free (node);
}