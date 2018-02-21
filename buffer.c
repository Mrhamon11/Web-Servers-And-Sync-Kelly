#include <stdlib.h>
#include "buffer.h"

//
// Created by parallels on 2/20/18.
//
void bufferInit(Buffer *buffer, int data) {
    buffer = malloc(sizeof(Buffer));
    buffer->data = data;
    buffer->next = NULL;
}

void queueInit(Queue *queue) {
    queue = malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
}

_Bool addToQueue(Queue *queue, int data) {
    Buffer *buff = malloc(sizeof(Buffer));
    if(queue->head == NULL) {
        queue->head = buff;
        queue->tail = buff;
    }
    else {
        queue->tail->next = buff;
        queue->tail = buff;
    }
    return 1;
}