#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int success;
char input;
char* tokens;
int running = 1;
#define BUFFER_SIZE 1024

char read_line() {
    int bufsize = BUFFER_SIZE;
    int position;
    char* buffer = malloc(sizeof(char) * bufsize);
    int stop = 0;
    int c = 0;
    if (!buffer)
    {
        printf("Gecko: error allocationg memory");
        exit(EXIT_FAILURE);
    }

    while(stop == 0) {
        
        position++;
        if(position >= *buffer) {
            buffer = realloc(buffer, sizeof(char) * (bufsize*2));
            if(!buffer) {
                printf("Gecko: error allocating memory");
            }
        }
    }
    
}
char parse_line() {

}
int execute_line() {

}
int main()
{
    do
    {
        printf(">> ");
        input = read_line();
    } while (running);
    return EXIT_SUCCESS;
}