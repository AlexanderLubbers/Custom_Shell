#include <stdio.h>
#include <stdlib.h>
char *line;
char **args;
int status = 1;

// https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
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
            input = realloc(input, sizeof(char) * (size += 2));
            if (!input)
            {
                perror("Shell: reallocation error");
            }
        }
    }
    // a string in c should always be ended with a new line character (escape sequence)
    input[length++] = '\n';
    // free any of the string's unused space
    input = realloc(input, sizeof(char) * length);
    return input;
}
char **parse_line()
{
}
int execute_line()
{
}
int main()
{
    do
    {
        printf(">> ");
        line = read_line(stdin, 1024);
        // args = parse_Line(input);
        // status = execute_line(args);
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
