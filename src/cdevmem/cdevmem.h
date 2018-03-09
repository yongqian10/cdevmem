#ifndef DEVMEM_H_
#define DEVMEM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

// the Lightweight HPS-to-FPGA Bus base address offset
#define HPS_LW_ADRS_OFFSET 0xFF200000

typedef struct Words {
    unsigned int size;
    int mask;
} word;

typedef struct DevMem {
    unsigned long base_addr;
    unsigned long base_addr_offset;
    unsigned int word;
    int mask;
    unsigned int length;
    char* fname;
    void* virtual_base;
    int fd;
} devmem;

// linked list
typedef struct node{
    void* val;
    struct node* next;
    struct node* prev;
} node;

typedef struct list{
    int size;
    node* front;
    node* back;
} list;

typedef struct DevMemBuffer {
    unsigned long base_addr;
    list* data;
} devmembuffer;

void error(const char *s);
void malloc_error();
void file_error(char *s);
unsigned int length(void* array);

list* make_list();
void* list_pop(list* l);
void list_insert(list* l, void* val);
void free_node(node* n);
void free_list(list* l);
void free_list_contents(list* l);
void **list_to_array(list* l);
devmem make_devman(unsigned int base_addr, unsigned int length, char* filename);
devmembuffer make_devmembuffer(unsigned long base_addr, list* data);
devmembuffer devmemread(devmem* devmemd, unsigned long offset, unsigned int length);
void devmemwrite(devmem* devmemd, unsigned long offset, unsigned int* data);
void devmemwritebit(devmem* devmemd, unsigned long offset, unsigned int bit, bool setbit);

#endif