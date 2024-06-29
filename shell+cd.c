#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


char **command_matrix(char *input){
    char **command = malloc(1024 * sizeof (char *));
    char *parsed;
    int i = 0;

    parsed = strtok(input, " ");
    while(parsed != NULL){
        command[i] = parsed;
        i++;

        parsed = strtok(NULL, " ");
    }
    command[i] = NULL;
    return command;
}
int changeDirectory(char** string){
    if (string[1] == NULL) {
        chdir(getenv("HOME"));
        return 1;
    }
    else{
        if (chdir(string[1]) == -1) {
            printf("Couldn't cd to: %s\n", string[1]);
            return -1;
        }
    }
    return 0;
}

int getCommand(char* command) {
    if (strcmp(command, "uniq") == 0)
        return 1;
    if (strcmp(command, "tail") == 0)
        return 2;
    if (strcmp(command, "cd") == 0)
        return 3;
    if (strcmp(command, "help") == 0)
        return 4;
    if (strcmp(command, "version") == 0)
        return 5;

    return 0;
}

int main(int argc, char **argv) {
    char *input, **command;
    pid_t child_pid;
    int status;

    signal(SIGINT, SIG_IGN); // shell does not stop afer ctrl c
    system("clear");
    using_history();

    while (1) {
	
        input = readline("user> ");
        add_history(input);
        
        command = command_matrix(input);

        if(command[0] == NULL){
            free(input);
            free(command);
            continue;
        }else if((strcmp(command[0], "exit") == 0)){
            exit(1);
        }

        int command_id = getCommand(command[0]);

        if(command_id == 3){
            if(changeDirectory(command) < 0)
                fprintf(stderr,"Not ok %s:",command[1]);
            continue;
        }

        child_pid = fork();
        if(child_pid <0){
            fprintf(stderr,"Fork failed!\n");
            exit(1);
        }
        if(child_pid == 0){
            signal(SIGINT, SIG_DFL);
            switch (command_id) {
                // UNIQ COMMAND
                case 1: {
                    execv("/home/paula/Desktop/uniq",command);
                    break;
                }
                    // TAIL COMMAND
                case 2: {
                    execv("/home/paula/Desktop/tail",command);
                    break;
                }
                    // CD COMMAND
                    // case 3: {
                    //execv("/home/paula/Desktop/cd",command);
                    //  break;
                    //}
                    // HELP COMMAND
                case 4: {
                    printf("IDk. Good luck\n");
                    break;
                }
                    // VERSION COMMAND
                case 5: {
                    printf("Version: 1\n");
                    break;
                }
                default:
                    if (execvp(command[0], command)) {
                        fprintf(stderr, "Command failed:%s\n", argv[0]);
                        exit(1);
                        break;
                    }
            }
        }else{
            waitpid(child_pid, &status, WUNTRACED);
        }
        
        free(input);
        free(command);
    }
    return 0;
}