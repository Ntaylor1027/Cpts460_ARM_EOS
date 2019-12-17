#include "ucode.c"
#define BLKSIZE 1024

int main(int argc, char *argv[])
{
    char buf[BLKSIZE];
    int src, dest;
    int bytes_read;

    printf("********** Noah's cp **********\n\r");
    // Validate arguments
    if (argc < 3)
    {
        printf("ERROR: not enough arguments... exiting");
        return 1;
    }
    // Open src for READ and validate
    src = open(argv[1], O_RDONLY);
    if (src < 0)
        exit(0);
    // Open dest for write. If dest does not exist create then validate
    dest = open(argv[2], O_WRONLY | O_CREAT);
    if (dest < 0)
        exit(0);
    // Copie all bytes from source file to dest n
    while (bytes_read = read(src, buf, 1024))
    {
        write(dest, buf, bytes_read);
    }
    // close files
    close(src);
    close(dest);
    return 0;
}