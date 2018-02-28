//
// Created by parallels on 2/27/18.
//

#ifndef OS_HW1_CLIENT_H
#define OS_HW1_CLIENT_H

#include <sys/types.h>

typedef struct ThreadQueue ThreadQueue;
typedef struct ParamStruct ParamStruct;

struct ThreadQueue{
    pthread_t *head;
    pthread_t *tail;
};

struct ParamStruct{
    ThreadQueue *queue;
    int clientfd;
    char *path;
    char * schedalg;
    ticket_lock_t *ticket;
};

ThreadQueue* threadQueueInit();
ParamStruct* psInit(ThreadQueue *queue, int clientfd, char *path, char *schedalg, ticket_lock_t *ticket);

void *getHandler(void *param);

void GET(int clientfd, char *path, char *schedalg);


#endif //OS_HW1_CLIENT_H
