#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__
#include <stdio.h>
#include <stdint.h>

#define BLOCKS_IN_INODE 14

typedef struct {
    uint32_t num_blocks_;
    uint16_t num_inodes_;
    uint32_t num_free_blocks_;
    uint16_t num_free_inodes_;
    uint16_t block_size_; //in bytes
    uint16_t inode_size_; //in bytes
    uint16_t current_inode_;
} Superblock;

typedef enum { Directory, File } inode_type;
typedef struct {
    uint32_t num_blocks_taken_;
    uint16_t size_;
    inode_type type_;
    char name_[256];
    uint16_t parent_dir_inode_num_;
    uint16_t block_numbers_[BLOCKS_IN_INODE];
} Inode;

typedef struct {
    char name_[256];
    uint16_t inode_num_;
} DirectoryInfoBlock;

#endif //__STRUCTURES_H__