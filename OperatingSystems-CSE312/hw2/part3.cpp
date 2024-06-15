#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAXNUMBEROFFILE 512


typedef struct DirectoryEntry {
    char filename[128];
    char parent[128];
    time_t last_modification;   // when write happens
    int size;
    int first_block;
    int directory;        // 1 if it is a directory, 0 if it is a file
    int exist;
    char permissions[3];  //  rw or r- or -w or --
    char password[20];    // Password
    DirectoryEntry(char filename[], char parent[], time_t last_modification, int size, int first_block, int directory);
    DirectoryEntry();
} DirectoryEntry;

typedef struct SuperBlock {
    int block_size;
    int number_of_blocks;
    int free_blocks;
    int fat_blocks;
    int directory_blocks;
    SuperBlock(int block_size, int number_of_blocks, int free_blocks, int fat_blocks, int directory_blocks);
    SuperBlock();
} SuperBlock;


DirectoryEntry::DirectoryEntry(char filename[], char parent[], time_t last_modification, int size, int first_block, int directory) {
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

void dir(char*, char*, DirectoryEntry[]);
void mkdir(char*, char*, DirectoryEntry[]);
void rmdir(char*, char*, SuperBlock, DirectoryEntry[], int[], int[]);
void writeFile(char*, char*, char*, SuperBlock, DirectoryEntry[], int[], int[]);
void del(char*, char*, SuperBlock, DirectoryEntry[], int[], int[]);
void readFile(char*, char*, char*, SuperBlock, DirectoryEntry[], int[], const char*);
void dumpe2fs(SuperBlock, int[], int[], DirectoryEntry[]);
int isParent(char*, char*, DirectoryEntry[]);
int isExist(char**, int, DirectoryEntry[]);
int getType(char*, char*, DirectoryEntry[]);
char** arrangePath(char*, int*);
void chmodFile(char*, char*, char*, DirectoryEntry[]);
void addpwFile(char*, char*, char*, DirectoryEntry[]);

int directoryTableChanged = 0, fatTableChanged = 0, freeTableChanged = 0;
char* fileSystem;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Insufficient number of elements...\n");
        exit(-1);
    }

    fileSystem = argv[1];
    char* operation = argv[2];
    char** paths = &argv[3];

    FILE* file = fopen(fileSystem, "rb");
    if (file == NULL) {
        printf("error file desciptor");exit(1);
    }

    SuperBlock superBlock;
    fread(&superBlock, sizeof(SuperBlock), 1, file);

    int free_table[superBlock.number_of_blocks];
    fread(&free_table, sizeof(int), superBlock.number_of_blocks, file);

    int fat_table[superBlock.number_of_blocks];
    fread(&fat_table, sizeof(int), superBlock.number_of_blocks, file);

    fseek(file, sizeof(SuperBlock) + (superBlock.fat_blocks + superBlock.free_blocks) * superBlock.block_size, SEEK_SET);

    DirectoryEntry directoryTable[MAXNUMBEROFFILE];                   
    
    fread(directoryTable, sizeof(DirectoryEntry), MAXNUMBEROFFILE, file);     
    fclose(file);

    int number_of_elements = 2;
    char** arrangedPath = nullptr;
    if (argc > 3)
        arrangedPath = arrangePath(paths[0], &number_of_elements);

    if (strncmp(operation, "dir", 3) == 0) {
        if (isExist(arrangedPath, number_of_elements - 1, directoryTable)) {
            dir(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], directoryTable);
        }

    } else if (strncmp(operation, "mkdir", 5) == 0) {
        if (number_of_elements >= 1) {
            if (isExist(arrangedPath, number_of_elements - 2, directoryTable)) {
                mkdir(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], directoryTable);
            }
        }
        else {
            printf("INSUFFICIENT PATH!\n");

        }
        
    } else if (strcmp(operation, "rmdir") == 0) {
        if (isExist(arrangedPath, number_of_elements - 1, directoryTable)) {
            rmdir(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], superBlock, directoryTable, fat_table, free_table);
        }
    } else if (strcmp(operation, "write") == 0) {
        if (isExist(arrangedPath, number_of_elements - 3, directoryTable)) {
            writeFile(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], paths[1], superBlock, directoryTable, fat_table, free_table);
        }
    } else if (strcmp(operation, "del") == 0) {
        if (isExist(arrangedPath, number_of_elements - 1, directoryTable)) {
            del(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], superBlock, directoryTable, fat_table, free_table);
        }
    } else if (strcmp(operation, "read") == 0) {
        if (isExist(arrangedPath, number_of_elements - 1, directoryTable)) {
            if (argc == 5) {
                readFile(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], paths[1], superBlock, directoryTable, fat_table, nullptr);
            } else {
                readFile(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], paths[1], superBlock, directoryTable, fat_table, argv[5]);
            }
        }

    } else if (strcmp(operation, "dumpe2fs") == 0) {
        dumpe2fs(superBlock, free_table, fat_table, directoryTable);
    } else if (strcmp(operation, "chmod") == 0) {
        if (argc == 5) {
            chmodFile(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], argv[4], directoryTable);
        }
        else {
            printf("Usage: %s <file system name> chmod <path> <permissions>\n", argv[0]);
        }
    } else if (strcmp(operation, "addpw") == 0) {
        if (argc == 5) {
            addpwFile(arrangedPath[number_of_elements - 2], arrangedPath[number_of_elements - 1], argv[4], directoryTable);
        }
        else {
            printf("Usage: %s <file system name> addpw <path> <password>\n", argv[0]);
        }
    } else {
        printf("\"%s\": NOT A VALID COMMAND", operation);
    }

    file = fopen(fileSystem, "rb+");
    if (file == NULL) {
        printf("error file desciptor");exit(1);
    }
    if (directoryTableChanged == 1) {
        rewind(file);   
        fseek(file, sizeof(SuperBlock) + (superBlock.fat_blocks + superBlock.free_blocks) * superBlock.block_size, SEEK_SET);
        fwrite(directoryTable, sizeof(DirectoryEntry), MAXNUMBEROFFILE, file);
    }
    if (fatTableChanged == 1) {
        rewind(file);
        fseek(file, sizeof(SuperBlock) + (superBlock.block_size * superBlock.free_blocks), SEEK_SET);
        fwrite(fat_table, sizeof(int), superBlock.number_of_blocks, file);
    }
    if (freeTableChanged == 1) {
        rewind(file);
        fseek(file, sizeof(SuperBlock), SEEK_SET);
        fwrite(free_table, sizeof(int), superBlock.number_of_blocks, file);
    }

    fclose(file);
    return 0;
}

void dir(char* parent, char* child, DirectoryEntry directoryTable[]) {
    int fileType = getType(parent, child, directoryTable);
    if (fileType == 0) {            
        printf("%s", child);
    } else {                      
        int number_of_files = 0;
        for (int i = 0; i < MAXNUMBEROFFILE; i++) {
            if (strcmp(directoryTable[i].parent, child) == 0) {
                if (directoryTable[i].directory == 1) { // if it is a directory
                    printf("Directory name :\n %s\t", directoryTable[i].filename);
                    printf("Last modification: %s", ctime(&directoryTable[i].last_modification));
                } else {    // if it is a file
                    printf("Filename:\n %s\t", directoryTable[i].filename);
                    printf("Last modification: %s", ctime(&directoryTable[i].last_modification));
                    printf("Size: %d\t", directoryTable[i].size);
                    printf("Permissions: %s\t", directoryTable[i].permissions);
                    printf("Password: %s\n", directoryTable[i].password);
                }
                number_of_files++;
            }
        }
        if (number_of_files > 0)
            printf("\n");
    }
}

void mkdir(char* parent, char* child, DirectoryEntry directoryTable[]) {
    int fileType = getType(parent, child, directoryTable);      
    if (fileType == -1) {
        for (int i = 0; i < MAXNUMBEROFFILE; i++) {
            if (directoryTable[i].exist != 1) {
                strcpy(directoryTable[i].filename, child);
                strcpy(directoryTable[i].parent, parent);
                time(&directoryTable[i].last_modification);
                directoryTable[i].directory = 1;
                directoryTable[i].size = 0;
                directoryTableChanged = 1;
                directoryTable[i].exist = 1;
                printf("Directory created: %s/%s\n", parent, child);
                return;
            }
        }
    } else if (fileType == 0) { //  if there is a file with the same name
        printf("cannot create directory \"%s\": File exists\n", child);
    } else if (fileType == 1) {
        printf("cannot create directory \"%s\": Directory exists\n", child);
    }
}
void rmdir(char* parent, char* child, SuperBlock superBlock, DirectoryEntry directoryTable[], int fat_table[], int free_table[]) {
    int fileType = getType(parent, child, directoryTable);
    if (fileType == 0) { // it is a file
        printf("Error: \"%s\" is not a directory and cannot be removed.\n", child);
    } else if (fileType == -1) {    // if the directory does not exist
        printf("Error: \"%s\" does not exist.\n", child);
    } else {
        // if the directory is not empty, do not remove it
        int number_of_files = 0;
        for (int i = 0; i < MAXNUMBEROFFILE; i++) {
            if (strcmp(directoryTable[i].parent, child) == 0) {
                number_of_files++;
            }
        }
        if (number_of_files > 0) {
            printf("Error: \"%s\" is not empty and cannot be removed.\n", child);
            return;
         }
    }

    for (int i = 0; i < MAXNUMBEROFFILE; i++) {
        if (strcmp(directoryTable[i].parent, child) == 0) {
            if (directoryTable[i].directory == 1) {     // if it is a directory, remove it recursively
                rmdir(child, directoryTable[i].filename, superBlock, directoryTable, fat_table, free_table);
            } else {
                del(child, directoryTable[i].filename, superBlock, directoryTable, fat_table, free_table);
            }
        }
        if (strcmp(directoryTable[i].filename, child) == 0) {
            strcpy(directoryTable[i].filename, "");
            strcpy(directoryTable[i].parent, "");
            directoryTable[i].last_modification = 0;
            directoryTable[i].directory = 0;
            directoryTable[i].size = 0;
            directoryTableChanged = 1;
            directoryTable[i].exist = 0;
            break;
        }
    }
    freeTableChanged = 1;
    fatTableChanged = 1;
    printf("Directory \"%s/%s\" deleted successfully.\n", parent, child);
}


void writeFile(char* parent, char* child, char* filename, SuperBlock superBlock, DirectoryEntry directoryTable[], int fat_table[], int free_table[]) {
    int fileType = getType(parent, child, directoryTable);
    if (fileType == 1) {
        printf("\"%s\": IS A DIRECTORY!\n", child);
    } else {
        if (fileType == 0) {
            del(parent, child, superBlock, directoryTable, fat_table, free_table);
        }

        FILE* file = fopen(filename, "rb");
        if (file == NULL) {
            printf("error file desciptor");exit(1);
        }

        fseek(file, 0, SEEK_END);   
        int size = ftell(file);
        rewind(file);

        int file_blocks = (size / superBlock.block_size) + 1;
        char** fileArray = (char**) malloc(file_blocks * sizeof(char*));

        for (int i = 0; i < file_blocks; i++) {
            fileArray[i] = (char*) malloc(superBlock.block_size * sizeof(char));
            fread(fileArray[i], sizeof(char), superBlock.block_size, file);
        }

        int counter = 0;
        FILE* fptr = fopen(fileSystem, "rb+");
        if (fptr == NULL) {
            printf("error file desciptor");exit(1);
        }
        int next = -1;
        for (int i = 0; i < superBlock.number_of_blocks && counter < file_blocks; i++) {
            if (free_table[i] == 1) { // free block found
                fat_table[i] = next;
                free_table[i] = 0;
                next = i;
                fseek(fptr, sizeof(SuperBlock) + i * superBlock.block_size, SEEK_SET);
                fwrite(fileArray[file_blocks - counter - 1], sizeof(char), superBlock.block_size, fptr);
                rewind(fptr);
                counter++;
            }
        }

        if (counter != file_blocks) {
            printf("SOME PARTS OF THE FILE IS NOT WRITTEN TO \"%s\": MEMORY IS FULL!\n", child);
        }

        fclose(file);
        fclose(fptr);

        
        for (int i = 0; i < MAXNUMBEROFFILE; i++) {         
            if (directoryTable[i].exist != 1) {     // IF THE ENTRY IS EMPTY, WRITE THE FILE TO THAT ENTRY. //
                strcpy(directoryTable[i].filename, child);
                strcpy(directoryTable[i].parent, parent);
                time(&directoryTable[i].last_modification);
                directoryTable[i].directory = 0;  
                directoryTable[i].size = size;
                directoryTable[i].first_block = next;
                directoryTable[i].exist = 1;
                strcpy(directoryTable[i].permissions, "rw");  // Default permissions
                memset(directoryTable[i].password, 0, sizeof(directoryTable[i].password));  // No password
                printf("File written: %s/%s\n", parent, child);
                break;
            }
        }

        freeTableChanged = 1;
        fatTableChanged = 1;
        directoryTableChanged = 1;
    }
}

void del(char* parent, char* child, SuperBlock superBlock, DirectoryEntry directoryTable[], int fat_table[], int free_table[]) {
    int fileType = getType(parent, child, directoryTable);
    if (fileType == 1) {
        printf("\"%s\": IS A DIRECTORY!\n", child);
    } else if (fileType == -1) {
        printf("\"%s\": NO SUCH FILE OR DIR!\n", child);
    } else {
        for (int i = 0; i < MAXNUMBEROFFILE; i++) {
            if (strcmp(directoryTable[i].filename, child) == 0 && strcmp(directoryTable[i].parent, parent) == 0) {
                int current = directoryTable[i].first_block;
                int next;
                do {
                    next = fat_table[current];
                    free_table[current] = 1;
                    fat_table[current] = -1;
                    current = next;
                } while (current != -1);
                strcpy(directoryTable[i].filename, "");
                strcpy(directoryTable[i].parent, "");
                directoryTable[i].last_modification = 0;
                directoryTable[i].directory = 0;
                directoryTable[i].size = 0;
                directoryTable[i].exist = 0;
                break;
            }
        }
        freeTableChanged = 1;
        fatTableChanged = 1;
        directoryTableChanged = 1;
        printf("File deleted: %s/%s\n", parent, child);
    }
}

void readFile(char* parent, char* child, char* filename, SuperBlock superBlock, DirectoryEntry directoryTable[], int fat_table[], const char* password) {
    int fileType = getType(parent, child, directoryTable);
    if (fileType == 1) {
        printf("\"%s\": IS A DIRECTORY!\n", child);
    } else if (fileType == -1) {
        printf("\"%s\": NO SUCH FILE OR DIR!\n", child);
    } else {
        for (int i = 0; i < MAXNUMBEROFFILE; i++) {
            if (strcmp(directoryTable[i].filename, child) == 0 && strcmp(directoryTable[i].parent, parent) == 0) {
                if (directoryTable[i].permissions[0] != 'r') {
                    printf("NO READ PERMISSION FOR \"%s\"\n", child);
                    return;
                }
                if (directoryTable[i].password[0] != '\0') {
                    if (password == NULL) {
                        printf("Password required but not provided for \"%s\"\n", child);
                        return;
                    } else {
                        printf("Input password: '%s'\n", password); // Debugging
                        if (strcmp(directoryTable[i].password, password) != 0) {
                            printf("INVALID PASSWORD FOR \"%s\"\n", child);
                            return;
                        }
                    }
                }
                FILE* file = fopen(filename, "wb");
                if (file == NULL) {
                    printf("error file desciptor");exit(1);
                }
                FILE* fptr = fopen(fileSystem, "rb+");
                if (fptr == NULL) {
                    printf("error file desciptor");exit(1);
                }
                int start = directoryTable[i].first_block;
                int remainingSize = directoryTable[i].size;
                while (start != -1 && remainingSize > 0) {
                    char buffer[superBlock.block_size];
                    fseek(fptr, sizeof(SuperBlock) + start * superBlock.block_size, SEEK_SET);
                    int bytesToRead = (remainingSize > superBlock.block_size) ? superBlock.block_size : remainingSize;
                    fread(buffer, sizeof(char), bytesToRead, fptr);
                    fwrite(buffer, sizeof(char), bytesToRead, file);
                    remainingSize -= bytesToRead;
                    start = fat_table[start];
                }
                fclose(file);
                fclose(fptr);
                printf("File read: %s/%s\n", parent, child);
                break;
            }
        }
    }
}



void dumpe2fs(SuperBlock superBlock, int free_table[], int fat_table[], DirectoryEntry directoryTable[]) {
    printf("\n****************** dump e2fs ******************\n");

    int free_blocks_counter = 0, file_counter = 0, directory_counter = 0;
    int occupied_blocks_counter = 0;

    printf("Block size: %d bytes\n", superBlock.block_size);
    printf("Total number of blocks: %d\n", superBlock.number_of_blocks);

    for (int i = 0; i < superBlock.number_of_blocks; i++) {
        if (free_table[i] == 1) {
            free_blocks_counter++;
        }
    }
    printf("Number of free blocks: %d\n", free_blocks_counter);

    for (int i = 0; i < MAXNUMBEROFFILE; i++) {
        if (directoryTable[i].exist == 1) {
            if (directoryTable[i].directory == 1) {
                directory_counter++;
            } else {
                file_counter++;
            }
        }
    }
    printf("Number of files: %d\n", file_counter);
    printf("Number of directories: %d\n", directory_counter);

    printf("Occupied blocks and file names:\n");
    printf("SuperBlock occupies 1 block\n");
    occupied_blocks_counter += 1;

    printf("FAT occupies %d blocks\n", superBlock.fat_blocks);
    occupied_blocks_counter += superBlock.fat_blocks;

    printf("Directory entries occupy %d blocks\n", superBlock.directory_blocks);
    occupied_blocks_counter += superBlock.directory_blocks;

    for (int i = 0; i < MAXNUMBEROFFILE; i++) {
        if (directoryTable[i].exist == 1) {
            if (directoryTable[i].directory == 0) {
                printf("File: %s\nOccupied Blocks: ", directoryTable[i].filename);
                int start = directoryTable[i].first_block;
                while (start != -1) {
                    printf("%d ", start);
                    start = fat_table[start];
                    occupied_blocks_counter++;
                }
                printf("\n");
            } else {
                printf("Directory: %s\n", directoryTable[i].filename);
            }
        }
    }


    printf("\n****************** dump e2fs end ******************\n\n");
}

void chmodFile(char* parent, char* child, char* permissions, DirectoryEntry directoryTable[]) {
    int fileType = getType(parent, child, directoryTable);
    if (fileType == -1) {
        printf("NO SUCH FILE OR DIR \"%s/%s\"\n", parent, child);
        return;
    }
    if ( (strlen(permissions) != 3 && strlen(permissions) != 2) ||  (permissions[0] != '+' && permissions[0] != '-') || (permissions[1] != 'r' && permissions[1] != 'w') ) {
        printf("INVALID PERMISSIONS FORMAT. USE +r, -r, +w, -w or +rw, -rw\n");
        return;
    }

    for (int i = 0; i < MAXNUMBEROFFILE; i++) {
        if (strcmp(directoryTable[i].filename, child) == 0 && strcmp(directoryTable[i].parent, parent) == 0) {
            printf("Permissions before: %s/%s: %s\n", parent, child, directoryTable[i].permissions); // Debugging
            printf("Permissions to change: %s\n", permissions); // Debugging
            if (permissions[0] == '+') {
                if (permissions[1] == 'r') {
                    directoryTable[i].permissions[0] = 'r';
                } else if (permissions[1] == 'w') {
                    directoryTable[i].permissions[1] = 'w';
                }
                if (permissions[2] == 'r') {
                    directoryTable[i].permissions[0] = 'r';
                } else if (permissions[2] == 'w') {
                    directoryTable[i].permissions[1] = 'w';
                }
            } else if (permissions[0] == '-') {
                if (permissions[1] == 'r') {
                    directoryTable[i].permissions[0] = '-';
                } else if (permissions[1] == 'w') {
                    directoryTable[i].permissions[1] = '-';
                }
                if (permissions[2] == 'r') {
                    directoryTable[i].permissions[0] = '-';
                } else if (permissions[2] == 'w') {
                    directoryTable[i].permissions[1] = '-';
                }
            }
            directoryTableChanged = 1;
            printf("Permissions changed: %s/%s to %s\n", parent, child, directoryTable[i].permissions);   
            return;
        }
    }
}

void addpwFile(char* parent, char* child, char* password, DirectoryEntry directoryTable[]) {
    int fileType = getType(parent, child, directoryTable);
    if (fileType == -1) {
        printf("NO SUCH FILE OR DIR \"%s/%s\"\n", parent, child);
        return;
    }

    for (int i = 0; i < MAXNUMBEROFFILE; i++) {
        if (strcmp(directoryTable[i].filename, child) == 0 && strcmp(directoryTable[i].parent, parent) == 0) {
            strncpy(directoryTable[i].password, password, sizeof(directoryTable[i].password) - 1);
            directoryTable[i].password[sizeof(directoryTable[i].password) - 1] = '\0'; // Ensure null-termination
            directoryTableChanged = 1;
            printf("Password added: %s/%s, '%s'\n", parent, child, directoryTable[i].password); // Debugging
            return;
        }
    }
}

char** arrangePath(char* path, int* number_of_elements) {
    char** arrangedPath = (char**) malloc(100 * sizeof(char*));
    arrangedPath[0] = strdup("BASE");    
    arrangedPath[1] = strdup("/");
    
    char* token = strtok(path, "/");    
    for (int i = 2; i < 100 && token != NULL; i++) {
        arrangedPath[i] = token;
        token = strtok(NULL, "/");
        (*number_of_elements)++;
    }
    return arrangedPath;
}

int getType(char* parent, char* child, DirectoryEntry directoryTable[]) {
    for (int i = 0; i < MAXNUMBEROFFILE; i++) {
        if (strcmp(directoryTable[i].filename, child) == 0 && strcmp(directoryTable[i].parent, parent) == 0) {
            return directoryTable[i].directory; // 1 for directory, 0 for file
        }
    }
    return -1;
}

int isParent(char* parent, char* child, DirectoryEntry directoryTable[]) {
    for (int i = 0; i < MAXNUMBEROFFILE; i++) {
        if (strcmp(directoryTable[i].filename, child) == 0) {
            if (strcmp(directoryTable[i].parent, parent) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

int isExist(char** arrangedPath, int until, DirectoryEntry directoryTable[]) {
    for (int i = 0; i < until; i++) {
        if (!isParent(arrangedPath[i], arrangedPath[i + 1], directoryTable)) {
            printf("NO SUCH FILE OR DIR!\n");
            return 0;
        }
    }
    return 1;
}
