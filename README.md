# Custom_Shell
This is a custom Unix like shell written in C that runs on windows. I got inspiration for this project and borrowed some of the code from:

https://medium.com/@santiagobedoa/coding-a-shell-using-c-1ea939f10e7e
# Capabilities
This shell has some builtin functions and can also run some non builtin commands
## Builtins
### cd: 
* type in ```cd``` followed by a space and the name of the directory that you want to go into and then hit enter
* type in ```cd ..``` and then hit enter to go back a directory

### help:
* type in ```help``` and then hit enter to recieve tips on how to use the builtin commands that come with this shell
* This command takes no inputs
### exit:
* type in ```exit``` and then hit enter to exit out of the shell
* this command takes no inputs
### directory:
* type in ```directory``` and then hit enter to display the current working directory
* this command takes no inputs
### mkdir:
* type in ```mkdir``` folllowed by a space and then the name of the new directory that you want to create and then hit enter
* to create a new directory with the specified name
### echo:
* type in ```echo``` followed by a series of words and hit enter to print out those words to standard output
* ex: ```echo hello world!```
* result:
```
hello
world!
```
### ls:
* type in ```ls``` and the hit enter to list all items in the current working directory
### newfile:
* type in ```newfile``` followed by a space and the name of the file to be created
* ex: ```newfile helloworld.js```
### removefile:
* type in ```removefile``` followed by a space and the name of the file you want to delete
* if you want to follow through with your action, enter in yes. If not, then type in no and hit enter to abort the process
* ex: ```removefile helloworld.js```
### removedirectory: 
* type in ```removedirectory``` followed by a space and the name of the directory you want to delete
* if you want to follow through with your action, enter in yes. If not, then type in no and hit enter to abort the process

These tips can also be viewed while using the shell by typing in ```help``` and hitting enter.
## Non-Builtins
The shell also has the capability to run non-builtin commands. for example:
```
git --version
```
outputs:
```
git version 2.43.0.windows.1
```
