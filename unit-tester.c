#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define memfile = "/dev/mem";

int fd = open("memfile", (O_RDWR | O_SYNC));
void* virtual_base = mmap(NULL, 0x04000000, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0x00 );
