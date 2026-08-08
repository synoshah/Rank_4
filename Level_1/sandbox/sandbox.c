// Assignment name  : sandbox
// Expected files   : sandbox.c
// Allowed functions: fork, waitpid, exit, alarm, sigaction, kill, printf, strsignal,
// errno, sigaddset, sigemptyset, sigfillset, sigdelset, sigismember
// --------------------------------------------------------------------------------------

// Write the following function:

// #include <stdbool.h>
// int sandbox(void (*f)(void), unsigned int timeout, bool verbose);

// This function must test if the function f is a nice function or a bad function, you
// will return 1 if f is nice, 0 if f is bad or -1 in case of an error in your function.

// A function is considered bad if it is terminated or stopped by a signal (segfault, abort...),
// if it exit with any other exit code than 0 or if it times out.

// If verbose is true, you must write the appropriate message among the following:
// "Nice function!
// "
// "Bad function: exited with code <exit_code>
// "
// "Bad function: <signal description>
// "
// "Bad function: timed out after <timeout> seconds
// "

// You must not leak processes (even in zombie state, this will be checked using wait).

// We will test your code with very bad functions.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

void alarm_handler(int sig)
{
    (void)sig;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    struct sigaction sa;
    int pid;
    int status;
    int code;

    sa.sa_handler = alarm_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM, &sa, NULL) == -1)
        return (-1);
    pid = fork();
    if(pid == -1)
        return (-1);
    if(pid == 0)
    {
        f();
        exit(0);
    }
    alarm(timeout);
    if(waitpid(pid, &status, WUNTRACED) == -1)
    {
        if(errno != EINTR)
            return (-1);
        
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        if(verbose)
            printf("Bad function: timed out after %u seconds\n", timeout);
        return (0);
    }
    alarm(0);
    if(WIFEXITED(status))
    {
        code = WEXITSTATUS(status);
        if(code == 0)
        {
            if(verbose)
                printf("Nice function!\n");
            return (1);
        }
        if(verbose)
            printf("Bad function: exited with code %d\n", code);
        return (0);
    }
    if(WIFSIGNALED(status))
    {
        if(verbose)
            printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
        return (0);
    }
    if(WIFSTOPPED(status))
    {
        if(verbose)
            printf("Bad function: %s\n", strsignal(WSTOPSIG(status)));
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        return (0);
    }
    return (-1);
}
