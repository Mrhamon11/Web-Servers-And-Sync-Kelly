//
// Created by hamon11 on 2/20/18.
//

#include <stdio.h>
#include <stdlib.h>
#include "web_page_buffer.h"

int main(int argc, char *argv[]){
    Queue *queue = queueInit();

    for(int i = 0; i < atoi(argv[1]); i++){
        addToQueue(queue, i);
    }

    while(!queueIsEmpty(queue)){
        printf("%d\t", pollFromQueue(queue));
    }
}
