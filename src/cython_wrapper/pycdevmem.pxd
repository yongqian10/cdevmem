import numpy as np
cimport numpy as np

cdef extern from "cdevmem.h":

    cdef struct DevMem:
        unsigned int base_addr
        unsigned int base_addr_offset
        unsigned int offset
        unsigned int word
        int mask
        unsigned int length
        char* fname
        void* virtual_base
        int fd

    ctypedef DevMem devmem

    devmem make_devmem(unsigned int base_addr, unsigned int offset, unsigned int length, char* filename)