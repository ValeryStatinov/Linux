#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Error! Wrong number of arguments. Expected 1, got %d.\n", argc - 1);
        return -1;
    }

    char buff_to_send[32] = "get ";
    strcat(buff_to_send, argv[1]);

    int filedescr = open("/dev/lkm_example", O_RDWR);
    if (filedescr < 0) {
        fprintf(stderr, "Error! Can't open file.\n");
        return 1;
    }
    
    write(filedescr, buff_to_send, strlen(buff_to_send));
    char data[128];
    read(filedescr, data, 128);
    printf("%s", data);

    printf("\n");
    return 0;
}