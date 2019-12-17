#include "ucode.c"
#define ENTER 13
#define BLKSIZE 1024

enum TRUTH_VALUES
{
    FALSE,
    TRUE
};

/*
    Name: print_upper
    Description: prints an upper case character on screen
*/
void print_upper(int c)
{
    if (c >= 'a' && c <= 'z')
        mputc(c - 'a' + 'A');
    else
        mputc(c);
}

/*
    Name: grab_compare_line
    Description: Goes through our infile_fd and reads byte by byte and places it into our output line
*/
void l2u_file()
{
    int infile, outfile, bytes_read;
    char buf[BLKSIZE], *char_ptr;

    infile = open(argv[1], O_RDONLY); // open input file and validate open
    if (infile < 0)
        exit(1);

    outfile = open(argv[2], O_WRONLY | O_CREAT); // open outfile and validate open
    if (outfile < 0)
        exit(1);

    // Read all of infile in BLKSIZE chunks and print_upper() each charater
    while (bytes_read = read(infile, buf, BLKSIZE))
    {
        char_ptr = buf;
        while (char_ptr < buf + bytes_read)
        {
            print_upper(*char_ptr);
            char_ptr++;
        }
        write(outfile, buf, bytes_read);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    char character;
    printf("********** Noah's l2u ***********\n\r");
    switch (argc)
    {
        case 1:
            while ((character = getc()) != ENTER) // Grab characters until enter and print it
            {
                print_upper(character);
            }
            printf("\n\r");
            exit(0);
            break;

        case 3:
            l2u_file();
            break;

        default:
            exit(1);
            break;
    }
}