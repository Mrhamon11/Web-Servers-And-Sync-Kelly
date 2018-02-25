//
// Created by hamon11 on 2/25/18.
//

#ifndef OS_HW1_SERVER_H
#define OS_HW1_SERVER_H
#define 0 FALSE
#define 1 TRUE

//
// Created by hamon11 on 2/25/18.
//

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

_Bool queueLocked = FALSE;



typedef struct Buffer Buffer;
typedef struct BuffQueue BuffQueue;
typedef struct Thread Thread;

struct Thread {
    pthread_t thread;
    int id;
};

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

Thread* threadInit(int id);

Buffer* bufferInit(int socketfd, int hit);

BuffQueue* buffQueueInit(int maxSize);

void addToBuffQueue(BuffQueue *buffQueue, int socketfd, int hit);

Buffer* pollFromBuffQueue(BuffQueue *buffQueue);

_Bool buffQueueIsEmpty(BuffQueue *buffQueue);

int threadIndex(int curIndex, int numThreads);

void *executeRequest(void* param);

#endif //OS_HW1_SERVER_H
