#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

#define true 		1
#define false		0

#define bytes		'c'
#define lines		'l'
#define quiet		'q'
#define verbose		'u'
#define number		'n'
#define ERROR		'e'

#define STRLEN 1024

int line_count(char *file_name)
{
    int i_file = 0;
    int line = 0;
    char ch;

    i_file = open(file_name, O_RDONLY);

    if (i_file == -1)
        return -1;

    while(read(i_file, &ch, 1))
        if (ch == '\n')
            line++;

    close(i_file);

    return line;
}

int byte_count(char *file_name)
{
    int i_file = 0;
    int byte = 0;
    char ch;

    i_file = open(file_name, O_RDONLY);

    if (i_file == -1)
        return -1;

    while(read(i_file, &ch, 1))
        byte++;

    close(i_file);

    return byte;
}

int parse_number(char *string)
{
    char  *s1 = malloc(STRLEN*sizeof(char));
    strcpy(s1,string);
    if (s1[0] == '-')
    {
        for (int i = 0; i < strlen(s1); i++)
        {
            s1[i] = s1[i+1];
        }
    }
    return atoi(s1);
}

int is_number(char num)
{
    if(num >= 48 && num <= 57)
        return true;
    else
        return false;
}

char get_command(char *string)
{
    if ((strcmp(string, "-c") == 0) || (strcmp(string, "--bytes") == 0))
        return bytes;

    if ((strcmp(string, "-n") == 0) || (strcmp(string, "--lines") == 0))
        return lines;

    if ((strcmp(string, "-q") == 0) || (strcmp(string, "--quiet") == 0) || (strcmp(string, "--silent") == 0))
        return quiet;

    if ((strcmp(string, "-v") == 0) || (strcmp(string, "--verbose") == 0))
        return verbose;

    if (((string[0] == '-') && (is_number(string[1]))) || (is_number(string[0])))
        return number;

    return ERROR;
}


int get_input_file_pos(int argc, char *argv[], int ignore)
{
    int input_position = 1;

    if (argc > 1)
    {
        while ((line_count(argv[input_position]) == -1) || (input_position == ignore) || (strcmp(argv[input_position], "") == 0))
        {
            input_position++;

            if (input_position == argc)
                break;
        }
    }

    if (input_position == argc)
        return -1;
    else return input_position;
}

void get_bytes(int no_bytes, int total_bytes, char *file_name)
{
    int i_file = 0;
    int byte = 0;
    char ch;

    no_bytes = total_bytes - no_bytes;

    i_file = open(file_name, O_RDONLY);

    while(read(i_file, &ch, 1))
    {
        byte++;
        if (byte >= no_bytes)
            putc(ch, stdout);
    }

    close(i_file);

}


void get_lines(int no_lines, int total_lines, char *file_name)
{
    int i_file = 0;
    int line = 0;
    char ch;

    no_lines = total_lines - no_lines;

    i_file = open(file_name, O_RDONLY);

    while(read(i_file, &ch, 1))
    {
        if (ch == '\n')
            line++;
        if (line >= no_lines)
            if (!((line == no_lines) && (ch == '\n')))
                putc(ch, stdout);

    }

    close(i_file);
}

int unknown_command(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
        if ((get_command(argv[i]) == ERROR) && (line_count(argv[i]) == -1))
        {
            fprintf(stderr, "Invalid file or command: %s\n", argv[i]);
            return true;
        }
    return false;
}


void do_verbose(int argc, char *argv[], char *file_name)
{
    int i = 0;
    int k;

    for (k = 1; k < argc; k++)
        if ((get_command(argv[k]) == quiet) || (get_command(argv[k]) == verbose))
            i = k;

    if (get_command(argv[i]) == verbose)
        printf("==> %s <==\n", file_name);
}

int user_specify_verbose(int argc, char *argv[])
{
    int k = 0;

    for (int i = 1; i < argc; i++)
    {
        switch (get_command(argv[i]))
        {
            case quiet:
                k++;
                break;

            case verbose:
                k++;
                break;
        }
    }

    if (k > 0)
        return true;
    else
        return false;
}

int user_specify_number(int argc, char *argv[])
{
    int k = 0;

    for (int i = 1; i < argc; i++)
    {
        switch (get_command(argv[i]))
        {
            case lines:
                k++;
                break;

            case bytes:
                k++;
                break;
            case number:
                if (argv[i][0] == '-')
                    k++;
                break;
        }
    }

    if (k > 0)
        return true;
    else
        return false;
}

int user_specify_help(int, char *[]);

int main(int argc, char *argv[])
{
	if (unknown_command(argc,argv))
		return -1;

	int  input_position = get_input_file_pos(argc, argv, 0); 

	if (input_position == -1)
	{
		fprintf(stderr, "EROARE: No input file\n");
		return -1;
	}

	char 	*input_file = malloc(STRLEN*sizeof(char));

	int  	file_position = get_input_file_pos(argc, argv, input_position); 

    int verbose_specified = user_specify_verbose(argc, argv);
    int no_lines_specified = user_specify_number(argc, argv);

	int do_command = 0; //command_order
	if (no_lines_specified)
	{
		int counter;
		for (counter = 1; counter < argc; counter++)
			switch (get_command(argv[counter]))
				{
					case lines:
						do_command = counter;
						break;

					case bytes:
						do_command = counter;
						break;

					case quiet:
						break;
					case verbose:
						break;

					case number:
						if (argv[counter][0] == '-')
							do_command = counter;
						break;
				}
	}

	while (input_position != -1)
	{
		input_file = strcpy(input_file, argv[input_position]);

		int no_lines = line_count(input_file);
		int no_bytes = byte_count(input_file);


		if (no_lines != -1)
		{
			if (no_lines_specified)
			{
				switch (get_command(argv[do_command]))
				{
					int counter, error;
					case lines:
						error = false;

						for (counter = 0; counter < strlen(argv[do_command+1]); counter++)
							if (!(is_number(argv[do_command+1][counter])))
							{
								error = true;
								break;
							}
						if (!error)
						{
							if ((!verbose_specified) && (file_position != -1))
								printf("==> %s <==\n", input_file);
							else do_verbose(argc, argv, input_file);

							get_lines(atoi(argv[do_command+1]), no_lines, input_file);
						}
						else fprintf(stderr, "%s:Invalid number of lines\n", argv[do_command+1]);

						break;

					case bytes:
						error = false;

						for (counter = 0; counter < strlen(argv[do_command+1]); counter++)
							if (!(is_number(argv[do_command+1][counter])))
							{
								error = true;
								break;
							}

						if (!error)
						{
							if ((!verbose_specified) && (file_position != -1))
								printf("==> %s <==\n", input_file);
							else do_verbose(argc, argv, input_file);

							get_bytes(atoi(argv[do_command+1]), no_bytes, input_file);
						}

						else fprintf(stderr, "%s: Invalid number of characters\n", argv[do_command+1]);

						break;

					case number:
						if (argv[do_command][0] == '-')
						{
							error = false;
							for (counter = 1; counter < strlen(argv[do_command]); counter++)
								if (!(is_number(argv[do_command][counter])))
								{
									error = true;
									break;
								}
							if (!error)
							{
								if ((!verbose_specified) && (file_position != -1))
									printf("==> %s <==\n", input_file);
								else do_verbose(argc, argv, input_file);

								get_lines(parse_number(argv[do_command]), no_lines, input_file);
							}
							else fprintf(stderr, "%s: Invalid number of lines\n", argv[do_command]);
						}

						break;
				}
			}
			else
			{
				if ((!verbose_specified) && (file_position != -1))
					printf("==> %s <==\n", input_file);
				else do_verbose(argc, argv, input_file);

				get_lines(10, no_lines, input_file);
			}
		}
		argv[input_position] = "";
        input_position = get_input_file_pos(argc, argv, 0);
	}

	return 0;
}