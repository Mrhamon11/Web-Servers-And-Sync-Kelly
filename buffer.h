//
// Created by hamon11 on 2/20/18.
//

#ifndef OS_HW1_BUFFER_H
#define OS_HW1_BUFFER_H

typedef struct Buffer Buffer;
typedef struct Queue Queue;

struct Buffer{
    int data;
    struct Buffer *next;
};

struct Queue{
    Buffer *head;
    Buffer *tail;
};

void bufferInit(Buffer *buffer, int data);

void queueInit(Queue *queue);

void addToQueue(Queue *queue, int data);

int pollFromQueue(Queue *queue);

_Bool queueIsEmpty(Queue *queue);

#endif //OS_HW1_BUFFER_H
