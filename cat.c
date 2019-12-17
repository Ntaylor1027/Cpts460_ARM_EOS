#include "ucode.c"
#define ENTER 13
#define NULL 0
#define NEWLINE 12

char buf[1024];

/*
    Name: cat_terminal
    Description: Grab the users input and spit it back out to them on enter
*/
int cat_terminal()
{
    while (gets(buf)) // Grab user input from STDIN
    {
        if (buf[0] == NULL || buf[0] == ENTER) // If we reach a null or enter key exit program
            break;
        printf(buf);   
        printf("\n\r");
    }
    return 0;
}
/*
    Name: cat_file
    Description: Read in a file and print it to the user
*/
int cat_file()
{
    int file_desc = 0, bytes_read;
    char *char_ptr;
    file_desc = open(argv[1], O_RDONLY); // open file for reading 
    if (file_desc < 0) // Validate file opened correctly
        exit(0);

    char r = '\r'; // address will be needed to print the content onto terminal
    char new_line = '\n';
    while (bytes_read = read(file_desc, buf, 1024)) // read all of the file
    {
        buf[bytes_read] = 0; // null terminate the buffer to use as a string
        char_ptr = buf; // set character pointer onto buffer
        if (file_desc) // We are able to continue reading
        {
            for (int i = 0; i < bytes_read; i++) // Write each character to the screen
            {
                write(1, &buf[i], 1); 
                if (buf[i] == '\n')
                    write(1, &r, 1);
            }
        }
        else
        {
            char_ptr = buf; //write the last byte to the screen
            if (*char_ptr == '\r')
                write(1, &new_line, 1);
            else
                write(1, char_ptr, 1);
        }
    }
    close(file_desc);
    return 0;
}

int main(int argc, char *argv[])
{
    
    printf("********** Noah's cat **********\n\r");
    switch (argc)
    {
        case 1:
            // No file to cat just input
            cat_terminal();
            exit(0);
            break;
    
        default:
            // cat a file
            cat_file();
            exit(0);
                break;
    }
}