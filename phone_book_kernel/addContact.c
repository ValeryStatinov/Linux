#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 6) {
        fprintf(stderr, "Error! Wrong number of arguments. Expected 5, got %d.\n", argc - 1);
        return -1;
    }
    char buff_to_send[128] = "add ";
    int i;
    for (i = 1; i < 6; ++i) {
        strcat(buff_to_send, argv[i]);
        strcat(buff_to_send, " ");
    }

    int filedescr = open("/dev/lkm_example", O_WRONLY);
    if (filedescr < 0) {
        fprintf(stderr, "Error! Can't open file.\n");
        return 1;
    }
    
    write(filedescr, buff_to_send, strlen(buff_to_send));
    return 0;
}