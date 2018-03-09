// c code for /dev/mem read | write
// auther: yongqian  date: 2/10/2020

#include "devmem.h"

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

// devmem
devmem make_devman(unsigned int base_addr, unsigned int length, char* filename){
    int fd;
    unsigned int stop;

    const unsigned int PAGE_SIZE = 4096;
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

	if((fd = open( devmemd.fname, ( O_RDWR | O_SYNC))) == -1) {
		file_error( "ERROR: could not open \"/dev/mem\"...\n" );
	}

	//devmemd.virtual_base = mmap( NULL, 256, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, 0xff200000);
	devmemd.virtual_base = mmap( NULL, devmemd.length, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, devmemd.base_addr);
    if( devmemd.virtual_base == MAP_FAILED ) {
    	printf( "ERROR: mmap() failed...\n" );
    }
    return devmemd;
}

devmembuffer make_devmembuffer(unsigned long base_addr, list* data){
    devmembuffer devmembufferd;
    memset(&devmembufferd, 0, sizeof(devmembuffer));
    devmembufferd.base_addr = base_addr;
    devmembufferd.data = data;
    return devmembufferd;
}

// find length utility
unsigned int length(void* array){
    int length = sizeof array / sizeof *array;
}

devmembuffer devmemread(devmem* devmemd, unsigned long offset, unsigned int length){
    unsigned long abs_addr;
    list* data = make_list();

    if (offset < 0 || length < 0){
        error("offset or length cant be < 0");
    }

    unsigned long virtual_base_addr = devmemd->base_addr_offset & devmemd->mask;
    void* aligned_base = devmemd->virtual_base + virtual_base_addr + offset;

	for(char i = 0; i < length; i++){
        list_insert(data, (unsigned int*)(*(uint32_t *)aligned_base));
    }

    abs_addr = devmemd->base_addr + virtual_base_addr;
    devmembuffer devmembufferd = make_devmembuffer(abs_addr  + offset, data);
    return devmembufferd;
}


void devmemwrite(devmem* devmemd, unsigned long offset, unsigned int* data){
    if (offset < 0 || length(data) <= 0){
		error( "ERROR: offset or data length cannot smaller than 0");
    }

    // # Compensate for the base_address not being what the user requested
    offset += devmemd->base_addr_offset;

    // # Check that the operation is going write to an aligned location
    if (offset & ~devmemd->mask){
		error( "ERROR: mem location not aligned");
    }

    // # Seek to the aligned offset
    unsigned long virtual_base_addr = devmemd->base_addr_offset & devmemd->mask;
    printf("virtual_base_addr is %d\n", virtual_base_addr);
    void* aligned_base = devmemd->virtual_base + virtual_base_addr + offset;

    for(char i=0; i<length(data); i+=devmemd->word){
        *(uint32_t *)aligned_base = data[i];    // write 1 word at a time
    }
}

void devmemwritebit(devmem* devmemd, unsigned long offset, unsigned int bit, bool setbit){
    //if (offset < 0 || length(bit) <= 0){
    if (offset < 0) {
		error( "ERROR: offset cannot smaller than 0");
    }

    // # Compensate for the base_address not being what the user requested
    offset += devmemd->base_addr_offset;

    // # Check that the operation is going write to an aligned location
    if (offset & ~devmemd->mask){
		error( "ERROR: mem location not aligned");
    }

    // # Seek to the aligned offset
    unsigned long virtual_base_addr = devmemd->base_addr_offset & devmemd->mask;
    void* aligned_base = devmemd->virtual_base + virtual_base_addr + offset;

	if(setbit){
		*(uint32_t *)aligned_base |= (0x00000001 << bit);
	}
	else{
		*(uint32_t *)aligned_base &= ~(0x00000001 << bit);
	}
}