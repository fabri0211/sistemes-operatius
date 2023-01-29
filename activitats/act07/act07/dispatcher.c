#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "process.h"
#include "queue.h"
#include "log.h"
#include "dispatcher.h"

int num_algorithms() {
  return sizeof(algorithmsNames) / sizeof(char *);
}

int num_modalities() {
  return sizeof(modalitiesNames) / sizeof(char *);
}


size_t initFromCSVFile(char* filename, Process** procTable){
    FILE* f = fopen(filename,"r");
    
    size_t procTableSize = 10;
    
    *procTable = malloc(procTableSize * sizeof(Process));
    Process * _procTable = *procTable;

    if(f == NULL){
      perror("initFromCSVFile():::Error Opening File:::");   
      exit(1);             
    }

    char* line = NULL;
    size_t buffer_size = 0;
    size_t nprocs= 0;
    while( getline(&line,&buffer_size,f)!=-1){
        if(line != NULL){

            Process p = initProcessFromTokens(line,";");
            if (nprocs==procTableSize-1){
                procTableSize=procTableSize+procTableSize;
                _procTable=realloc(_procTable, procTableSize * sizeof(Process));
            }
            _procTable[nprocs]=p;
            nprocs++;
        }
    }
   free(line);
   fclose(f);
   return nprocs;
}

size_t getTotalCPU(Process *procTable, size_t nprocs){
    size_t total=0;
    for (int p=0; p<nprocs; p++ ){
        total += (size_t) procTable[p].burst;
    }
    return total;
}

int getCurrentBurst(Process* proc, int current_time){
    int burst = 0;
    for(int t=0; t<current_time; t++){
        if(proc->lifecycle[t] == Running){
            burst++;
        }
    }
    return burst;
}

int run_dispatcher(Process *procTable, size_t nprocs, int algorithm, int modality){

    Process * _proclist;
    log_info("Running %s simulation", algorithmsNames[algorithm]);
    qsort(procTable,nprocs,sizeof(Process),compareArrival);
    log_debug("Sorted by arrival");
    init_queue();
    size_t duration = getTotalCPU(procTable, nprocs) +1;

    for (int p=0; p<nprocs; p++ ){
        procTable[p].lifecycle = malloc(duration * sizeof(int));
        for(int t=0; t<duration; t++){
            procTable[p].lifecycle[t]=-1;
        }
        procTable[p].waiting_time = 0;
        procTable[p].return_time = 0;
        procTable[p].response_time = 0;
        procTable[p].completed = false;
    }

    Process* selected = NULL;
    for (int t=0; t<duration-1; t++ ){

        for (int p=0; p<nprocs; p++ ){
            Process* current = &procTable[p];
            if (current->arrive_time == t)
            {
                current->lifecycle[t]=Bloqued;
                enqueue(current);
                log_info("The dispatcher enqueued the process: %s by the arrival in t: %d", current->name, t);
                log_info("Dispatcher needs to pick the next process.");                  
                           
            }
            
            if(selected!=NULL && modality==PREEMPTIVE && t>=current->arrive_time && current->completed==false){  
                if(selected->burst > current->burst || selected->priority > current->priority){
                    enqueue(selected);
                    current->lifecycle[t]=Bloqued;
                    selected=NULL;
                    }
                 
            }
        
        }
        
        if (selected == NULL && get_queue_size() > 0){            
            if(algorithm==SJF && get_queue_size()>1){
                _proclist = transformQueueToList();
                qsort(_proclist,get_queue_size(),sizeof(Process),compareBurst);
                setQueueFromList(_proclist);
                free(_proclist);
            }
            else if(algorithm==PRIORITIES && get_queue_size()>1){
                _proclist = transformQueueToList();
                qsort(_proclist,get_queue_size(),sizeof(Process),comparePriority);
                setQueueFromList(_proclist);
                free(_proclist);
            }/*
            else if (algorithm==RR && get_queue_size()>1){
                enqueue(dequeue());
            }
            */
            selected=dequeue();

        } 
        
        if (selected != NULL){
            selected->lifecycle[t]=Running;
            
            for (int p=0; p<nprocs; p++ ){
                Process* current = &procTable[p];
                
                if ( t>=current->arrive_time && current->completed==false ){
                    
                    if(current->id == selected->id  && getCurrentBurst(current,t)==0){
                        current->response_time=t-current->arrive_time;
                    }

                    if( current->id == selected->id  
                        && getCurrentBurst(current,t) == selected->burst - 1 ){
                            selected->completed=true;
                            current->completed=true;
                            current->return_time= t + 1 - current->arrive_time;
                            current->lifecycle[t+1]=Finished;                    }
                        
                    if ( current->id != selected->id ){
                            current->lifecycle[t]=Bloqued;
                            current->waiting_time++;
                        }

                    }
            } 
        }

        if(selected->completed){
            selected=NULL;
        }
        printSimulation(nprocs, procTable, (unsigned) t);
    }
    log_info(" Ending fcfs simulation...");


    printSimulation(nprocs,procTable,duration);
    printMetrics(duration-1,nprocs,procTable);

    for (int p=0; p<nprocs; p++ ){
        cleanProcess(procTable[p]);
    }
    cleanQueue();
    return EXIT_SUCCESS;

}

void printSimulation(size_t nprocs, Process *procTable, size_t duration){
    printf("%14s","== SIMULATION ");
    for (int t=0; t<duration; t++ ){
        printf("%5s","=====");
    }
    printf("\n");

    printf ("|%4s", "name");
    for(int t=0; t<duration; t++){
        printf ("|%2d", t);
    }
    printf ("|\n");
    
    for (int p=0; p<nprocs; p++ ){
        Process current = procTable[p];
            printf ("|%4s", current.name);
            for(int t=0; t<duration; t++){
                printf("|%2s",  (current.lifecycle[t]==Running ? BLUE " E" RESET:  
                        current.lifecycle[t]==Bloqued ? YELLOW " B" RESET:   
                        current.lifecycle[t]==Finished ?GREEN " F" RESET : " "));
            }
            printf ("|\n");

    }


}

void printMetrics(size_t simulationCPUTime, size_t nprocs, Process *procTable ){

    printf("%-14s","== METRICS ");
    for (int t=0; t<simulationCPUTime+1; t++ ){
        printf("%5s","=====");
    }
    printf("\n");

    printf("= Duration: %ld\n", simulationCPUTime );
    printf("= Processes: %ld\n", nprocs );

    size_t baselineCPUTime = getTotalCPU(procTable, nprocs);
    double throughput = (double) nprocs / (double) simulationCPUTime;
    double cpu_usage = (double) simulationCPUTime / (double) baselineCPUTime;

    printf("= CPU (Usage): %lf\n", cpu_usage*100 );
    printf("= Throughput: %lf\n", throughput*100 );

    double averageWaitingTime = 0;
    double averageResponseTime = 0;
    double averageReturnTime = 0;
    double averageReturnTimeN = 0;

    for (int p=0; p<nprocs; p++ ){
            averageWaitingTime += procTable[p].waiting_time;
            averageResponseTime += procTable[p].response_time;
            averageReturnTime += procTable[p].return_time;
            averageReturnTimeN += procTable[p].return_time / (double) procTable[p].burst;
    }


    printf("= averageWaitingTime: %lf\n", (averageWaitingTime/(double) nprocs) );
    printf("= averageResponseTime: %lf\n", (averageResponseTime/(double) nprocs) );
    printf("= averageReturnTimeN: %lf\n", (averageReturnTimeN/(double) nprocs) );
    printf("= averageReturnTime: %lf\n", (averageReturnTime/(double) nprocs) );

}