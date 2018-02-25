//
// Created by hamon11 on 2/20/18.
//

#include "web_thread.h"

void *threadFunc(void *pArg){
    int *p = (int*)pArg;
    int myNum = *p;
    printf("Thread number %d\n", myNum);
    return 0;
}

int threads(){
    int i;
    int index[4];
    pthread_t tid[4];
    for(i = 0; i < 4; i++){
        index[i] = i;
        pthread_create(&tid[i], NULL, threadFunc, &index[i]);
    }
    for(i = 0; i < 4; i++){
        pthread_join(tid[i], NULL);
    }
}

