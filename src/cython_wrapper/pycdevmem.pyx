# current design required new memory allocation each time server trigger the function
cdef devmem py_make_devmem(unsigned int base_addr, unsigned int offset, unsigned int length):
    cdef devmem devmemd = make_devmem(base_addr, offset, length, "/dev/mem")
    return devmemd 
