// Assignment name:        picoshell
// Expected files:         picoshell.c
// Allowed functions:      close, fork, wait, exit, execvp, dup2, pipe
// ___________________________________________________________________

// Write the following function:

// int    picoshell(char **cmds[]);

// The goal of this function is to execute a pipeline. It must execute each
// commands of cmds and connect the output of one to the input of the
// next command (just like a shell).
// e
// Cmds contains a null-terminated list of valid commands. Each rows
// of cmds are an argv array directly usable for a call to execvp. The first
// arguments of each command is the command name or path and can be passed
// directly as the first argument of execvp.

// If any error occur, The function must return 1 (you must of course
// close all the open fds before). otherwise the function must wait all child
// processes and return 0. You will find in this directory a file main.c which
// contain something to help you test your function.


// Examples:
// ./picoshell /bin/ls "|" /usr/bin/grep picoshell
// picoshell
// ./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
// squblblb

// ___________________________________________________________________

// Old summary by a student:
// You are given a main function. It converts received arguments into cmds array
// of strings. When there is a pipe the commands after the pipe are in the next
// array of strings. You have to create a pipeline using the cmds you receive from
// the main, and execute them. If there is any error the function should return 1.
// Close all FFS before returning. If the cmds executed successfully wait all
// child processes and return 0.

#include <unistd.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
    int i = 0;
    int fd[2];
    int pfd = -1;
    int pid;

    while (cmds[i])
    {
        if (cmds[i + 1])
        {
            if (pipe(fd) == -1)
            {
                if (pfd != -1)
                    close(pfd);
                return (1);
            }
        }
        pid = fork();
        if (pid == -1)
        {
            if (pfd != -1)
                close(pfd);
            if (cmds[i + 1])
            {
                close(fd[0]);
                close(fd[1]);
            }
            return (1);
        }
        if (pid == 0)
        {
            if (cmds[i + 1])
                dup2(fd[1], 1);
            if (pfd != -1)
                dup2(pfd, 0);
            if (pfd != -1)
                close(pfd);
            if (cmds[i + 1])
            {
                close(fd[0]);
                close(fd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        if (pfd != -1)
            close(pfd); 
        if (cmds[i + 1])
        {
            pfd = fd[0];
            close(fd[1]);
        }
        i++;
    }
    while (wait(NULL) != -1)
    {
    }
    return (0);
}
