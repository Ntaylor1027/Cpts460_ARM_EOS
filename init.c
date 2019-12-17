/*
    This is the start of our operating system
    
    1. When EOS starts the initial process P0 is handcrafted
    2. P0 creates a child P1 by loading the /bin/init file as its Umode Image
    3. When P1 runs it executes the init program in User mode
        - P1 plays the same role as the INIT process
*/

/**************** init.c file ****************/
#include "ucode.c"
int console, s0, s1; // Processes created during the init used for Serial ports and qemu
/*
    Name: parent()
    Description: Creating the parent process for each console either /dev/tty0, /dev/ttyS0, /dev/ttyS1.
*/
int parent() //P1's Code
{
    int pid, status;
    // This is the parent process
    while (1)
    {
        printf("Noah's INIT: wait for Zombie child\n");
        pid = wait(&status);
        // Process creation determined
        if (pid == console) // if console login process dies
        {
            printf("Noah's INIT: forks a new console login\n");
            console = fork(); // fork a new console process
            if (console)
                continue;
            else
            {
                exec("login /dev/tty0"); // running a new console login process
            }
        }
        if (pid == s0) // if s0 is the child forked, fork again
        {
            s0 = fork();
            if (s0)
                continue;
            else
                exec("login /dev/ttyS0");
        }
        if (pid == s1) //if s1 is child forked, fork again
        {
            s1 = fork();
            if (s1)
                continue;
            else
                exec("login /dev/ttyS1");
        }
        printf("Noah's INIT: I just buried an orphan child proc %d (RIP child proc you will be missed)\n");
    }
}
main()
{
    int in, out;                       // file descriptors used for terminal I/O
    // Open our file descriptors
    in = open("/dev/tty0", O_RDONLY);  // file descriptors 0
    out = open("/dev/tty0", O_WRONLY); // for display to console
    printf("Noah's INIT: fork a login proc on console\n");
    console = fork(); 
    if (console) // create P1 for the INIT process
    {
        printf("Noah's INIT: fork is going to login onto ttyS0\n");
        s0 = fork(); 
        if (s0) // if the console is created, connect to another process ethir on ttyS1  or ttyS2
        {
            s1 = fork();
            if (s1)
                parent();
            else
                exec("login /dev/ttyS1");
        }
        else
            exec("login /dev/ttyS0");
    }
    else // child: exec to login on tty0
        exec("login /dev/tty0");
}