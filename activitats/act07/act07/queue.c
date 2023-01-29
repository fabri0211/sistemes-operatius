#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "process.h"
#include <string.h>
#include "log.h"

Process** queue;
size_t elements;

void init_queue(){
    queue = malloc(1 * sizeof(Process*));
    elements = 0;
}

size_t get_queue_size(){
    return elements;
}

int enqueue(Process* process){
    if (queue != NULL){
        queue[elements] = process;
        elements++;
        queue = realloc(queue,(elements+1)*sizeof(Process*));
        queue[elements]=NULL;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

Process* dequeue(){
    if (elements <= 0){
        log_debug("There are no elements to dequeue.");
        return NULL;
    } else {
        Process* process = queue[0];
        elements--;
        queue = realloc(queue,(elements+1)*sizeof(Process));
        for(int p=1; p<=elements; p++){
            queue[p-1]=queue[p];
        }
        return process;
    }
}

void cleanQueue(){
    free(queue);
}

Process* transformQueueToList(){
    Process* _list = malloc(sizeof(elements-1)*sizeof(Process));
    for (int i=0; i<elements; i++){
        _list[i]=*queue[i];
    }
    return _list;
}

void setQueueFromList(Process* list){
    for (int i=0; i<elements; i++){
        *(queue[i])=list[i];
    }
}








