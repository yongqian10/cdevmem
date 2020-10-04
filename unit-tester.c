#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

unsigned char *read_file(char *filename)
{
    FILE *fp = fopen(filename, "rb");
    size_t size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *text = (unsigned char *)calloc(size+1, sizeof(unsigned char));
    fread(text, 1, size, fp);
    fclose(fp);
    return text;
}

int main(){
int fd = open("/dev/mem", (O_RDWR | O_SYNC));
void* virtual_base = mmap(NULL, 0x04, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0x00 );
//// aligned base based on offset
void* aligned_base = (void*)((char*)virtual_base + 10);
printf("%p\n", virtual_base);
printf("%p\n", aligned_base);
//
//
//// small test snippet
//void* a = (void*)0x10;
//void* intptr =  (void*)a;
//void* inptrsum =  intptr + 0x10;
//printf("%d", &inptrsum);

//    u_int16_t a = 0x1011;
//    printf("%u\n", a);
//
//    u_int16_t b = (a >> 12u);
//    printf("%x\n", b);

    // causing core dump error
    //char* filename = "/dev/mem";

    //unsigned char* a = read_file(filename);
    //printf("%s\n", a);

}
