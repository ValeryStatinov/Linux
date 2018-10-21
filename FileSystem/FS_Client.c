#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "Structures.h"
#include "FileSystem.c"

int main() {
    void* filesystem = init_filesystem();
    make_directory(filesystem, "fuck15");
    make_directory(filesystem, "fuck15");  
    make_directory(filesystem, "dir");
    char* ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);
    change_directory(filesystem, "dir");
    make_directory(filesystem, "underdir");
    ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);
    change_directory(filesystem, "..");
    ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);
    free_allocated_filesystem_memory(filesystem);
    return 0;
}