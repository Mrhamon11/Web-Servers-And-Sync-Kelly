//
// Created by hamon11 on 2/25/18.
//

#ifndef OS_HW1_SERVER_H
#define OS_HW1_SERVER_H
#define FALSE 0
#define TRUE 1


#include <sys/time.h>

//
// Created by hamon11 on 2/25/18.
//

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

_Bool queueAccessible = FALSE;



typedef struct Buffer Buffer;
typedef struct BuffQueue BuffQueue;
typedef struct Thread Thread;

struct Thread {
    pthread_t thread;
    int id;
};

struct Buffer{
    int *socketfd;
    int hit;
    long ret;
    char *buff;
    time_t timeArrived;
    struct Buffer *next;
    struct Buffer *prev;
};

struct BuffQueue{
    Buffer *head;
    Buffer *tail;
    int size;
    int maxSize;
    char *type;
};

Thread* threadInit(int id);

Buffer* bufferInit(int *socketfd, int hit, long ret, char buff[], time_t timeArrived);

BuffQueue* buffQueueInit(int maxSize, char *type);

void addToBuffQueue(BuffQueue *buffQueue, int *socketfd, int hit, long ret, char buff[], time_t timeArrived);

void orderedAdd(BuffQueue *buffQueue, int *socketfd, int hit, long ret, char buff[], time_t timeArrived);

Buffer* pollFromBuffQueue(BuffQueue *buffQueue);

_Bool buffQueueIsEmpty(BuffQueue *buffQueue);

void initThreads(Thread threads[], int numThreads, BuffQueue *queue);

void *executeRequest(void* param);

void web(int *sfd, int hit, long ret, char buffer[], int reqArrived, int reqDispatched, int reqCompleted, time_t dispatchedTime);

void logger(int type, char *s1, char *s2, int socket_fd);

#endif //OS_HW1_SERVER_H
