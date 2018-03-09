// c code for /dev/mem read | write
// auther: yongqian  date: 2/10/2020

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

void error(const char *s)
{
    perror(s);
    exit(-1);
}

void malloc_error()
{
    fprintf(stderr, "Malloc error\n");
    exit(-1);
}

void file_error(char *s)
{
    fprintf(stderr, "Couldn't open file: %s\n", s);
    exit(0);
}

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

typedef struct DevMemBuffer {
    int base_addr;
    unsigned int* data;
} devmembuffer;

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

//int devmembuffer_hexdump(devmembuffer* memstruct, int word_size, int words_per_row){
//    memstruct->data = data;
//}

// devmem
devmem make_devman(int base_addr, int length, char* filename){
    int fd;
    int stop;

    const int PAGE_SIZE = 4096;
    //devmem* devmemd = (devmem *)malloc(sizeof(devmem));
    devmem devmemd;
    memset(&devmemd, 0, sizeof(devmem));
    devmemd.word = 4;
    devmemd.mask = ~(devmemd.word - 1);

    if (base_addr < 0 || length < 0){
        error("offset or length cant be < 0");
    }
    devmemd.base_addr = base_addr & ~(PAGE_SIZE-1);
    devmemd.base_addr_offset = base_addr - devmemd.base_addr;

    stop = base_addr + length * devmemd.word;
    if (stop % devmemd.mask){
        stop = (stop + devmemd.word) &  ~(devmemd.word - 1);
    }
    devmemd.length = stop - devmemd.base_addr;
    devmemd.fname = filename;

	if( ( fd = open( devmemd.fname, ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		file_error( "ERROR: could not open \"/dev/mem\"...\n" );
	}

	devmemd.virtual_base = mmap( NULL, devmemd.length, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, devmemd.base_addr );

	//if( devmemd.virtual_base == MAP_FAILED ) {
	//	perror("Virtual_addr_in mappong for absolute memory access failed!\n");
	//	close( fd );
	//	return(-1);
	//}
    return devmemd;
}

devmembuffer make_devmembuffer(int base_addr, unsigned int* data){
    devmembuffer devmembufferd;
    memset(&devmembufferd, 0, sizeof(devmembuffer));
    devmembufferd.base_addr = base_addr;
    devmembufferd.data = data;
    return devmembufferd;
}

// concat utility
unsigned int concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;
}

// dec to hex array
char* dec2hex_arr(int dec){
    int quotient, rem;
    int i, j = 0;
    static char hexcode[100];

    quotient = dec;

    while(quotient != 0){
        rem = quotient % 16;
        if (rem < 10)
            hexcode[j++] = 48 + rem;
        else 
            hexcode[j++] = 55 + rem;
        
        quotient = quotient / 16;
    }
    return hexcode;
}

// find length utility
unsigned int length(void* array){
    int length = sizeof array / sizeof *array;
}


devmembuffer read(devmem* devmemd, int offset, int length){
    unsigned int data[length];
    unsigned int virtual_base_addr;
    unsigned int abs_addr;

    if (offset < 0 || length < 0){
        error("offset or length cant be < 0");
    }

    virtual_base_addr = devmemd->base_addr_offset & devmemd->mask;
    // aligned base based on offset
    void* aligned_base = (void*)((unsigned int*)devmemd->virtual_base + virtual_base + 1);

    // # Read length words of size self.word and return it
    // data = []
    // for i in range(length):
    //     data.append(struct.unpack('I', mem.read(self.word))[0])

    // abs_addr = self.base_addr + virt_base_addr
    // return DevMemBuffer(abs_addr + offset, data)

	for (char i = 0; i < length; i++) {
            unsigned int data[i] = *(unsigned int*)(aligned_base);
        }
    }

    abs_addr = devmemd->base_addr + virtual_base_addr;
    devmembuffer devmembufferd = make_devmembuffer(abs_addr  + offset, data);
    return devmembufferd;
}


void write(devmem* devmemd, unsigned int offset, unsigned int* data){
    unsigned int* sdata;

    if (offset < 0 or length(data) <= 0){
		error( "ERROR: offset or data length cannot smaller than 0");
    }

    // # Compensate for the base_address not being what the user requested
    unsigned int offset += devmemd->base_addr_offset;

    // # Check that the operation is going write to an aligned location
    if (offset & ~devmemd->mask){
		error( "ERROR: mem location not aligned");
    }

    // # Seek to the aligned offset
    unsigned int virtual_base_addr = devmemd->base_addr_offset & devmemd->mask;
    void* aligned_base = (void*)((unsigned int*)devmemd->virtual_base + virtual_base + 1);

    // # Read until the end of our aligned address
    // for i in range(0, len(din), self.word):
    //     self.debug('writing at position = {0}: 0x{1:x}'.
    //                 format(self.mem.tell(), din[i]))
    //     # Write one word at a time
    //     mem.write(struct.pack('I', din[i]))
    for(i=0; i<length(data), i+=devmemd->word){
        aligned_base = &data[i];    // write 1 word at a time
    }
}


int main(){

    printf(" congrats\n");
    return 0;
}
