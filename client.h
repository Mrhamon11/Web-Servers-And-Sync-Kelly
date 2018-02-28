//
// Created by parallels on 2/27/18.
//

#ifndef OS_HW1_CLIENT_H
#define OS_HW1_CLIENT_H

//#define TICKET_LOCK_INITIALIZER { PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER }

#include <sys/types.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct ThreadQueue ThreadQueue;
typedef struct ParamStruct ParamStruct;
typedef struct ticket_lock_t ticket_lock_t;



struct ThreadQueue{
    pthread_t *head;
    pthread_t *tail;
};

struct ParamStruct{
    ThreadQueue *queue;
    int clientfd;
    char *path;
    char *secondPath;
    char * schedalg;
    int numThreads;
    ticket_lock_t *ticket;
};

typedef struct ticket_lock_t {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    unsigned long queue_head, queue_tail;
} ticket_lock_t;

ThreadQueue* threadQueueInit();
ParamStruct* psInit(ThreadQueue *queue, int clientfd, char *path, char *secondPath, char *schedalg, int numThreads, ticket_lock_t *ticket);
ticket_lock_t* ticketInit(pthread_mutex_t *mutex, pthread_cond_t *cond);


void *getHandler(void *param);

void GET(int clientfd, char *path);

void ticket_unlock(ticket_lock_t *ticket);

void ticket_lock(ticket_lock_t *ticket);


#endif //OS_HW1_CLIENT_H
