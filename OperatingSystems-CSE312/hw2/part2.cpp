#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAXSIZE_2MB (2 * 1024 * 1024)
#define MAXSIZE_4MB (4 * 1024 * 1024)

#define MAXNUMBEROFFILE 512


typedef struct DirectoryEntry {
    char filename[128];
    char parent[128];
    time_t last_modification;    // when write happens it is updated
    int size;   
    int first_block;            
    int directory;          // 1 if it is a directory, 0 if it is a file
    int exist;              // 1 if the entry is valid, 0 if it is not
    char permissions[3];  // rw or r- or -w or --
    char password[20];      
    DirectoryEntry(const char filename[], const char parent[], time_t last_modification, int size, int first_block, int directory);
    DirectoryEntry();
} DirectoryEntry;

typedef struct SuperBlock { 
    int block_size;             // Block size in bytes
    int number_of_blocks;       // Total number of blocks
    int free_blocks;            // Number of free blocks
    int fat_blocks;             // Number of blocks used by FAT
    int directory_blocks;       // Number of blocks used by directory

    SuperBlock(int block_size, int number_of_blocks, int free_blocks, int fat_blocks, int directory_blocks);
    SuperBlock();
} SuperBlock;



DirectoryEntry::DirectoryEntry(const char filename[], const char parent[], time_t last_modification, int size, int first_block, int directory) {
    strcpy(this->filename, filename);
    strcpy(this->parent, parent);
    this->last_modification = last_modification;
    this->size = size;
    this->first_block = first_block;
    this->directory = directory;
    this->exist = 1;
    strcpy(this->permissions, "rw");  // Default permissions
    memset(this->password, 0, sizeof(this->password));  // No password by default
}

DirectoryEntry::DirectoryEntry() {
    this->exist = 0;
    strcpy(this->permissions, "rw");
    memset(this->password, 0, sizeof(this->password));
}

SuperBlock::SuperBlock(int block_size, int number_of_blocks, int free_blocks, int fat_blocks, int directory_blocks) {
    this->block_size = block_size;
    this->number_of_blocks = number_of_blocks;
    this->free_blocks = free_blocks;
    this->fat_blocks = fat_blocks;
    this->directory_blocks = directory_blocks;
}

SuperBlock::SuperBlock() {}

int getMaxSize(int block_size) {
    switch (block_size) {
        case 512:
            return MAXSIZE_2MB;
        case 1024:
            return MAXSIZE_4MB;
        case 2048:
            printf("********USE 0.5 or 1 KB BLOCK SIZE ******\n");
            exit(-1);
        case 4096:
            printf("********USE 0.5 or 1 KB BLOCK SIZE ******\n");
            exit(-1);
        default:
            printf("********USE 0.5 or 1 KB BLOCK SIZE ******\n");
            exit(-1);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Insufficient number of elements...\n");
        exit(-1);
    }

    double block_size_d = atof(argv[1]);

    block_size_d *= 1024; // Convert to bytes
    int block_size = (int)block_size_d;
    printf("Block size: %d bytes\n", block_size);

    if (block_size != 512 && block_size != 1024) {
        printf("Invalid block size. Choose 0.5, 1.\n");
        exit(-1);
    }

    int max_size = getMaxSize(block_size);
    int number_of_blocks = max_size / block_size; // Number of blocks is calculated
    int fat_blocks = ((number_of_blocks * sizeof(int)) + block_size - 1) / block_size; // Number of FAT blocks is calculated
    int directory_blocks = ((MAXNUMBEROFFILE * sizeof(DirectoryEntry)) + block_size - 1) / block_size; // Number of directory blocks is calculated
    int free_blocks = ((number_of_blocks * sizeof(int)) + block_size - 1) / block_size; // Number of free blocks is calculated

    char* filename = argv[2];
    char rootName[128] = "/";
    char rootParent[128] = "BASE";

    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("File could not be created...\n");
        exit(-1);
    }

    SuperBlock superBlock(block_size, number_of_blocks, free_blocks, fat_blocks, directory_blocks);
    DirectoryEntry rootDir(rootName, rootParent, 0, 0, 0, 1);

    fwrite(&superBlock, sizeof(SuperBlock), 1, file);

    int free_table[number_of_blocks];
    for (int i = 0; i < number_of_blocks; i++) {
        if (i < 1 + free_blocks + fat_blocks + directory_blocks)
            free_table[i] = 0; // NOT FREE
        else
            free_table[i] = 1; // FREE
        fwrite(&free_table[i], sizeof(int), 1, file);
    }

    int fat_table[number_of_blocks];
    for (int i = 0; i < number_of_blocks; i++) {
        fat_table[i] = -1;
        fwrite(&fat_table[i], sizeof(int), 1, file);
    }

    fwrite(&rootDir, sizeof(DirectoryEntry), 1, file);

    fseek(file, (1 + free_blocks + fat_blocks + directory_blocks) * block_size, SEEK_SET);
    char buffer[block_size];
    memset(buffer, 0, block_size);
    for (int i = 0; i < number_of_blocks - fat_blocks - directory_blocks - free_blocks - 1; i++) {
        fwrite(buffer, sizeof(char), block_size, file);
    }

    fclose(file);

    // Verify the file size
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("File could not be opened...\n");
        exit(-1);
    }
    fseek(file, 0, SEEK_END);   
    long fileSize = ftell(file);
    fclose(file);
    printf("File size: %ld bytes\n", fileSize);

    return 0;
}
