#include "parse.h"
#include "log.h"

void parseLine(char *line) {
    char *delim = ";\n\0";
    char *rest = line;

    char *token;
    while (token = strtok_r(rest, delim, &rest)) {
        parseCommand(token);
    }
}

int parseCommand(char *command) {
    if (parseErrors(command) == 1) {
        return 0;
    }

    handleOwnCommands(command);

    char *delim = " ";
    char *arguments[10] = {NULL};

    char *token = strtok(command, delim);
    arguments[0] = token;
    if (token != NULL) {
        char *path = malloc(strlen(token));
        strcpy(path, token);

        int argIndex = 0;
        while (token != NULL) {
            arguments[argIndex] = strdup(token);
            token = strtok(NULL, delim);
            argIndex++;
        }

        pid_t pid = fork();
        int status;
        if (pid == 0) {
            execvp(path, arguments);
            exit(0);
        } else {
            waitpid(pid, &status, WUNTRACED | WCONTINUED);
            return status;
        }
    }
}

void handleOwnCommands(char *cmd) {
    if (strncmp(cmd, "cd", 2) == 0) {
        handleCdCommand(cmd);
    } else if (strncmp(cmd, "help", 4) == 0) {
        showHelp();
    } else if (strncmp(cmd, "exit", 4) == 0) {
        exit(0);
    }
}

void handleCdCommand(char *cmd) {
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    chdir(token);
}

void showHelp() {
    puts("#### GTIDIC - UDL - OSSH #################################\n"
         "#                                                        #\n"
         "# Type program names and arguments, and hit enter.       #\n"
         "# Use the man command for information on other programs. #\n"
         "#                                                        #\n"
         "##########################################################\n");
}

int parseErrors(char *command) {
    for (int i = 0; i < strlen(command); i++) {
        if (command[i] == '|' || command[i] == '<' || command[i] == '>' || command[i] == '&') {
            log_error("Error: command contains [ | < > & ]");
            return 1;
        }
    }
    return 0;
}
