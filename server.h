//
// Created by hamon11 on 2/25/18.
//

#ifndef OS_HW1_SERVER_H
#define OS_HW1_SERVER_H

//
// Created by hamon11 on 2/25/18.
//

typedef struct Buffer Buffer;
typedef struct Queue Queue;

struct Buffer{
    int socketfd;
    int hit;
    struct Buffer *next;
};

struct Queue{
    Buffer *head;
    Buffer *tail;
    int size;
    int maxSize;
};

Buffer* bufferInit(int socketfd, int hit);

Queue* queueInit(int maxSize);

void addToQueue(Queue *queue, int socketfd, int hit);

Buffer* pollFromQueue(Queue *queue);

_Bool queueIsEmpty(Queue *queue);

#endif //OS_HW1_SERVER_H
