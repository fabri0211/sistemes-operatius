#ifndef __PROCESS__
#define __PROCESS__

#include <stdbool.h>

// Process
// Ready (P): The process is waiting for the scheduler. 
// Running (E): The process consumes CPU 
// Blocked (W): The process does input/output operations. 
// Finished (F): The process finishes its work.
enum processStatus{Ready, Running, Bloqued, Finished};


typedef struct _process
{
    // Informació d'entrada necessària per identificar i planificar un procés
    int id;
    char* name;
    int burst; 
    int priority;
    int arrive_time;           
    // Informació de sortida del planificador
    int* lifecycle;    // Vector d'unitats de temps. Cada posició indicarà l'estat del procés en un instant de temps determinat.    
    int waiting_time;  //Temps espera    
    int return_time;   //Temps de retorn      
    int response_time; //Temps de resposta
    bool completed;         
} Process;

void printProcess(Process proces);
void cleanProcess(Process proces);

// Inicialitza un procés a partir d'una línia separada per tokens (tipus csv)
Process initProcessFromTokens(char* line, char* separator);

// Mètodes necessaris per ordenar les cues utilitzant qsort
int compareArrival(const void *s1, const void *s2);
int compareBurst(const void *s1, const void *s2);
int comparePriority(const void *s1, const void *s2);

char* procToString(Process* p);
#endif