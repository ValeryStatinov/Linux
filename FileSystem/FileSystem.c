#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Structures.h"//fflush(stdout) for debug

//define global constants
const uint32_t NUM_BLOCKS = 65536;
const uint16_t NUM_INODES = 4096;
const uint16_t BLOCK_SIZE = 1024;
const uint16_t INODE_SIZE = sizeof(Inode);
const uint64_t BLOCK_BITMAP_OFFSET = sizeof(Superblock);
const uint64_t INODE_BITMAP_OFFSET = sizeof(Superblock)+ NUM_BLOCKS / 8;
const uint64_t INDOES_OFFSET = sizeof(Superblock) + NUM_BLOCKS / 8 + NUM_INODES / 8;
const uint64_t INFO_BLOCKS_OFFSET = INDOES_OFFSET + INODE_SIZE * NUM_INODES;

void print(uint64_t a) {
    printf("%llu\n", a);
}

void* allocate_memory_for_filesystem() {
    size_t size = sizeof(Superblock) + NUM_BLOCKS / 8 + NUM_INODES / 8 + 
                  NUM_INODES * INODE_SIZE + NUM_BLOCKS * BLOCK_SIZE;
    void* filesystem = malloc(size);
    memset(filesystem, 0, size);
    return filesystem;
}

void create_root_directory(void* filesystem) {
    uint64_t* inode_bitmap = (uint64_t*)(filesystem + INODE_BITMAP_OFFSET);
    inode_bitmap[0] = 1;
    Inode root;
    root.type_ = Directory;
    root.num_blocks_taken_ = 0;
    uint16_t block_numbers[BLOCKS_IN_INODE];
    memcpy(root.block_numbers_, block_numbers, sizeof(block_numbers));
    memcpy(root.name_, "/\0", 2);
    root.size_ = 0;
    root.parent_dir_inode_num_ = 0;
    memcpy(filesystem + INDOES_OFFSET, &root, sizeof(root));

    Superblock* super = (Superblock*)filesystem;
    super->current_inode_ = 0;
    return;
}

uint32_t get_empty_block(void* filesystem) {
    uint64_t* bitmap = (uint64_t*)(filesystem + BLOCK_BITMAP_OFFSET);
    for (int i = 0; i < NUM_BLOCKS / 64; ++i) {
        for(int j = 0; j < 64; ++j) {
            if (((bitmap[i] >> j) & 1) == 0) {
                return i * 64 + j;
            }
        }
    }
    exit(-1);
}

uint16_t get_empty_inode(void* filesystem) {
    uint64_t* bitmap = (uint64_t*)(filesystem + INODE_BITMAP_OFFSET);
    for (int i = 0; i < NUM_INODES / 64; ++i) {
        for (int j = 0; j < 64; ++j) {
            if (((bitmap[i] >> j) & 1) == 0) {
                return i * 64 + j;
            }
        }
    }
    exit(-1); //returns -1 if there is no empty inodes
}

uint8_t make_directory(void* filesystem, const char* name) {
    Superblock* super = (Superblock*)filesystem;
    Inode* current = (Inode*)(filesystem + INDOES_OFFSET + super->current_inode_ * (sizeof(Inode)));
    //reserve place for new inode in bitmap
    uint16_t empty_inode_num = get_empty_inode(filesystem);
    if (empty_inode_num == 0) {
        exit(-1);
    }
    uint64_t* inodes_bitmap = (uint64_t*)(filesystem + INODE_BITMAP_OFFSET);
    inodes_bitmap[empty_inode_num / 64] = inodes_bitmap[empty_inode_num / 64] | (1 << (empty_inode_num % 64));

    //create inode for directory
    Inode new_dir;
    new_dir.type_ = Directory;
    uint16_t block_numbers[BLOCKS_IN_INODE];
    memcpy(new_dir.block_numbers_, block_numbers, sizeof(block_numbers));
    memcpy(new_dir.name_, name, strlen(name));
    memcpy(new_dir.name_ + strlen(name), "\0", 1);
    new_dir.size_ = 0;
    new_dir.num_blocks_taken_ = 0;
    new_dir.parent_dir_inode_num_ = super->current_inode_;
    memcpy(filesystem + INDOES_OFFSET + empty_inode_num * sizeof(Inode), &new_dir, sizeof(Inode));

    //create new infoblock
    DirectoryInfoBlock blk;
    memcpy(blk.name_, name, strlen(name));
    memcpy(blk.name_ + strlen(name), "\0", 1);
    blk.inode_num_ = empty_inode_num;

    //tell current inode that new directory is created in it
    
    printf("%d ", current->num_blocks_taken_);
    uint16_t fillable_space = BLOCK_SIZE - BLOCK_SIZE % sizeof(DirectoryInfoBlock);
    uint16_t empty_space = fillable_space - current->size_ % fillable_space;
    if (empty_space >= sizeof(DirectoryInfoBlock) && empty_space != fillable_space) {
        uint16_t last_block_num = current->block_numbers_[current->num_blocks_taken_ - 1];
        memcpy(filesystem + INFO_BLOCKS_OFFSET + last_block_num * BLOCK_SIZE + fillable_space - empty_space,
            &blk, sizeof(DirectoryInfoBlock));
    }
    else {
        if (current->num_blocks_taken_ == BLOCKS_IN_INODE - 1) {
            exit(-1); //error
        }
        uint32_t empty_block_num = get_empty_block(filesystem);
        uint64_t* blocks_bitmap = (uint64_t*)(filesystem + BLOCK_BITMAP_OFFSET);
        blocks_bitmap[empty_block_num / 64] = blocks_bitmap[empty_block_num / 64] | (1 << (empty_block_num % 64));
        current->block_numbers_[current->num_blocks_taken_] = empty_block_num;
        current->num_blocks_taken_ += 1;
        memcpy(filesystem + INFO_BLOCKS_OFFSET + empty_block_num * BLOCK_SIZE, &blk, sizeof(DirectoryInfoBlock));
    }
    current->size_ += sizeof(DirectoryInfoBlock);
    return 0; //success
}

char* list_dir(void* filesystem) { ///!!! ALLOCATES MEMORY VIA MALLOC !!!
    Superblock* super = (Superblock*)filesystem;
    Inode* current = (Inode*)(filesystem + INDOES_OFFSET + super->current_inode_ * (sizeof(Inode)));
    char* output = malloc(current->num_blocks_taken_ * sizeof(DirectoryInfoBlock));
    memcpy(output, "\0", 1);
    uint16_t bytes_to_read = current->size_;
    uint16_t fillable_space = BLOCK_SIZE - BLOCK_SIZE % sizeof(DirectoryInfoBlock);
    for (int i = 0; i < current->num_blocks_taken_; ++i) {
        uint16_t block_num = current->block_numbers_[i];
        DirectoryInfoBlock* blk = (DirectoryInfoBlock*)(filesystem + INFO_BLOCKS_OFFSET + block_num * BLOCK_SIZE);
        uint16_t num_dir_info_blocks_to_read;

        //set how many DirectoryInfoBlocks to read in BLOCK
        if (bytes_to_read / sizeof(DirectoryInfoBlock) > fillable_space / sizeof(DirectoryInfoBlock)) {
            num_dir_info_blocks_to_read = fillable_space / sizeof(DirectoryInfoBlock);
        }
        else {
            num_dir_info_blocks_to_read = bytes_to_read / sizeof(DirectoryInfoBlock);
        }
        for (int j = 0; j < num_dir_info_blocks_to_read; ++j) {
            strcat(output, blk[j].name_);
            uint16_t content_inode_num = blk[j].inode_num_;
            Inode* content_inode = (Inode*)(filesystem + INDOES_OFFSET + content_inode_num * sizeof(Inode));
            if (content_inode->type_ == Directory) {
                strcat(output, "/");
            }
            strcat(output, " ");
            bytes_to_read -= sizeof(DirectoryInfoBlock);
        }
    }
    return output;
}

void change_directory_absolute(void* filesystem, const char* path) {

}

void change_directory_relative(void* filesystem, const char* path) {
    uint16_t path_index = 0;
    while (path_index < strlen(path)) {
        while(path[path_index] != '/' && path_index < strlen(path)) {

        }

    }
}

void change_directory(void* filesystem, const char* path) {
    if (path[0] == '/') {
        change_directory_absolute(filesystem, path);
    }
    else {
        change_directory_relative(filesystem, path);
    }
    return;
}



void* init_filesystem() {
    void* filesystem = allocate_memory_for_filesystem();
    Superblock super;
    super.num_blocks_ = NUM_BLOCKS;
    super.num_inodes_ = NUM_INODES;
    super.block_size_ = BLOCK_SIZE;
    super.inode_size_ = INODE_SIZE;
    super.num_free_blocks_ = NUM_BLOCKS;
    super.num_free_inodes_ = NUM_INODES - 1;
    memcpy(filesystem, &super, sizeof(super));

    uint64_t block_bitmap_init[NUM_BLOCKS / 64 + 1] = {};
    uint64_t inode_bitmap_init[NUM_INODES / 64 + 1] = {};
    memcpy(filesystem + BLOCK_BITMAP_OFFSET, block_bitmap_init, sizeof(block_bitmap_init));
    memcpy(filesystem + INODE_BITMAP_OFFSET, inode_bitmap_init, sizeof(inode_bitmap_init));

    create_root_directory(filesystem);

    Inode* root = (Inode*)(filesystem + INDOES_OFFSET);
    return filesystem;
}

void free_allocated_filesystem_memory(void* filesystem) {
    free(filesystem);
}