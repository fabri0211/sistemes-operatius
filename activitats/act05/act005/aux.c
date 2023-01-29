//Echo por Fabrisio Ulloa y Cesar Crespo
#include "aux.h"

void generar_contraseña(char *contraseña){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int i;
    unsigned int seed = (unsigned) getpid() * (unsigned) tv.tv_usec;
    srand(seed);

    for (i = 0; i < PASS_LEN; i++){
        char c = (char) (rand() % 26 + 'A');
        contraseña[i] = c;
    }

    contraseña[i] = '\0';
}