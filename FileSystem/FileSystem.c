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
                uint32_t empty_block_num = i * 64 + j;
                uint64_t* blocks_bitmap = (uint64_t*)(filesystem + BLOCK_BITMAP_OFFSET);
                blocks_bitmap[empty_block_num / 64] = blocks_bitmap[empty_block_num / 64] | (1 << (empty_block_num % 64));
                return empty_block_num;
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
                uint16_t empty_inode_num = i * 64 + j;
                uint64_t* inodes_bitmap = (uint64_t*)(filesystem + INODE_BITMAP_OFFSET);
                inodes_bitmap[empty_inode_num / 64] = inodes_bitmap[empty_inode_num / 64] | (1 << (empty_inode_num % 64));
                return empty_inode_num;
            }
        }
    }
    return 0;
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

int get_inode_number(void* filesystem, const char* name, uint16_t current_inode_num) {
    Inode* current = (Inode*)(filesystem + INDOES_OFFSET + current_inode_num * (sizeof(Inode)));
    if (!strcmp(name, "..")) {
        return current->parent_dir_inode_num_;
    }
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
            if (!strcmp(name, blk[j].name_)) {
                return blk[j].inode_num_;
            }
            bytes_to_read -= sizeof(DirectoryInfoBlock);
        }
    }
    return -1;
}

uint8_t change_directory_from_inode(void* filesystem, const char* path, uint16_t inode_num) {
    Superblock* super = (Superblock*)filesystem;
    uint16_t begin = 0;
    uint16_t end = 0;
    char next_directory[256];
    uint16_t tmp_inode_num = inode_num;
    while (end < strlen(path)) {
        while(path[end] != '/' && end < strlen(path)) {
            ++end;
        }
        memcpy(next_directory, path + begin, end - begin);
        memcpy(next_directory + end - begin, "\0", 1);
        ++end;
        begin = end;
        int inode_num = get_inode_number(filesystem, next_directory, tmp_inode_num);
        if (inode_num != -1) {
            tmp_inode_num = inode_num;
            Inode* inode = (Inode*)(filesystem + INDOES_OFFSET + tmp_inode_num * sizeof(Inode));
            if (inode->type_ != Directory) {
                return 1; //error
            }
        }
        else {
            return 1; //error
        }
    }
    super->current_inode_ = tmp_inode_num;
    return 0; //success
}

void change_directory(void* filesystem, const char* path) {
    if (path[0] == '/') {
        change_directory_from_inode(filesystem, path + 1, 0); //from root inode
    }
    else {
        Superblock* super = (Superblock*)filesystem;
        change_directory_from_inode(filesystem, path, super->current_inode_);
    }
    return;
}

uint8_t create_file(void* filesystem, char* name, inode_type type) { //creates both files and directories
    Superblock* super = (Superblock*)filesystem;
    Inode* current = (Inode*)(filesystem + INDOES_OFFSET + super->current_inode_ * sizeof(Inode));

    //reserve place for new inode in bitmap
    uint16_t empty_inode_num = get_empty_inode(filesystem);
    if (empty_inode_num == 0) {
        return 1;
    }
    //create inode for file
    Inode new_file;
    new_file.type_ = type;
    uint16_t block_numbers[BLOCKS_IN_INODE];
    memcpy(new_file.block_numbers_, block_numbers, sizeof(block_numbers));
    memcpy(new_file.name_, name, strlen(name));
    memcpy(new_file.name_ + strlen(name), "\0", 1);
    new_file.size_ = 0;
    new_file.num_blocks_taken_ = 0;
    new_file.parent_dir_inode_num_ = super->current_inode_;
    memcpy(filesystem + INDOES_OFFSET + empty_inode_num * sizeof(Inode), &new_file, sizeof(Inode));

    //create info block
    DirectoryInfoBlock blk;
    memcpy(blk.name_, name, strlen(name));
    memcpy(blk.name_ + strlen(name), "\0", 1);
    blk.inode_num_ = empty_inode_num;

    //tell current inode that new file is created in it
    uint16_t fillable_space = BLOCK_SIZE - BLOCK_SIZE % sizeof(DirectoryInfoBlock);
    uint16_t empty_space = fillable_space - current->size_ % fillable_space;
    if (empty_space >= sizeof(DirectoryInfoBlock) && empty_space != fillable_space) {
        uint16_t last_block_num = current->block_numbers_[current->num_blocks_taken_ - 1];
        memcpy(filesystem + INFO_BLOCKS_OFFSET + last_block_num * BLOCK_SIZE + fillable_space - empty_space,
            &blk, sizeof(DirectoryInfoBlock));
    }
    else {
        if (current->num_blocks_taken_ == BLOCKS_IN_INODE - 1) {
            return 1; //error
        }
        uint32_t empty_block_num = get_empty_block(filesystem);
        current->block_numbers_[current->num_blocks_taken_] = empty_block_num;
        current->num_blocks_taken_ += 1;
        memcpy(filesystem + INFO_BLOCKS_OFFSET + empty_block_num * BLOCK_SIZE, &blk, sizeof(DirectoryInfoBlock));
    }
    current->size_ += sizeof(DirectoryInfoBlock);
    return 0; //success
}

uint8_t write_to_FS_file(void* filesystem, char* name, char* data) {
    Superblock* super = (Superblock*)filesystem;
    Inode* current = (Inode*)(filesystem + INDOES_OFFSET + super->current_inode_ * sizeof(Inode));
    int inode_num = get_inode_number(filesystem, name, super->current_inode_);
    if (inode_num == -1) {
        create_file(filesystem, name, File);
        inode_num = get_inode_number(filesystem, name, super->current_inode_);
        if (inode_num == -1) {
            return 1;
        }
    }
    Inode* file_inode = (Inode*)(filesystem + INDOES_OFFSET + inode_num * sizeof(Inode));
    if (file_inode->type_ == Directory) {
        return 1;
    }
    uint64_t num_bytes_to_write = strlen(data);
    uint64_t offset_in_data = 0;
    uint16_t empty_space = BLOCK_SIZE - file_inode->size_ % BLOCK_SIZE;
    while (num_bytes_to_write > 0) {
        if (empty_space > 0 && empty_space != BLOCK_SIZE) {
            uint16_t last_block_num = file_inode->block_numbers_[file_inode->num_blocks_taken_ - 1];
            void* destination = filesystem + INFO_BLOCKS_OFFSET + last_block_num * BLOCK_SIZE +
                (BLOCK_SIZE - empty_space);
            if (num_bytes_to_write > empty_space) {
                memcpy(destination, data, empty_space);
                num_bytes_to_write -= empty_space;
                offset_in_data += empty_space;
                file_inode->size_ += empty_space;
            }
            else {
                memcpy(destination, data, num_bytes_to_write);
                file_inode->size_ += num_bytes_to_write;
                num_bytes_to_write = 0;
            }
            
        }
        else {
            if (file_inode->num_blocks_taken_ == BLOCKS_IN_INODE - 1) {
                return 1; //error
            }
            uint32_t empty_block_num = get_empty_block(filesystem);
            file_inode->block_numbers_[file_inode->num_blocks_taken_] = empty_block_num;
            file_inode->num_blocks_taken_ += 1;
            void* destination = filesystem + INFO_BLOCKS_OFFSET + empty_block_num * BLOCK_SIZE;
            if (num_bytes_to_write > BLOCK_SIZE) {
                memcpy(destination, data + offset_in_data, BLOCK_SIZE);
                num_bytes_to_write -= BLOCK_SIZE;
                file_inode->size_ += BLOCK_SIZE;
                offset_in_data += BLOCK_SIZE;
            }
            else {
                memcpy(destination, data + offset_in_data, num_bytes_to_write);
                file_inode->size_ += num_bytes_to_write;
                num_bytes_to_write = 0;
            }
        }
    }
    return 0;
}

char* read_from_FS_file(void* filesystem, const char* name) { //!!! ALLOCATES MEMORY VIA MALLOC !!!
    Superblock* super = (Superblock*)filesystem;
    int inode_num = get_inode_number(filesystem, name, super->current_inode_);
    if (inode_num == -1) {
        exit(-1);
    }
    Inode* file_inode = (Inode*)(filesystem + INDOES_OFFSET + inode_num * sizeof(Inode));
    if (file_inode->type_ == Directory) {
        exit(-1); //error
    }
    char* output = malloc(file_inode->size_ + 1);
    memcpy(output, "\0", 1);
    uint16_t bytes_to_read = file_inode->size_;
    for (int i = 0; i < file_inode->num_blocks_taken_; ++i) {
        uint16_t block_num = file_inode->block_numbers_[i];
        char* block = (char*)(filesystem + INFO_BLOCKS_OFFSET + block_num * BLOCK_SIZE);
        if (bytes_to_read > BLOCK_SIZE) {
            strncat(output, block, BLOCK_SIZE);
            bytes_to_read -= BLOCK_SIZE;
        }
        else {
            strncat(output, block, bytes_to_read);
            bytes_to_read = 0;
        }
    }
    strcat(output, "\0");
    return output;
}

uint8_t import_file(void* filesystem, char* inner_name, const char* outer_name) {
    Superblock* super = (Superblock*)filesystem;
    Inode* current = (Inode*)(filesystem + INDOES_OFFSET + super->current_inode_ * sizeof(Inode));

    FILE *file = fopen(outer_name, "r");

    if (file == NULL)
    {
        return 1;
    }
    fseeko(file, 0, SEEK_END);
    int file_size = (int)ftell(file);
    rewind(file);
    char* file_data = malloc(file_size +1);
    memset(file_data, 0, file_size + 1);
    fread(file_data, sizeof(char), file_size, file);

    write_to_FS_file(filesystem, inner_name, file_data);
    fclose(file);
    free(file_data);
    return 0;
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