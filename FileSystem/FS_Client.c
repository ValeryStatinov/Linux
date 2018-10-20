#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "Structures.h"
#include "FileSystem.c"

int main() {
    void* filesystem = init_filesystem();
    make_directory(filesystem, "fuck15");  
    char* ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);
    free_allocated_filesystem_memory(filesystem);
    return 0;
}