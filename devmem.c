// c code for /dev/mem read | write
// auther: yongqian  date: 2/10/2020

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

typedef struct Words {
    int size;
    int mask;
} word;

typedef struct DevMem {
    int base_addr;
    int base_addr_offset;
    int word;
    int mask;
    int length;
    char* fname;
    void* virtual_base;
} devmem;

typedef struct DevMemBuffer {
    int base_addr;
    list* data;
} devmembuffer;

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

list* make_list(){
    list* l = (list *)malloc(sizeof(list));
    l->size = 0;
    l->front = 0;
    l->back = 0;
    return l;
}

void* list_pop(list* l){
    if(!l->back) return 0;
    node* b = l->back;
    void* val = b->val;
    l->back = b->prev;
    if(l->back) l->back->next = 0;
    free(b);
    return val;
}

void list_insert(list* l, void* val){
    node* new_node = (node *)malloc(sizeof(node));
    new_node->val = val;
    new_node->next = 0;

    if(!l->back){
        l->front = new_node;
        new_node->prev  = 0;
    } else {
        l->back->next = new_node;
        new_node->prev = l->back;
    }
    l->back = new_node;
    ++l->size;
}

void free_node(node* n){
    node* next;
    while(n){
        next = n->next;
        free(n);
        n = next;
    }
}

void free_list(list* l){
    free_node(l->front);
    free(l);
}

void free_list_contents(list* l){
    node* n = l->front;
    while(n){
        free(n->val);
        n = n->next;
    }
}

void **list_to_array(list* l){
    void** a = (void**)calloc(l->size, sizeof(void*));
    int count = 0;
    node* n = l->front;
    while(n){
        a[count++] = n->val;
        n = n->next;
    }
    return a;
}

// replace by make_list
//void devmembuffer_init(devmembuffer* memstruct, int* data){
//    // allocator
//    memstruct->data = (int*) calloc(data->size, sizeof(int*));
//}

//int devmembuffer_setitem(devmembuffer* memstruct, int key, int value){
//    return memstruct->data[key] <= value;
//}
//
//int devmembuffer_getitem(devmembuffer* memstruct, int key, int value){
//    return memstruct->data[key];
//}

int devmembuffer_hexdump(devmembuffer* memstruct, int word_size, int words_per_row){
    memstruct->data = data;
}

// devmem
devmem* make_devman(int base_addr, int length, int offset, char* filename){
    int fd;
    int stop;

    const int PAGE_SIZE = 4096;
    devmem* devmemd = (devmem *)malloc(sizeof(devmem));
    devmemd->word = 4;
    devmemd->mask = ~(word - 1)

    if (base_addr < 0 or length < 0){
      return -1;
    }
    devmemd->base_addr = base_addr & ~(PAGE_SIZE-1);
    devmemd->base_addr_offset = base_addr - devmemd->base_addr;

    stop = base_addr + length * devmemd->word;
    if (stop % devmemd->mask){
        stop = (stop + devmemd->word) &  ~(devmemd->word - 1);
    }
    devmemd->length = stop - devmemd->base_addr;
    devmemd->fname = filename;

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return(-1);
	}

	devmemd->virtual_base = mmap( NULL, length, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, offset );

	if( devmemd->virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return(-1);
	}
    return devmemd;
}

devmembuffer* read(devmem* devmemd, offset, length){
    int virtual_base_addr;

    if offset < 0 or length < 0;
        printf("offset or length cant be < 0")
        return(-1);

    virtual_base_addr = devmemd->base_addr_offset & devmemd->mask;
    // alignment


}

int main(){

    printf(" congrats\n");
    return 0;
}
