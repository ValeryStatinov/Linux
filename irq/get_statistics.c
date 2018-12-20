#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int filedescr = open("/dev/keyboard_1min_statistics", O_RDWR);
    if (filedescr < 0) {
        fprintf(stderr, "Error! Can't open file.\n");
        return 1;
    }

    char stat[32];
    read(filedescr, stat, 32);
    printf("%s", stat);

    printf("\n");
    return 0;
}