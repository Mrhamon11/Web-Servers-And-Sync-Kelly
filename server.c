#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"

#define VERSION 23
#define BUFSIZE 8096
#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404


typedef struct Stats Stats;

struct {
    char *ext;
    char *filetype;
} extensions [] = {
        {"gif", "image/gif" },
        {"jpg", "image/jpg" },
        {"jpeg","image/jpeg"},
        {"png", "image/png" },
        {"ico", "image/ico" },
        {"zip", "image/zip" },
        {"gz",  "image/gz"  },
        {"tar", "image/tar" },
        {"htm", "text/html" },
        {"html","text/html" },
        {0,0} };



//Our code:

struct Stats{
    int reqArrived;
    int reqDispatched;
    int reqCompleted;
    time_t serverStartTime;
};

Stats *stats;

Stats* statsInit(Stats *stats){
    stats = malloc(sizeof(Stats));
    stats->reqArrived = 0;
    stats->reqDispatched = 0;
    stats->reqCompleted = 0;
    return stats;
}

Thread* threadInit(int id) {
	Thread *thread = malloc(sizeof(Thread));
	thread->id = id;
	return thread;
}

Buffer* bufferInit(int *socketfd, int hit, long ret, char buff[], time_t timeArrived) {
	Buffer *buffer = malloc(sizeof(Buffer));
	buffer->socketfd = socketfd;
	buffer->hit = hit;
	buffer->next = NULL;
	buffer->prev = NULL;
    buffer->ret = ret;
	buffer->timeArrived = timeArrived;
    char *buffcopy = malloc(sizeof(char) * strlen(buff));
    strcpy(buffcopy, buff);
    buffer->buff = buffcopy;
	return buffer;
}

BuffQueue* buffQueueInit(int maxSize, char *type) {
	BuffQueue *queue = malloc(sizeof(BuffQueue));
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
	queue->maxSize = maxSize;
	queue->type = type;
	return queue;
}

void addToBuffQueue(BuffQueue *buffQueue, int *socketfd, int hit, long ret, char buff[], time_t timeArrived) {
	if(strcmp(buffQueue->type, "FIFO") == 0 || strcmp(buffQueue->type, "ANY") == 0) {
		Buffer *buffer = bufferInit(socketfd, hit, ret, buff, timeArrived);

		if(buffQueue->size < buffQueue->maxSize){
			if(buffQueue->head == NULL) {
				buffQueue->head = buffer;
				buffQueue->tail = buffer;
			}
			else {
				buffQueue->tail->next = buffer;
				buffQueue->tail = buffer;
			}
			buffQueue->size++;
		}
	}
	else {
		orderedAdd(buffQueue, socketfd, hit, ret, buff, timeArrived);
	}

}

void orderedAdd(BuffQueue *buffQueue, int *socketfd, int hit, long ret, char buff[], time_t timeArrived) {
	Buffer *buffer = bufferInit(socketfd, hit, ret, buff, timeArrived);
	int buflen = strlen(buff);

	char * fstr = (char *)0;
	for(int i=0;extensions[i].ext != 0;i++) {
		long len = strlen(extensions[i].ext);
		if( !strncmp(&buff[buflen-len], extensions[i].ext, len)) {
			fstr =extensions[i].filetype;
			break;
		}
	}
	if(buffQueue->size < buffQueue->maxSize){
		if(strcmp(fstr, "text/html") == 0) {
			if(buffQueueIsEmpty(buffQueue)) {
				buffQueue->head = buffer;
				buffQueue->tail = buffer;
			}
			else {
				buffer->next = buffQueue->head;
				buffQueue->head = buffer;
			}

		}
		else {
			if(buffQueueIsEmpty(buffQueue)) {
				buffQueue->head = buffer;
				buffQueue->tail = buffer;
			}
			else {
				buffer->prev = buffQueue->tail;
				buffQueue->tail = buffer;
			}
		}
		buffQueue->size++;
	}
}

Buffer* pollFromBuffQueue(BuffQueue *buffQueue){
	if(buffQueueIsEmpty(buffQueue)){
		return NULL;
	}

	Buffer *toRemove;
	if(strcmp(buffQueue->type, "HPIC") == 0) {
		toRemove = buffQueue->tail;
		buffQueue->tail = buffQueue->tail->prev;

	}
	else {
		toRemove = buffQueue->head;
		buffQueue->head = toRemove->next;
	}

	buffQueue->size--;
	return toRemove;

}

_Bool buffQueueIsEmpty(BuffQueue *buffQueue){
	return buffQueue->size == 0;
}

void initThreads(Thread threads[], int numThreads, BuffQueue* queue){
    for(int i = 0; i < numThreads; i++){
        Thread thread = threads[i];
        thread.id = i;
        pthread_create(&thread.thread, NULL, executeRequest, queue);
    }
}

void *executeRequest(void* param) {
	BuffQueue *buffQueue = (BuffQueue*) param;
	while(TRUE) {
		while(!queueAccessible) {
			pthread_cond_wait(&cond, &m);
		}
		pthread_mutex_lock(&m);
		stats->reqDispatched++;
		queueAccessible = FALSE;
		Buffer *buffer = pollFromBuffQueue(buffQueue);
		queueAccessible = buffQueueIsEmpty(buffQueue) ? FALSE : TRUE;

		int reqArrived = stats->reqArrived;
		int reqDispatched = stats->reqDispatched;
		int reqCompleted = stats->reqCompleted;

        struct timeval curTime;
        gettimeofday(&curTime, NULL);


        time_t dispatchedTime = curTime.tv_sec - stats->serverStartTime;

		pthread_mutex_unlock(&m);
		pthread_cond_broadcast(&cond);
		web(buffer->socketfd,buffer->hit, buffer->ret, buffer->buff, reqArrived, reqDispatched, reqCompleted, dispatchedTime);
		pthread_mutex_lock(&m);
		stats->reqCompleted++;
		pthread_mutex_unlock(&m);
	}
}


static int dummy; //keep compiler happy

void logger(int type, char *s1, char *s2, int socket_fd)
{
	int fd ;
	char logbuffer[BUFSIZE*2];

	switch (type) {
	case ERROR: (void)sprintf(logbuffer,"ERROR: %s:%s Errno=%d exiting pid=%d",s1, s2, errno,getpid());
		break;
	case FORBIDDEN:
		dummy = write(socket_fd, "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",271);
		(void)sprintf(logbuffer,"FORBIDDEN: %s:%s",s1, s2);
		break;
	case NOTFOUND:
		dummy = write(socket_fd, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this server.\n</body></html>\n",224);
		(void)sprintf(logbuffer,"NOT FOUND: %s:%s",s1, s2);
		break;
	case LOG: (void)sprintf(logbuffer," INFO: %s:%s:%d",s1, s2,socket_fd); break;
	}
	/* No checks here, nothing can be done with a failure anyway */
	if((fd = open("nweb.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0) {
		dummy = write(fd,logbuffer,strlen(logbuffer));
		dummy = write(fd,"\n",1);
		(void)close(fd);
	}
	if(type == ERROR || type == NOTFOUND || type == FORBIDDEN) exit(3);
}


/* this is a child web server process, so we can exit on errors */
void web(int *sfd, int hit, long ret, char buffer[], int reqArrived, int reqDispatched, int reqCompleted, time_t dispatchedTime)
{
    int fd = *sfd;
	int j, file_fd, buflen;
	long i, /*ret,*/ len;
	char * fstr;
//	static char buffer[BUFSIZE+1]; /* static so zero filled */
//
//	ret =read(fd,buffer,BUFSIZE); 	/* read Web request in one go */
	if(ret == 0 || ret == -1) {	/* read failure stop now */
		logger(FORBIDDEN,"failed to read browser request","",fd);
	}
	if(ret > 0 && ret < BUFSIZE)	/* return code is valid chars */
		buffer[ret]=0;		/* terminate the buffer */
	else buffer[0]=0;
	for(i=0;i<ret;i++)	/* remove CF and LF characters */
		if(buffer[i] == '\r' || buffer[i] == '\n')
			buffer[i]='*';
	logger(LOG,"request",buffer,hit);
	if( strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4) ) {
		logger(FORBIDDEN,"Only simple GET operation supported",buffer,fd);
	}
	for(i=4;i<BUFSIZE;i++) { /* null terminate after the second space to ignore extra stuff */
		if(buffer[i] == ' ') { /* string is "GET URL " +lots of other stuff */
			buffer[i] = 0;
			break;
		}
	}
	for(j=0;j<i-1;j++) 	/* check for illegal parent directory use .. */
		if(buffer[j] == '.' && buffer[j+1] == '.') {
			logger(FORBIDDEN,"Parent directory (..) path names not supported",buffer,fd);
		}
	if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6) ) /* convert no filename to index file */
		(void)strcpy(buffer,"GET /index.html");

	/* work out the file type and check we support it */
	buflen=strlen(buffer);
	fstr = (char *)0;
	for(i=0;extensions[i].ext != 0;i++) {
		len = strlen(extensions[i].ext);
		if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
			fstr =extensions[i].filetype;
			break;
		}
	}
	if(fstr == 0) logger(FORBIDDEN,"file extension type not supported",buffer,fd);

	if(( file_fd = open(&buffer[5],O_RDONLY)) == -1) {  /* open the file for reading */
		logger(NOTFOUND, "failed to open file",&buffer[5],fd);
	}

    struct timeval curTime;

    gettimeofday(&curTime, NULL);
    time_t completedTime = curTime.tv_sec - stats->serverStartTime;


    logger(LOG,"SEND",&buffer[5],hit);
	len = (long)lseek(file_fd, (off_t)0, SEEK_END); /* lseek to the file end to find the length */
	      (void)lseek(file_fd, (off_t)0, SEEK_SET); /* lseek back to the file start ready for reading */
          (void)sprintf(buffer,"HTTP/1.1 200 OK\nServer: nweb/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\nRequests Arrived: %d\nRequests Dispatched: %d\nRequests Completed: %d\nDispatched Time: %lu\nCompleted Time: %lu\n\n", VERSION, len, fstr, reqArrived, reqDispatched, reqCompleted, dispatchedTime, completedTime); /* Header + a blank line */
    logger(LOG,"Header",buffer,hit);
    dummy = write(fd,buffer,strlen(buffer));
          (void)sprintf(buffer, "test string \n");
    dummy = write(fd,buffer, strlen(buffer));

    /* Send the statistical headers described in the paper, example below

    (void)sprintf(buffer,"X-stat-req-arrival-count: %d\r\n", xStatReqArrivalCount);
	dummy = write(fd,buffer,strlen(buffer));
    */

    /* send file in 8KB block - last block may be smaller */
	while (	(ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
		dummy = write(fd,buffer,ret);
	}
	sleep(1);	/* allow socket to drain before signalling the socket is closed */
	close(fd);
	exit(1);
}

int main(int argc, char **argv)
{
    stats = statsInit(stats);
    struct timeval startTime;
    gettimeofday(&startTime, NULL);
    stats->serverStartTime = startTime.tv_sec;

	int i, port, /*pid,*/ listenfd, socketfd, hit, numThreads, bufferSize;
	socklen_t length;
	static struct sockaddr_in cli_addr; /* static = initialised to zeros */
	static struct sockaddr_in serv_addr; /* static = initialised to zeros */

	if( argc < 6  || argc > 6 || !strcmp(argv[1], "-?") ) {
		(void)printf("hint: nweb Port-Number Top-Directory\t\tversion %d\n\n"
	"\tnweb is a small and very safe mini web server\n"
	"\tnweb only servers out file/web pages with extensions named below\n"
	"\t and only from the named directory or its sub-directories.\n"
	"\tThere is no fancy features = safe and secure.\n\n"
	"\tExample: nweb 8181 /home/nwebdir &\n\n"
	"\tOnly Supports:", VERSION);
		for(i=0;extensions[i].ext != 0;i++)
			(void)printf(" %s",extensions[i].ext);

		(void)printf("\n\tNot Supported: URLs including \"..\", Java, Javascript, CGI\n"
	"\tNot Supported: directories / /etc /bin /lib /tmp /usr /dev /sbin \n"
	"\tNo warranty given or implied\n\tNigel Griffiths nag@uk.ibm.com\n"  );
		exit(0);
	}
	if( !strncmp(argv[2],"/"   ,2 ) || !strncmp(argv[2],"/etc", 5 ) ||
	    !strncmp(argv[2],"/bin",5 ) || !strncmp(argv[2],"/lib", 5 ) ||
	    !strncmp(argv[2],"/tmp",5 ) || !strncmp(argv[2],"/usr", 5 ) ||
	    !strncmp(argv[2],"/dev",5 ) || !strncmp(argv[2],"/sbin",6) ){
		(void)printf("ERROR: Bad top directory %s, see nweb -?\n",argv[2]);
		exit(3);
	}
	if(chdir(argv[2]) == -1){ 
		(void)printf("ERROR: Can't Change to directory %s\n",argv[2]);
		exit(4);
	}
    numThreads = atoi(argv[3]);
    Thread threads[numThreads];
	bufferSize = atoi(argv[4]);
	char *type = argv[5];
    BuffQueue *queue = buffQueueInit(bufferSize, type);
    initThreads(threads, numThreads, queue);
    /* Become deamon + unstopable and no zombies children (= no wait()) */
    if(fork() != 0)
		return 0; /* parent returns OK to shell */
	(void)signal(SIGCHLD, SIG_IGN); /* ignore child death */
	(void)signal(SIGHUP, SIG_IGN); /* ignore terminal hangups */
	for(i=3;i<32;i++)
		(void)close(i);		/* close open files */
	(void)setpgrp();		/* break away from process group */
	logger(LOG,"nweb starting",argv[1],getpid());
	/* setup the network socket */
	if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0)
		logger(ERROR, "system call","socket",0);

    //Should help with binding and reusing ports, doesn't seem to work.
    int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    #ifdef SO_REUSEPORT
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEPORT) failed");
    #endif

	port = atoi(argv[1]);
	if(port < 0 || port >60000)
		logger(ERROR,"Invalid port number (try 1->60000)",argv[1],0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
		logger(ERROR,"system call","bind",0);
	if( listen(listenfd,64) <0)
		logger(ERROR,"system call","listen",0);
	for(hit=1; ;hit++) {
		length = sizeof(cli_addr);
		if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
			logger(ERROR,"system call","accept",0);
//		if((pid = fork()) < 0) {
//			logger(ERROR,"system call","fork",0);
//        }
//		else {
//            if(pid == 0) { 	/* child */
//                (void)close(listenfd);
//                web(socketfd,hit); /* never returns */
//            } else { 	/* parent */
//                (void)close(socketfd);
//            }
//        }
        char buffer[BUFSIZE + 1];
        long ret = read(socketfd, buffer, BUFSIZE);
        pthread_mutex_lock(&m);
		struct timeval requestArrived;
		gettimeofday(&requestArrived, NULL);
		requestArrived.tv_sec = requestArrived.tv_sec - startTime.tv_sec;

		stats->reqArrived++;
        addToBuffQueue(queue, &socketfd, hit, ret, buffer, requestArrived.tv_sec);
        queueAccessible = TRUE;
        pthread_mutex_unlock(&m);
        pthread_cond_broadcast(&cond);
        memset(buffer, 0, sizeof(buffer));
    }

}
