//Echo por Fabrisio Ulloa y Cesar Crespo

#include "aux.h"

int main(int argc, char* argv[]){
    char *code = malloc(sizeof(char) * PASS_LEN);
    code = argv[1];

    while(1){
        char contraseña[PASS_LEN];
        generar_contraseña(contraseña);

        if (strncmp(code, contraseña, PASS_LEN)){
            kill(getppid(), SIGUSR2);
        }
    }
    return 0;
}