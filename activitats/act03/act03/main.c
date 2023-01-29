#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "parse.h"

int
main(int argc, char *argv[]){
        char *input = malloc(100 * sizeof(char*));

    while (1) {
        printf("%sossh%s> %s", GREEN, BLUE, WHITE);
        
        fgets(input, 100, stdin);

        parseLine(input);
    }
    return 0;
}