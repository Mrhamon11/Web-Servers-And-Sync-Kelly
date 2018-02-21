//
// Created by hamon11 on 2/20/18.
//

#ifndef OS_HW1_BUFFER_H
#define OS_HW1_BUFFER_H

typedef struct{
    int data;
    Buffer *next;
}Buffer;

typedef struct{
    Buffer *head;
    Buffer *tail;
}Queue;

void bufferInit(Buffer *buffer, int data);

void queueInit(Queue *queue);

_Bool addToQueue(Queue *queue, int data);

Buffer* pollFromQueue(Queue *queue);

#endif //OS_HW1_BUFFER_H
