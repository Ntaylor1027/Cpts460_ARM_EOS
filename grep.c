#include "ucode.c"
#define BLKSIZE 1024

enum TRUTH_VALUES
{
    FALSE,
    TRUE
};

int main(int argc, char *argv[])
{
    int file_desc, n_line = 0;
    char line[BLKSIZE];
    int searching = 1;
    printf("********** Noah's grep **********\n\r");
    //check if we are using stdin, a file, or what not
    switch (argc)
    {
    case 1: // Not enough arguments
        return -1;
        break;

    case 2: // using STDIN
        file_desc = 0;
        break;

    default: // using infile
        file_desc = open(argv[2], O_RDONLY); // open file and validate it opened correctly
        if (file_desc < 0)
            return -1;
    }
    // while we are searching for the pattern
    while (searching == 1)
    {
        //clears our line.
        memset(line, 0, BLKSIZE);
        searching = grab_compare_line(file_desc, line); // grab a line for comparing against pattern
        n_line++;
        if (searching == TRUE && check_pattern(line, argv[1]) == TRUE) // if pattern found
        {
            printf("%d: %s\n", n_line, line); // display pattern line
        }
    }
    close(file_desc);
    return 0;
}

/*
    Name: grab_compare_line
    Description: Goes through our infile_fd and reads byte by byte and places it into our output line
*/
int grab_compare_line(int infile_fd, char *line)
{
    int bytes_read;
    for (int i = 0; i < BLKSIZE; i++)
    {
        bytes_read = read(infile_fd, &line[i], 1);
        if (bytes_read < 1 || line[i] == 0) // Cannot read anymore
            return 0;

        else if ((line[i] == '\n') || line[i] == '\r') // grabbed a entire line
        {
            line[i + 1] = 0;
            return 1;
        }
    }
}

/*
    Name: check_pattern
    Description: validates if a substring exists in a string
*/
int check_pattern(char *line, char *pattern)
{
    char *check;
    check = strstr(line, pattern);
    if (check)
        return TRUE;
    return FALSE;
}