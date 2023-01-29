//Echo por Fabrisio Ulloa y Cesar Crespo
#include "aux.h"

void sigHandler(int);

int num_players;
char *tiempo;

time_t tiempo_inicial;
time_t tiempo_final;

pid_t temp;
pid_t *hijos;

int main (int argc, char* argv[]){
    
    if (argc != 4){
        printf("Usage: %s <num args>\n", argv[0]);
        return -1;
    }

    tiempo = argv[1];

    num_players = atoi(argv[2]);

    char *contraseña = malloc(sizeof(char) * PASS_LEN);
    contraseña = argv[3];

    signal(SIGUSR1, sigHandler);  

    printf("Inici del Room Escape [%d] (%s segons)\n", getpid(), tiempo);
    printf("--- Entren %d jugadors a la sala\n", num_players);
    printf("--- La contrasenya a endevinar és: %s\n", contraseña);
     

    hijos = (pid_t*) malloc((unsigned) num_players * sizeof(pid_t));

    switch(temp = fork()){
        case -1:
            perror("fork");
            return -1;
        case 0: ;
            char *argsv[] = {"./temp", tiempo, NULL};
            execv(argsv[0], argsv);
            exit(EXIT_SUCCESS);
        default:
            tiempo_inicial = time(NULL);
            localtime(&tiempo_inicial);
            break;
    }

    
    for (int i = 0; i < num_players; i++){
        switch(hijos[i] = fork()){
            case -1:
                perror("Error al crear el proces");
                exit(EXIT_FAILURE);
            case 0:
                printf(BLUE" EscapeRoom[%d] ha entrat el jugador[%d]\n"RESET, getppid(), getpid());
                char *argsv[] = {"./player", NULL};
                execv(argsv[0], argsv);
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }

    pause();
    return 0;
}


void sigHandler(int sig){
    if(sig == SIGUSR1){
        printf(RED"***********DING DONG\n"RESET);
        for (int i = 0; i < num_players; i++){
            printf(RED" El jugador [%d] ha quedat atrapat i surt del Escape Room [%d]\n"RESET, hijos[i], getpid());
            kill(hijos[i], SIGKILL);
        }
        printf(" Els jugadors han perdut la partida. Fins aviat.\n");
        exit(EXIT_SUCCESS);
    } 
    else if(sig == SIGUSR2){
        tiempo_final = time(NULL);
        localtime(&tiempo_final);
        for (int i = 0; i < num_players; i++){
            printf(GREEN" El jugador [%d] ha escapat i surt del Escape Room [%d]\n"RESET, hijos[i], getpid());
            kill(hijos[i], SIGKILL);
        }
        printf(" Els jugadors han guanyat la partidaen %d segons.\n", (int) difftime(tiempo_final, tiempo_inicial));
        exit(EXIT_SUCCESS);
    }
    exit(0);
}
