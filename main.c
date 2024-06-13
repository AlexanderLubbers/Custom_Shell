#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include <ctype.h>
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
    return 1;
}
int print_file()
{
    // open the help.txt file in read mode
    FILE *file = fopen("help.txt", "r");
    // if the file cannot be opened then an error needs to be returned
    if (!file)
    {
        printf("Shell: error opening help file");
        return 1;
    }
    // create a buffer to store each line of the text file
    char line[1024];
    // while there is still lines to grab, grab them and then print them out
    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }
    // close the file
    fclose(file);
    return 1;
}
int shell_help(char **args)
{
    int length = 0;
    while (args[length] != NULL)
    {
        length++;
    }
    if (length > 1)
    {
        printf("Shell: help does not accept any arguments\n");
        return 1;
    }
    print_file();
}

int shell_exit(char **args)
{
    // exit the program
    int length = 0;
    while (args[length] != NULL)
    {
        length++;
    }
    if (length > 1)
    {
        printf("Shell: the exit command does not take any inputs\n");
        return 1;
    }
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
    if (length != 2)
    {
        printf("Shell: invalid amount of arguments supplied for mkdir\n");
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
    // get the length of args
    int length = 0;
    while (args[length] != NULL)
    {
        length++;
    }
    // if the length is one then display everything in the current directory
    if (length > 2)
    {
        printf("Shell: too many inputs passed into ls\n");
        return 1;
    }
    if (length == 1)
    {
        // get the current working directory
        char *buffer;
        buffer = _getcwd(NULL, 0);
        get_directory_contents(buffer);
        free(buffer);
        return 1;
    }
    else
    {
        // the contents of another directory needs to be dispayed
        // get the current directory
        char *buffer;
        if ((buffer = _getcwd(NULL, 0)) == NULL)
        {
            printf("Shell: ls command error");
        }
        // change the directory to the one supplied by the input
        int result = chdir(args[1]);
        if (result == -1)
        {
            printf("Shell: invalid parameters\n");
            return 1;
        }
        char *new_directory;
        if ((new_directory = _getcwd(NULL, 0)) == NULL)
        {
            printf("Shell: error getting directory (ls)");
        }
        get_directory_contents(new_directory);

        // change the directory back to the original one
        int second_result = chdir(buffer);
        if (second_result == -1)
        {
            printf("Shell: directory invalid (ls)");
        }
        free(buffer);
        return 1;
    }
}
int shell_newfile(char **args)
{
    int length = 0;
    while (args[length] != NULL)
    {
        length++;
    }
    // make sure that there is only two inputs
    if (length > 2)
    {
        printf("Shell: too many inputs\n");
        return 1;
    }
    if (args[1] == NULL)
    {
        printf("Shell: no name specified for file creation\n");
        return 1;
    }
    FILE *fp;
    // create a new file and open it for writing
    fp = fopen(args[1], "w");
    fclose(fp);
    if (fp == NULL)
    {
        printf("Shell: new file with name: %s could not be created\n", args[1]);
    }
    return 1;
}
char *confirmation()
{
    int max = 5;
    char *buffer = (char *)malloc(max); /* allocate buffer */
    // the buffer could not be created
    if (buffer == 0)
        return NULL;
    printf("are you sure? (yes / no)> ");

    while (1)
    {
        // skip the leading whitespace
        int c = getchar();
        if (c == EOF)
            break;
        if (!isspace(c))
        {
            // pushes it character back input stream
            ungetc(c, stdin);
            break;
        }
    }
    int i = 0;
    while (1)
    {
        // get the response from the user
        // getchar reads one character at a time from the key board
        int c = getchar();
        if (isspace(c) || c == EOF)
        { /* at end, add terminating zero */
            buffer[i] = 0;
            break;
        }
        buffer[i] = c;
        // if the buffer is full, then a larger buffer needs to be allocated
        if (i == max - 1)
        {
            max += max;
            buffer = (char *)realloc(buffer, max); /* get a new and larger buffer */
            if (buffer == 0)
                return NULL;
        }
        i++;
    }
    return buffer;
}
int shell_removefile(char **args)
{
    int length = 0;
    while (args[length] != NULL)
    {
        length++;
    }
    if (length > 2)
    {
        printf("Shell: invalid input\n");
        return 1;
    }
    char *confirm = confirmation();
    if (strcmp(confirm, "no") == 0)
    {
        printf("Shell: aborting process\n");
        return 1;
    }
    if (strcmp(confirm, "no") != 0 && strcmp(confirm, "yes") != 0)
    {
        printf("Shell: invalid input\n");
        return 1;
    }
    if (remove(args[1]) != 0)
    {
        printf("Shell: unable to delete the file\n");
    }
    return 1;
}
void delete_directory(const char *directory)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char dirSpec[MAX_PATH];
    // snprintf is used to format a string and store it in a buffer
    // %s\\* is the format string. This will tell the function that a string will be inserted at this point
    // in the string followed by \*
    // This means that everything under a current directory will be grabbed
    snprintf(dirSpec, MAX_PATH, "%s\\*", directory);

    // verify that the path the user supplied is valid
    // find first file is a windows api function that finds the first file or directory
    hFind = FindFirstFile(dirSpec, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Shell: Invalid file handle\n");
        return;
    }

    do
    {
        // ignore files that have the name "." or ".."
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
        {
            char filePath[MAX_PATH];
            snprintf(filePath, MAX_PATH, "%s\\%s", directory, findFileData.cFileName);

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // if there is another directory in the folder to be deleted, then the contents of that directory
                // needs to be deleted as well
                delete_directory(filePath);
            }
            else
            {
                // delete all the files in the directory to be deleted
                if (DeleteFile(filePath))
                {
                    printf("Deleted file: %s\n", filePath);
                }
                else
                {
                    printf("failed to delete file: %s\n", filePath);
                    return;
                }
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
    // now that all the contents in the directory have been deleted, the actually directory can now be deleted
    if (RemoveDirectory(directory))
    {
        printf("Deleted directory: %s\n", directory);
    }
    else
    {
        printf("Failed to delete directory: %s\n", directory);
    }
}
int shell_removedirectory(char **args)
{
    int length = 0;
    while (args[length] != NULL)
    {
        length++;
    }
    if (length > 2)
    {
        printf("Shell: invalid input\n");
        return 1;
    }
    char *confirm = confirmation();
    if (strcmp(confirm, "no") == 0)
    {
        printf("Shell: aborting process\n");
        return 1;
    }
    if (strcmp(confirm, "no") != 0 && strcmp(confirm, "yes") != 0)
    {
        printf("Shell: invalid input\n");
        return 1;
    }
    delete_directory(args[1]);
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
    "ls",
    "newfile",
    "removefile",
    "removedirectory"};
int (*builtin_functions[])(char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit,
    &shell_display_directory,
    &shell_make_directory,
    &shell_echo,
    &shell_ls,
    &shell_newfile,
    &shell_removefile,
    &shell_removedirectory};

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
int new_process(char **args)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Calculate the required length for the command string
    int command_len = 0;
    for (int i = 0; args[i] != NULL; i++)
    {
        command_len += strlen(args[i]) + 3; // 2 for quotes, 1 for space
    }
    // Allocate memory for the command string
    char *command = (char *)malloc(command_len + 1);
    if (command == NULL)
    {
        printf("Shell: memory allocation failed\n");
        return 1;
    }
    // Construct the command string with proper quoting
    command[0] = '\0';
    for (int i = 0; args[i] != NULL; i++)
    {
        strcat(command, "\"");
        strcat(command, args[i]);
        strcat(command, "\"");
        if (args[i + 1] != NULL)
        {
            strcat(command, " ");
        }
    }

    // Create a new process
    // input1: no module name so pass in NULL to use the command line
    // input2: pass in the command
    // input3: pass in NULL because the process handle cannot be inherited
    // input4: pass in NULL because the thread handle cannot be inherited
    // input5: set the handle inheritaicne to false
    // input6: pass in zero because there are no creating flags
    // input7: pass in NULL so the parent's environment block will be used
    // input8: pass in NULL so that the parent's starting directory is used
    // input9: pointer to the STARTUPINFO structure
    // input10: pointer to the PROCESS_INFORMATION structure
    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printf("Shell: failed to execute command\n");
        free(command);
        return 1;
    }
    // Wait until child process exits
    WaitForSingleObject(pi.hProcess, INFINITE);
    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    free(command);
    return 1;
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
    int status = new_process(arguments);
    return status;
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
