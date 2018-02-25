#include <stdlib.h>
#include "web_page_buffer.h"

//
// Created by Marty on 2/20/18.
//
Buffer* bufferInit(int socketfd, int hit) {
    Buffer *buffer = malloc(sizeof(Buffer));
    buffer->socketfd = socketfd;
    buffer->hit = hit;
    buffer->next = NULL;
}

Queue* queueInit(int maxSize) {
    Queue *queue = malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->maxSize = maxSize;
}

void addToQueue(Queue *queue, int socketfd, int hit) {
    Buffer *buff = bufferInit(socketfd, hit);
    if(queue->size < queue->maxSize){
        if(queue->head == NULL) {
            queue->head = buff;
            queue->tail = buff;
        }
        else {
            queue->tail->next = buff;
            queue->tail = buff;
        }
        queue->size++;
    }
}

Buffer* pollFromQueue(Queue *queue){
    if(queueIsEmpty(queue)){
        return NULL;
    }
    Buffer *toRemove = queue->head;
    queue->head = toRemove->next;

    queue->size--;

    return toRemove;
}

_Bool queueIsEmpty(Queue *queue){
    return queue->size == 0;
}