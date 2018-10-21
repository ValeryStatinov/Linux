#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "Structures.h"
#include "FileSystem.c"

int main() {
    void* filesystem = init_filesystem();
    create_file(filesystem, "fuck15", Directory);
    create_file(filesystem, "file", File);
    write_to_FS_file(filesystem, "file", "LALALA\n");
    write_to_FS_file(filesystem, "file", "papapam\n");
    //mport_file(filesystem, "file", "test.txt");
    char* cat = read_from_FS_file(filesystem, "file");
    printf("%s\n", cat);
    free(cat);
    char* ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);
    free_allocated_filesystem_memory(filesystem);
    return 0;
}