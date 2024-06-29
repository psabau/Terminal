#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>

#define READ_BUFFER_SIZE 1024

#define ERROR 'e'

char *readline(FILE *f) {
    int current_pos = ftell(f);

    fseek(f, 0, SEEK_END);
    int end = ftell(f);

    fseek(f, current_pos, SEEK_SET);

    char read_buffer[READ_BUFFER_SIZE];
    memset(read_buffer, 0, READ_BUFFER_SIZE);

    int read_bytes = fread(read_buffer, READ_BUFFER_SIZE - 1, 1, f);
    char *read_line = NULL;
    for (size_t i = 0; i < READ_BUFFER_SIZE - 1 && current_pos<=end; ++i) {
        if (read_buffer[i] == '\n' || i == end - 1) {
            read_line = (char *) malloc((i + 1) * sizeof(char));
            memcpy(read_line, read_buffer, i);
            read_line[i] = '\0';

            fseek(f, current_pos + i + 1, SEEK_SET);

            break;
        }
    }

    return read_line;
}

void uniq_simple(FILE *file){

    char * aux = readline(file);
    fprintf(stderr, "%s\n", aux);
    char * line = readline(file);
    while(line != NULL){
        if(strcmp(aux,line) != 0)
            fprintf(stderr, "%s\n", line);
        aux = line;
        line = readline(file);
    }
    free(line);
    free(aux);
}

void uniq_i(FILE* file){
    char * aux = readline(file);
    fprintf(stderr, "%s\n", aux);
    char * line = readline(file);
    while(line != NULL){
        if(strcasecmp(aux,line) != 0)
            fprintf(stderr, "%s\n", line);
        aux = line;
        line = readline(file);
    }
    free(line);
    free(aux);
}

void uniq_d(FILE* file){
    char * aux = readline(file);
    fprintf(stderr, "%s\n", aux);
    char * line = readline(file);
    int contor = 0;
    while(line != NULL){
        if(strcmp(aux,line) == 0) {
            if(contor == 1)
                fprintf(stderr, "%s\n", line);
            contor++;
        }
        else
            contor = 0;
        aux = line;
        line = readline(file);
    }
    free(line);
    free(aux);
}

void uniq_u(FILE* file){
    char * aux = readline(file);
    char * line = readline(file);
    int contor = 0;
    while(line != NULL){
        if(strcmp(aux,line) == 0) {
            contor++;
        }
        else{
            if(contor == 0)
                fprintf(stderr, "%s\n", aux);
            contor = 0;
        }
        aux = line;
        line = readline(file);
        if(line == NULL && contor ==0){
            fprintf(stderr, "%s\n", aux);
        }

    }
    free(line);
    free(aux);
}

int count_lines(char *file){
    int input_file = 0;
    int line = 0;
    char character;

    input_file = open(file, O_RDONLY);

    if(input_file== -1)// if it doesn't exist
        return -1;

    while(read(input_file, &character, 1))
        if(character == '\n'){
            line++;
        }

    close(input_file);

    return line;
}

int input_file_position(int argc, char **argv, int ignore){
    //ignore- i ignore -v,-q etc
    int input_position = 1;
    if(argc >1){
        while((count_lines(argv[input_position])== -1) || (input_position == ignore) || (strcmp(argv[input_position],"") == 0)){
            input_position++;

            if(input_position == argc)
                break;
        }
    }
    if(input_position == argc)
        return -1;
    else
        return input_position;
}

int main(int argc, char **argv) {

    int input_position = input_file_position(argc,argv,0);
    if(input_position == -1){
        fprintf(stderr,"Check input file.\n");
        return -1;
    }
    FILE* f = fopen(argv[input_position],"r");

    if(argc == 2){
        uniq_simple(f);
    }else if (argc>2){
        switch(argv[1][1]){
            case 'i':
                uniq_i(f);
                break;

            case 'd':
                uniq_d(f);
                break;

            case 'u':
                uniq_u(f);
                break;

            default:
                fprintf(stderr, "Unknown command");
                break;
        }
    }
    return 0;
}