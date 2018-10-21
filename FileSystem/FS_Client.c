#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "Structures.h"
#include "FileSystem.c"

int main() {
    void* filesystem = init_filesystem();
    create_file(filesystem, "fuck1", File);
    create_file(filesystem, "dir", Directory);
    char* ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);

    delete_item(filesystem, "dir");
    /*
    char* cat = read_from_FS_file(filesystem, "file");
    printf("%s\n", cat);
    free(cat);*/
    ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);
    

    free_allocated_filesystem_memory(filesystem);
    return 0;
}