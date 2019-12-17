/*
    Login Algorithm:
        1. Close File descriptors 0,1 inherited from INIT
        2. open argv[1] 3 times for in,out,err
        3. settty(argv[1]) // Set tty name string in PROC.tty
        4. open /etc/passwd file for reading
        Loop
        5. Grab arguments for username and password
        6. if user has a valid account
        7. change uid, gid to users uid and gid
           change cwd to users home dir
           close /etc/passwd
        8. exec to program in user account
    
*/
/************** login.c **************/


#include "ucode.c"

enum TRUTH_VALUES
{
    FALSE,
    TRUE
};

// Note to self: Globals are bad thanks Andy!
char buf[1024];                     
int i = 0;
int in, out, err;                   // terminal I/O files descriptors
int password_fd;                    // file descriptor for "/etc/passwd"
int totalEntires;                   // total username/password entries in /etc/passwd
int passwordExists;                 // open password file validation
int gid, uid;                       // users gid and uid
char *vars[100], *lineEntries[100]; // username:password:gid:uid:fullname:HOMEDIR:program and line buf
char username[128], password[128];  // User login info

void close_inherited_fds();
void set_terminal_io();
void grab_login_info();
void go_to_user_command();
int validate_login();

/*
    Name: close_inherited_fds()
    Description: Close the inherited file descriptors from INIT
*/
void close_inherited_fds()
{
    close(0);
    close(1);
}

/*
    Name: set_terminal_io()
    Description: sets the terminal io file descriptors for, stdin, stdout, stderr
*/
void set_terminal_io()
{
    in = open(argv[1], 0);
    out = open(argv[1], 1);
    err = open(argv[1], 2);
}

/*
    Name: grab_login_info()
    Description: grab the username and account.
*/
void grab_login_info()
{
    printf("Username:");
    gets(username);

    printf("Password:");
    gets(password);
}

/*
    Name: go_to_user_command()
    Description: set the correct uid, changes to user's home directory 
                 and closes password file descritpor. Then executes command.
*/
void go_to_user_command()
{
    // Step 7: change uid, gid to users uid and gid
    //         change cwd to users home dir
    //         close /etc/passwd
    gid = atoi(vars[2]);
    uid = atoi(vars[3]);
    chuid(gid, uid);
    chdir(vars[5]);
    close(password_fd);

    // Step 8: exec to program in user account
    printf("Noah's Login: Welcome! %s\n", vars[0]);
    exec(vars[6]);
}

/*
    Name: validate_login()
    Description: validate a user and execute to sh or fail validation
*/
int validate_login()
{
    if (strcmp(username, vars[0]) == 0 && strcmp(password, vars[1]) == 0)
    {
        go_to_user_command();
        return 1;
    }
    else
    {
        printf("login failed\n");
        return 0;
    }
}

main(int argc, char *argv[])
{
    int ret_val;
    // login.c Upon entry argv[0] =  login, argv[1] = /dev/tty[?]
    // Must close the file descriptors in and our from init.c
    // Step 1: Close File descriptors 0,1 inherited from INIT
    close_inherited_fds();

    // Step 2: open argv[1] 3 times for in, out, and err
    set_terminal_io();

    // Step 3: settty(argv[1]) // Set tty name string in PROC.tty
    settty(argv[1]);

    // Step 4: open /etc/passwd file for reading
    password_fd = open("/etc/passwd", O_RDONLY);

    while (1) // Grab User info until correct
    {
        // Step 5: Grab arguments for username and password
        grab_login_info();

        passwordExists = read(password_fd, buf, 1024);
        if (passwordExists <= 0)
        {
            printf("There are no passwords found\n");
        }

        // Tokenize the password file to grab each line into correct vars
        // Format: username:password:gid:uid:fullname:HOMEDIR:program
        // e.g.    root:xxxxxxx:1000:0:superuser:/root:sh
        totalEntires = tokenParse(buf, lineEntries, '\n'); // Grab each account entry

        for (i = 0; i < totalEntires; i++) // for every account
        {
            tokenParse(lineEntries[i], vars, ':'); // grab account details in
                                                   // username:password:gid:uid:fullname:HOMEDIR:program format

            // Step 6: Validate User
            // username:password:uid:gid:mode:directory:execmd
            ret_val = validate_login();
            if (ret_val == 1)
                return ret_val;
        }

        // The user forgot or is trying to hack someone, either way lets reset our buffers and try again
        memset(username, 0, 128);
        memset(password, 0, 128);
        prints("login failed, try again\n");
    }
}