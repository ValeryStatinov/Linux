#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "Structures.h"
#include "FileSystem.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void ls(void* filesystem) {
    char* ls = list_dir(filesystem);
    printf("%s\n", ls);
    free(ls);
    return;
}

void mkdir(void* filesystem, char* name) {
    create_file(filesystem, name, Directory);
    return;
}

void cd(void* filesystem, char* path) {
    if (change_directory(filesystem, path) == 1) {
        printf(ANSI_COLOR_RED "No such directory or it is a file\n" ANSI_COLOR_RESET);
    }
    return;
}

void touch(void* filesystem, char* name) {
    if (create_file(filesystem, name, File) == 1) {
        printf("Cannot create file due to lack of inodes or due to lack of blocks in inode\n");
    }
    return;
}

void cat(void* filesystem, char* name) {
    if (is_file(filesystem, name) == 1) {
        printf(ANSI_COLOR_RED "Such file doesn't exist or it is a directory\n" ANSI_COLOR_RESET);
        return;
    }
    char* out = read_from_FS_file(filesystem, name);
    printf("%s\n", out);
    free(out);
    return;
}

void rm(void* filesystem, char* name) {
    if (delete_item(filesystem, name) == 1) {
        printf(ANSI_COLOR_RED "No such file or directory\n" ANSI_COLOR_RESET);
    }
    return;
}

void import(void* filesystem, char* inner_name, char* outer_name) {
    if (import_file(filesystem, inner_name, outer_name) == 1) {
        printf(ANSI_COLOR_RED "No such file %s\n" ANSI_COLOR_RESET, outer_name);
    }
}

int main() {
    void* filesystem = open_filesystem();
    char command[10];
    char argument[256];
    char second_argument[256];
    char input[266];
    printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);

    while(fgets(input, 266, stdin) != 0){
        int num_arguments = sscanf(input, "%s %s %s", command, argument, second_argument);
        if (!strcmp(command, "ls")){
            ls(filesystem);
            printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
        }

        else if (!strcmp(command, "mkdir")) {
            if (num_arguments != 2) {
                printf("Wrong number of arguments\n");
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }
            else {
                mkdir(filesystem, argument);
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }  
        }

        else if (!strcmp(command, "cd")) {
            if (num_arguments != 2) {
                printf("Wrong number of arguments\n");
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }
            else {
                cd(filesystem, argument);
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }  
        }

        else if (!strcmp(command, "touch")) {
            if (num_arguments != 2) {
                printf("Wrong number of arguments\n");
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }
            else {
                touch(filesystem, argument);
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }  
        }

        else if (!strcmp(command, "cat")) {
            if (num_arguments != 2) {
                printf("Wrong number of arguments\n");
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }
            else {
                cat(filesystem, argument);
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }  
        }

        else if (!strcmp(command, "rm")) {
            if (num_arguments != 2) {
                printf("Wrong number of arguments\n");
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }
            else {
                rm(filesystem, argument);
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }  
        }

        else if (!strcmp(command, "import")) {
            if (num_arguments != 3) {
                printf("Wrong number of arguments\n");
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }
            else {
                import(filesystem, argument, second_argument);
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
            }
        }

        else if (!strcmp(command, "help")) {
            printf("Avaliable commands:\n"
                "ls - list directory\n"
                "mkdir [name] - make directory\n"
                "cd [path] - change directory\n"
                "touch [name] - create empty file\n"
                "cat [name] - display contents of file\n"
                "rm [file/dir] - delete file of directory"
                "import [inner_name] [outer_name] - brings copy of outer_name file to your file system"
                "to inner_name file. If inner_name file doesn't exist then it will be created in current directory\n"
                "exit - use it to finish working with file system and save all changes\n"
                "!!! WITHOUT EXIT COMMAND ALL CHANGES WILL BE LOST\n"
                );
                printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
        }
        else if (!strcmp(command, "exit")) {
            save_filesystem(filesystem);
            break;
        }

        else {
            printf(ANSI_COLOR_RED "Wrong command, use help to know about avaliable commands\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR_YELLOW "ValeryStatinovFS$ " ANSI_COLOR_RESET);
        }
    }
    free_allocated_filesystem_memory(filesystem);
    return 0;
}