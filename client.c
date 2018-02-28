/* Generic */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* Network */
#include <netdb.h>
#include <sys/socket.h>
#include "client.h"

#define BUF_SIZE 100


ThreadQueue* threadQueueInit() {
    ThreadQueue *threadQueue = malloc(sizeof(ThreadQueue));
    threadQueue->tail = NULL;
    threadQueue->head = NULL;
    return threadQueue;
}

ParamStruct* psInit(ThreadQueue *queue, int clientfd, char *path, char *schedalg, ticket_lock *ticket){
    ParamStruct *ps = malloc(sizeof(ParamStruct));
    ps->queue = queue;
    ps->clientfd = clientfd;
    ps->path = path;
    ps->schedalg;
    ps->ticket = ticket;
}

void *getHandler(void *param){
    ParamStruct *ps = (ParamStruct*) param;
    GET(ps->clientfd, ps->path, ps->schedalg);
}

void initThreads(pthread_t threads[], int numThreads, ParamStruct *ps){
    for(int i = 0; i < numThreads; i++){
        ThreadQueue *queue = ps->queue;

        pthread_create(&threads[i], NULL, getHandler, ps);
    }
}


// Get host information (used to establishConnection)
struct addrinfo *getHostInfo(char *host, char *port) {
    int r;
    struct addrinfo hints, *getaddrinfo_res;
    // Setup hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((r = getaddrinfo(host, port, &hints, &getaddrinfo_res))) {
        fprintf(stderr, "[getHostInfo:21:getaddrinfo] %s\n", gai_strerror(r));
        return NULL;
    }
    return getaddrinfo_res;
}

// Establish connection with host
int establishConnection(struct addrinfo *info) {
    if (info == NULL) return -1;

    int clientfd;
    for (; info != NULL; info = info->ai_next) {
        if ((clientfd = socket(info->ai_family,
                               info->ai_socktype,
                               info->ai_protocol)) < 0) {
            perror("[establishConnection:35:socket]");
            continue;
        }

        if (connect(clientfd, info->ai_addr, info->ai_addrlen) < 0) {
            close(clientfd);
            perror("[establishConnection:42:connect]");
            continue;
        }

        freeaddrinfo(info);
        return clientfd;
    }

    freeaddrinfo(info);
    return -1;
}

// idk how to use this queue

typedef struct ticket_lock {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    unsigned long queue_head, queue_tail;
} ticket_lock_t;

void ticket_lock(ticket_lock_t *ticket) {
    unsigned long queue_me;

    pthread_mutex_lock(&ticket->mutex);
    queue_me = ticket->queue_tail++;
    while (queue_me != ticket->queue_head) {
        pthread_cond_wait(&ticket->cond, &ticket->mutex);
    }
    pthread_mutex_unlock(&ticket->mutex);
}

void ticket_unlock(ticket_lock_t *ticket) {
    pthread_mutex_lock(&ticket->mutex);
    ticket->queue_head++;
    pthread_cond_broadcast(&ticket->cond);
    pthread_mutex_unlock(&ticket->mutex);
}

// end queue


// Send GET request
void GET(int clientfd, char *path, char *schedalg) {
    // CONCUR scheduling
    if (strcmp(schedalg, "CONCUR") == 0) {
        // wait
        pthread_barrier_wait();
        // send
        char req[1000] = {0};
        sprintf(req, "GET %s HTTP/1.0\r\n\r\n", path);
        send(clientfd, req, strlen(req), 0);
    } else if (strcmp(schedalg, "FIFO") == 0) { // FIFO scheduling
        // lock
        pthread_mutex_lock(&send_lock);
        // send
        char req[1000] = {0};
        sprintf(req, "GET %s HTTP/1.0\r\n\r\n", path);
        send(clientfd, req, strlen(req), 0);
        // unlock
        pthread_mutex_unlock(&send_lock);
    }
    // wait for responses
    char buf[BUF_SIZE];
    pthread_mutex_lock(&recieve_lock);
    printf("\n%s%ld\n", "THREAD: ", pthread_self());
    while (recv(clientfd, buf, BUF_SIZE, 0) > 0) {
        fputs(buf, stdout);
        memset(buf, 0, BUF_SIZE);
    }
    pthread_mutex_unlock(&recieve_lock);
}

int main(int argc, char **argv) {
    int clientfd;
    char buf[BUF_SIZE];

    char *path = argv[3];
    int numThreads = atoi(argv[4]);
    char *schedalg = argv[5];

    // argument check
    if (argc != 6) {
        fprintf(stderr, "USAGE: ./httpclient <hostname> <port> <request path> <threads> <schedalg>\n");
        return 1;
    }

    // thread count check
    if (atoi(argv[4]) < 1) {
        fprintf(stderr, "USAGE: Must initialize one or more threads\n");
        return 1;
    }

    // scheduling check
    if (strcmp(argv[5], "CONCUR") != 0 && strcmp(argv[4], "FIFO") != 0) {
        fprintf(stderr, "%s\n", "USAGE: Must specify either CONCUR or FIFO scheduling");
        return 1;
    }

    // Establish connection with <hostname>:<port>
    clientfd = establishConnection(getHostInfo(argv[1], argv[2]));
    if (clientfd == -1) {
        fprintf(stderr,
                "[main:73] Failed to connect to: %s:%s%s \n",
                argv[1], argv[2], argv[3]);
        return 3;
    }

    // Send GET request > stdout
//    GET(clientfd, argv[3], argv[5]);
    pthread_t threads[atoi(argv[4])];
    ThreadQueue *queue = threadQueueInit();
    ParamStruct *ps = psInit(clientfd, path, schedalg);
    initThreads(threads, atoi(argv[4]), ps);
    while (recv(clientfd, buf, BUF_SIZE, 0) > 0) {
        fputs(buf, stdout);
        memset(buf, 0, BUF_SIZE);
    }

    close(clientfd);
    return 0;
}
