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

// the Lightweight HPS-to-FPGA Bus base address offset
#define HPS_LW_ADRS_OFFSET 4280287232

// LTC2308 Address offset
#define ADC_ADDRES_OFFSET 56

// Register set of the LTC230
#define ADC_CMD_REG_OFFSET 0
#define ADC_DATA_REG_OFFSET 4

// FIFO Convention Data Size for average calculation
#define FIFO_SIZE 2 // MAX=1024

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
    unsigned long base_addr;
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
    unsigned int test = (stop % devmemd.mask);
    if (stop % devmemd.mask){
        stop = (stop + devmemd.word) &  ~(devmemd.word - 1);
    }
    devmemd.length = stop - devmemd.base_addr;
    devmemd.fname = filename;

	if( ( fd = open( devmemd.fname, ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		file_error( "ERROR: could not open \"/dev/mem\"...\n" );
	}

	//devmemd.virtual_base = mmap( NULL, 256, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, 0xff200000);
	devmemd.virtual_base = mmap( NULL, devmemd.length, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, devmemd.base_addr);
    if( devmemd.virtual_base == MAP_FAILED ) {
    	printf( "ERROR: mmap() failed...\n" );
    }

    return devmemd;
}

devmembuffer make_devmembuffer(unsigned int base_addr, unsigned int* data){
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

unsigned int hex2dec(char hexVal[]){
    int len = strlen(hexVal); 
      
    // Initializing base value to 1, i.e 16^0 
    int base = 1; 
      
    int dec_val = 0; 
      
    // Extracting characters as digits from last character 
    for (int i=len-1; i>=0; i--) 
    {    
        // if character lies in '0'-'9', converting  
        // it to integral 0-9 by subtracting 48 from 
        // ASCII value. 
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
                  
            // incrementing base by power 
            base = base * 16; 
        } 
  
        // if character lies in 'A'-'F' , converting  
        // it to integral 10 - 15 by subtracting 55  
        // from ASCII value 
        else if (hexVal[i]>='A' && hexVal[i]<='F') 
        { 
            dec_val += (hexVal[i] - 55)*base; 
          
            // incrementing base by power 
            base = base*16; 
        } 
    } 
    return dec_val; 
} 

// find length utility
unsigned int length(void* array){
    int length = sizeof array / sizeof *array;
}


devmembuffer devmemread(devmem* devmemd, unsigned int offset, unsigned int length){
    unsigned int data[length];
    unsigned int virtual_base_addr;
    unsigned int abs_addr;

    if (offset < 0 || length < 0){
        error("offset or length cant be < 0");
    }

    virtual_base_addr = devmemd->base_addr_offset & devmemd->mask;
    // aligned base based on offset
    void* aligned_base = (void*)((unsigned int*)devmemd->virtual_base + virtual_base_addr + 1);
    //printf("%p\n", aligned_base);

    // # Read length words of size self.word and return it
    // data = []
    // for i in range(length):
    //     data.append(struct.unpack('I', mem.read(self.word))[0])

    // abs_addr = self.base_addr + virt_base_addr
    // return DevMemBuffer(abs_addr + offset, data)

	for (char i = 0; i < length; i++) {
            data[i] = *(unsigned int*)(aligned_base);
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
    // # Read until the end of our aligned address
    // for i in range(0, len(din), self.word):
    //     self.debug('writing at position = {0}: 0x{1:x}'.
    //                 format(self.mem.tell(), din[i]))
    //     # Write one word at a time
    //     mem.write(struct.pack('I', din[i]))
    for(char i=0; i<length(data); i+=devmemd->word){
        *(uint32_t *)aligned_base = data[i];    // write 1 word at a time
    }
}


int main(){
    unsigned int ch = 0;
    unsigned int ch_sw;
    unsigned int data;
    unsigned int count;

    printf(" congrats\n");
    // try cdevmem
    //de = devmem.DevMem(HPS_LW_ADRS_OFFSET, ADC_ADDRES_OFFSET+0x8, "/dev/mem")
    // Set meassure number for ADC convert
    devmem devmemd = make_devman(HPS_LW_ADRS_OFFSET, ADC_ADDRES_OFFSET+8, "/dev/mem");
    //de.write(ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET,[FIFO_SIZE])

    int fifo_size = FIFO_SIZE;  
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET, &fifo_size);

    // # Enable the convention with the selected Channel
    // de.write(ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, [(ch <<1) | 0x00])
    ch_sw = ch << 1 | 0x00;
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET, &ch_sw);

    // de.write(ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, [(ch <<1) | 0x01])
    ch_sw = ch << 1 | 0x01;
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET, &ch_sw);

    // de.write(ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, [(ch <<1) | 0x00])
    ch_sw = ch << 1 | 0x00;
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET, &ch_sw);

    // timeout = 300 #ms
    // # Wait until convention is done or a timeout occurred
    // while (not(timeout == 0)):
    while (devmemread(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET,1).data[0] & (1<<0)){
        count += 1;
        printf("its counting ... %d\n", count);
        continue;
    };
    //         break

    //     timeout = timeout -1
    //     time.sleep(.001) # delay 1ms

    // # calculate the average of the FIFO
    unsigned int rawValue = 0;
    for(int i; i<FIFO_SIZE; i++){
        rawValue = rawValue+ (devmemread(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET,1)).data[0];
    }
    // value = rawValue / FIFO_SIZE

    // # Convert ADC Value to Volage
    // volage = round(value/1000,2)
    // # print the Value
    // print(str(volage))

    return 0;
}
