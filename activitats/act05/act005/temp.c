//Echo por Fabrisio Ulloa y Cesar Crespo
#include "aux.h"

int main (int argc, char* argv[]){
    int tiempo = atoi(argv[1]);
    if (tiempo == 0){
        kill(getppid(), SIGUSR1);
        return -1;
    }

    while(tiempo > 0){  
        sleep(1);
        tiempo--;
    }
    kill(getppid(), SIGUSR1);

    return 0;
}