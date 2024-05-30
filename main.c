#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
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
    if (args[1] == NULL)
    {
        return 1;
    }
    int result = chdir(args[1]);
    if (result == -1)
    {
        printf("Shell: invalid parameters\n");
    }
    else
    {
        return 1;
    }
}
int shell_help(char **args)
{
    printf("help command called \n");
}
int shell_exit(char **args)
{
    // exit the program
    exit(EXIT_SUCCESS);
}
int shell_display_directory(char **args)
{
    char *buffer;
    // getcwd gets the full path of the current working directory and then stores it in the buffere variable
    // the first input for getcwd is the storage location for the path which can be left NULL
    // second input specifies the maximum length for the path. an error will be reported if the maxlength is exceeded
    //_getcwd returns a pointer to the buffer which containes a string that represents the path of the current
    // working directory
    // by passing in NULL and 0 into the function, this tells the function to dynamically allocate the buffer needed
    // to store the current working directory. This means that the corrent amount of memory will automoatically be allocated
    if ((buffer = _getcwd(NULL, 0)) == NULL)
    {
        printf("Shell: directory command error");
    }
    else
    {
        printf("%s\n", buffer);
    }

    // the buffer variable must be freed from memory to prevent memory leaks
    free(buffer);
    return 1;
}
int shell_make_directory(char **args)
{
    // get the length of args
    int length = 0;
    while ((args[length] != NULL))
    {
        length++;
    }
    // check to make sure the right amount of inputs were passed in
    // if there was more than one input then an error should be displayed
    if (length > 2)
    {
        printf("Shell: too many arguments\n");
        return 1;
    }
    // create a directory using the name that was supplied
    int status = mkdir(args[1]);
    if (status)
    {
        printf("Shell: mkdir failed\n");
        return 1;
    }
    return 1;
}
int shell_echo(char **args)
{
    int x = 1;
    while (args[x] != NULL)
    {
        printf("%s\n", args[x]);
        x++;
    }
    return 1;
}
int get_directory_contents(const char *directory)
{
    WIN32_FIND_DATA file;
    HANDLE hFind = NULL;
    char path[2048];
    // create a file mask
    //"." means that everything will be grabbed
    sprintf(path, "%s\\*.*", directory);
    // verify the path
    if ((hFind = FindFirstFile(path, &file)) == INVALID_HANDLE_VALUE)
    {
        printf("path not found [%s]\n", directory);
        return 1;
    }
    do
    {
        // find first file will always return "." and ".." as the first two directories
        if (strcmp(file.cFileName, ".") != 0 && strcmp(file.cFileName, "..") != 0)
        {
            // begin building up the file path
            sprintf(path, "%s\\%s", directory, file.cFileName);
            // determine whether the entity is a file or a folder
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("Directory: %s\n", path);
            }
            else
            {
                printf("File: %s\n", path);
            }
        }

    } while (FindNextFile(hFind, &file)); // find the next file
    // cleanup
    FindClose(hFind);
    return 1;
}
int shell_ls(char **args)
{
    // get the current working directory
    char *buffer;
    buffer = _getcwd(NULL, 0);
    get_directory_contents(buffer);
    free(buffer);
    return 1;
}

// list of builtin shell commands and then their coresponding functions
char *builtin_string[] = {
    "cd",
    "help",
    "exit",
    "directory",
    "mkdir",
    "echo",
    "ls"};
int (*builtin_functions[])(char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit,
    &shell_display_directory,
    &shell_make_directory,
    &shell_echo,
    &shell_ls};

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
            // exit will end the program
            exit(status);
        }
    } while (status);
}
