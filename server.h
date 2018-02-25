//
// Created by hamon11 on 2/25/18.
//

#ifndef OS_HW1_SERVER_H
#define OS_HW1_SERVER_H

//
// Created by hamon11 on 2/25/18.
//

typedef struct Buffer Buffer;
typedef struct BuffQueue BuffQueue;

struct Buffer{
    int socketfd;
    int hit;
    struct Buffer *next;
};

struct BuffQueue{
    Buffer *head;
    Buffer *tail;
    int size;
    int maxSize;
};


Buffer* bufferInit(int socketfd, int hit);

BuffQueue* buffQueueInit(int maxSize);

void addToBuffQueue(BuffQueue *buffQueue, int socketfd, int hit);

Buffer* pollFromBuffQueue(BuffQueue *buffQueue);

_Bool buffQueueIsEmpty(BuffQueue *buffQueue);

int threadIndex(int curIndex, int numThreads);

#endif //OS_HW1_SERVER_H
