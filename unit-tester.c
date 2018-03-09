#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(){
int fd = open("/dev/mem", (O_RDWR | O_SYNC));
void* virtual_base = mmap(NULL, 0x04000000, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0x00 );
// aligned base based on offset
void* aligned_base = virtual_base + 0x21000;


// small test snippet
void* a = (void*)0x10;
void* intptr =  (void*)a;
void* inptrsum =  intptr + 0x10;
printf("%d", &inptrsum);
}