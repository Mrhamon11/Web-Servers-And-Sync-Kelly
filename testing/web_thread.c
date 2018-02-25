//
// Created by hamon11 on 2/20/18.
//

//#include "web_thread.h"
//
//void *threadFunc(void *pArg){
//    int *p = (int*)pArg;
//    int myNum = *p;
//    printf("Thread number %d\n", myNum);
//    return 0;
//}
//
//void threads(int numThreads){
//    int i;
//    int index[numThreads];
//    pthread_t tid[numThreads];
//    for(i = 0; i < numThreads; i++){
//        index[i] = i;
//        pthread_create(&tid[i], NULL, threadFunc, &index[i]);
//    }
//    for(i = 0; i < numThreads; i++){
//        pthread_join(tid[i], NULL);
//    }
//}

