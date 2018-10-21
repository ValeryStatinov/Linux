#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Structures.h"

//define global constants

void* allocate_memory_for_filesystem();

void create_root_directory(void* filesystem);

uint32_t get_empty_block(void* filesystem);

uint16_t get_empty_inode(void* filesystem);

void release_inode(void* filesystem, uint16_t inode_num);

void release_block(void* filesystem, uint32_t block_num);

char* list_dir(void* filesystem);

int get_inode_number(void* filesystem, const char* name, uint16_t current_inode_num);

uint8_t change_directory_from_inode(void* filesystem, const char* path, uint16_t inode_num);

uint8_t change_directory(void* filesystem, const char* path);

uint8_t create_file(void* filesystem, char* name, inode_type type);

uint8_t write_to_FS_file(void* filesystem, char* name, char* data);

uint8_t is_file(void* filesystem, char* name);

char* read_from_FS_file(void* filesystem, const char* name);

uint8_t import_file(void* filesystem, char* inner_name, const char* outer_name);

void delete_file(void* filesystem, uint16_t file_inode_num);

void delete_directory(void* filesystem, uint16_t dir_inode_num);

uint8_t delete_item(void* filesystem, const char* name);

void* init_filesystem();

void* open_filesystem();

void save_filesystem(void* filesystem);

void free_allocated_filesystem_memory(void* filesystem);

#endif //__FILESYSTEM_H__