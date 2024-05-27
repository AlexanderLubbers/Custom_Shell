#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TOKEN_DELIMETER " \t\r\n\a\""
// important: the size of a pinter type depends on system architecture
// ex: a pointer is 8 bytes on a 64 bit system
// ex: a pointer is 4 bytes on a 32 bit system
//  this stores the current input
char *line;
// a double pointer is commonly used to reppresent an array
// in this case the double pointer represents an array of strings
// that stores parsed tokens / arguments
char **args;
// variable that determines the status of the command that got executed
int status = 1;
// builtin shell functions
int shell_cd(char **args)
{
    printf("cd command called\n");
}
int shell_help(char **args)
{
    printf("help command called \n");
}
int shell_exit(char **args)
{
    exit(EXIT_SUCCESS);
}

// list of builtin shell commands and then their coresponding functions
char *builtin_string[] = {
    "cd",
    "help",
    "exit"};
int (*builtin_functions[])(char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit};

// fp (file pointer) points to a FILE object that gets the stream that the operation will be used on
char *read_line(FILE *fp, size_t size)
{
    char *input;
    int ch;
    size_t length = 0;

    input = malloc(sizeof(char) * size);
    if (!input)
    {
        // perror prints a message to the standard error stream which can be seen in the terminal
        perror("Shell: allocation error");
        exit(EXIT_FAILURE);
    }
    // the while loop will end once fgetc returns EOF marking the end of a file
    while (1)
    {
        // fgetc gets the input one character at a time
        ch = fgetc(fp);
        if (feof(fp) || ch == '\n')
        {
            break;
        }
        // the charcter gotten from fgetc is appended to a string
        input[length++] = ch;
        // if the length of the input is equal to the size of the string, then
        // more space needs to be allocated
        if (length == size)
        {
            input = realloc(input, sizeof(char) * (size += 32));
            if (!input)
            {
                perror("Shell: reallocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
    // a string in c should always be terminated with the null character
    input[length++] = '\0';
    // free any of the string's unused space
    input = realloc(input, sizeof(char) * length);
    return input;
}
char **parse_line(char *input)
{
    int size = 1024;
    int counter = 0;
    char **tokens = malloc(size * sizeof(char));
    if (!tokens)
    {
        perror("Shell: allocation error");
        exit(EXIT_FAILURE);
    }
    // strtok splits up strings based on a delimeter and then returns the next token
    // it needs to be called in a loop to get all tokens and then returns NULL when
    // there are no more tokens
    char *token = strtok(input, TOKEN_DELIMETER);
    while (token != NULL)
    {
        tokens[counter] = token;
        counter++;
        // if the amount of tokens exceeds the space allocated for it, then more space needs to be allocated
        if (counter >= size)
        {
            size += 1024;
            char **tokens = realloc(tokens, size);
            if (!tokens)
            {
                perror("Shell: allocation error");
                exit(EXIT_FAILURE);
            }
        }
        // the string arguement nees to be supplied to only the first call, after that NULL can be passed in
        token = strtok(NULL, TOKEN_DELIMETER);
    }
    tokens[counter] = NULL;
    return tokens;
}
int execute_builtin(char **arguments)
{

    for (int i = 0; i < sizeof(builtin_string) / sizeof(char *); i++)
    {
        // strcmp compares each string character by character
        if (strcmp(args[0], builtin_string[i]) == 0)
        {
            // calls the function through the function pointer and passes in the arguments
            // and then returns the result (whether the function was successful or not)
            int result = (*builtin_functions[i])(arguments);
            return result;
        }
    }
}
int main()
{
    do
    {
        printf(">> ");
        line = read_line(stdin, 1024);
        if (!line)
        {
            perror("Shell: error processing input");
            exit(EXIT_FAILURE);
        }
        args = parse_line(line);
        status = execute_builtin(args);
        // free input variable and args to avoid memory leaks
        free(line);
        free(args);
        if (status <= 0)
        {
            printf("status is less than 0 so the shell process should quit");
            // exit will end the program
            exit(status);
        }
    } while (status);
}
