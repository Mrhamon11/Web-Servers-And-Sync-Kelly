#include <stdlib.h>
#include "buffer_struct.h"

//
// Created by Marty on 2/20/18.
//
Buffer* bufferInit(int data) {
    Buffer *buffer = malloc(sizeof(Buffer));
    buffer->data = data;
    buffer->next = NULL;
}

Queue* queueInit() {
    Queue *queue = malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
}

void addToQueue(Queue *queue, int data) {
    Buffer *buff = bufferInit(data);

    if(queue->head == NULL) {
        queue->head = buff;
        queue->tail = buff;
    }
    else {
        queue->tail->next = buff;
        queue->tail = buff;
    }
}

int pollFromQueue(Queue *queue){
    if(queueIsEmpty(queue)){
        return -1;
    }
    Buffer *toRemove = queue->head;
    queue->head = toRemove->next;

    return toRemove->data;
}

_Bool queueIsEmpty(Queue *queue){
    return queue->head == NULL;
}