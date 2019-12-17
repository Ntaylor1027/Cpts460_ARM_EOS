#include "ucode.c"

#define BLKSIZE 1024
enum TRUTH_VALUES
{
    FALSE,
    TRUE
};

// Built in commands that will need special handling
// others can be executed as normal using exec()
char *built_in_commands[] = {
    "cd",
    "pwd",
    "chname",
    "logout",
    0};

/*
    Name: grab_bi_command
    Description: Validates a command is a built in command. If it is built in send the built_in_commands index
*/
int grab_bi_command(char *command)
{
    int n = 0;
    char *check_command = built_in_commands[0];
    while (check_command)
    {
        if (strcmp(check_command, command) == 0)
            return n;
        n++;
        check_command = built_in_commands[n];
    }
    return -1;
}

/*
    Name: process_built_in 
    Description: run built in commands and return if we need to logout
*/
int process_built_in(int n_cmd, char *arguments[16])
{
    switch (n_cmd)
    {
    case 0: // cd
        if (arguments[1] == 0)
        {
            chdir("/");
        }
        else
        {
            chdir(arguments[1]);
        }
        return FALSE;
        break;

    case 1: // pwd
        pwd();
        return FALSE;
        break;

    case 2: // chname
        chname(arguments[1]);
        return FALSE;
        break;

    case 3: // logout
        printf("Logging out ... Have a nice day!\n\r");
        return TRUE;

    default:
        break;
    }
}

/*
    Name: get_command
    Description: grab the command from the buf and remove whitespace if needed.
*/
int get_command(char buf[BLKSIZE])
{
    char *remove_whitespace;
    gets(buf);
    if (buf[0] == FALSE) // No valid command
    {
        return FALSE;
    }
    else // Valid command but remove any leading whitespace
    {
        remove_whitespace = buf;
        while (*remove_whitespace == ' ')
        {
            remove_whitespace++;
        }
        return TRUE;
    }
}

/*
    Name: scan_pipes
    Description: scan from right to left for pipe and return right side of command and if the pipe was found
*/
int scan_pipes(char *buf, char **right_side)
{
    char *pipe_searcher;
    int found_pipe = FALSE;
    *right_side = 0;
    pipe_searcher = buf;
    while (*pipe_searcher != 0) // Set pipe_searcher to the end of the buffer
    {
        pipe_searcher++;
    }
    while (pipe_searcher != buf) // search from right to left for a pipe in the buffer 
    {
        if (*pipe_searcher == '|')
        {
            
            found_pipe = TRUE;                         
            *pipe_searcher = 0;                            // null the pipe found
            pipe_searcher++;                               // grab the right side of the pipe
            for (; *pipe_searcher == ' '; pipe_searcher++) // iterate over spaces
                ;
            *right_side = pipe_searcher; // ship the right side of the pipe so it can be exec()
            return found_pipe;
            break;
        }
        pipe_searcher--;
    }
    // no pipe found
    return found_pipe;
}

/*
    Name: run_cmd
    Description: grab serial port and open it for terminal 
*/
int run_cmd(char *command)
{
    char *arguments[16];
    char token_buf[BLKSIZE], command_to_exec[BLKSIZE];
    strcpy(token_buf, command);
    int n_args = tokenParse(token_buf, arguments, ' '); 
    int command_end = n_args; // Find the end of the command

    for (int i = 0; i < n_args; i++) // look for redirects and handle them
    {
        if (strcmp(arguments[i], "<") == 0) // handle read 
        {
            if (command_end > i)
                command_end = 1;
            close(0); // close input
            open(arguments[i + 1], 0); // open file as input
            break;
        }
        else if (strcmp(arguments[i], ">>") == 0)
        {
            if (command_end > i) 
                command_end = 1;
            close(1); // close writer
            open(arguments[i + 1], O_WRONLY | O_CREAT | O_APPEND); // open file as output
            break;
        }
        else if (strcmp(arguments[i], ">") == 0)
        {
            if (command_end > i)
                command_end = 1;
            close(1); // close writer
            open(arguments[i + 1], O_WRONLY | O_CREAT); // open file as writer
            break;
        }
    }
    memset(command_to_exec, 0, BLKSIZE);
    // build execution string
    strcpy(command_to_exec, arguments[0]);
    for (int i = 1; i < n_args; i++)
    {
        strcat(command_to_exec, " ");
        strcat(command_to_exec, arguments[i]);
    }
    if (exec(command_to_exec) < 0) // run command
        exit(1);
    return 1;
}

/*
    Name: run_pipe
    Description: Recursively scan for pipes and run the right side of the pipe with the parent proc and
                 the left side of the command to a child process
*/
int run_pipe(char *buf, int *right_pipe)
{
    int left_pipe[2], proc_id;
    char *right_cmd;


    if (right_pipe) // right pipe is a WRITER and we need to change where we write
    {
        
        close(right_pipe[0]); // close old writer
        close(1); // closer where terminal writes
        dup(right_pipe[1]); // set new writer
        close(right_pipe[1]);
    }
    
    int pipe_exists = scan_pipes(buf, &right_cmd); // find pipe from right command

    if (pipe_exists == TRUE)
    {
        
        if (pipe(left_pipe) < 0) // make a new pipe
            exit(1);
        proc_id = fork(); // make a new process
        if (proc_id < 0)
            exit(1);
        if (proc_id) // parent runs next command
        {
            close(left_pipe[1]);
            close(0);
            dup(left_pipe[0]);
            close(left_pipe[0]);
            run_cmd(right_cmd);
        }
        else
        {
            run_pipe(buf, left_pipe); // child runs left command
        }
    }
    else
    {
        printf("pipe did not exists run cmd\n\r");
        run_cmd(buf);
    }
    return 1;
}

int main(int argc, char *argv)
{
    char buf[BLKSIZE], token_buf[BLKSIZE];
    char *arguments[16];
    int n_args = 0, is_built_in = -1, proc_id, status, valid_cmd = FALSE;

    while (TRUE)
    {
        printf("*********** Noah's sh %d **********\n\r", getpid());
        valid_cmd = get_command(buf); // Grab command
        if (!valid_cmd)
            continue; // skip invalid commands
        strcpy(token_buf, buf); 
        n_args = tokenParse(token_buf, arguments, ' '); // grab arguments of command
        is_built_in = grab_bi_command(arguments[0]);    // check if command is built in
        if (is_built_in > 0)
        {
            // Run built in process and exit if needed
            switch (process_built_in(is_built_in, arguments))
            {
            case FALSE:
                continue;
                break;

            case TRUE:
                exit(0);
                break;
            }
        }
        else
        {
            printf("parent sh %d: forks a child\n", getpid());
            proc_id = fork();  // fork a child

            if (proc_id)  // Parent forks a child and waits
            {
                printf("parent sh %d: wait for child %d to die\n", getpid(), proc_id);
                proc_id = wait(&status);
                printf("sh %d: child %d exit status = %x\n", getpid(), proc_id, status);
                continue;
            }
            else // child runs the command 
            {
                printf("child sh %d running : cmd=%s\n", getpid(), buf);
                run_pipe(buf, 0);
            }
        }
    }
}
