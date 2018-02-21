//
// Created by hamon11 on 2/20/18.
//

#include <stdio.h>
#include "buffer.h"

int main(){
    Queue *queue = queueInit();

    for(int i = 0; i < 10; i++){
        addToQueue(queue, i);
    }

    while(!queueIsEmpty(queue)){
        printf("%d\t", pollFromQueue(queue));
    }
}
