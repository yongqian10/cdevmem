import mmap
import sys
import os
import struct

def mmap_io(base_addr, length=8, fname='/dev/mem'):
    f = os.open(fname, os.O_RDWR | os.O_SYNC)

    mem = mmap.mmap(f, length, mmap.MAP_SHARED,
            mmap.PROT_READ | mmap.PROT_WRITE,
            offset=base_addr)

    print(mem.read(4))
    mem.seek(4)
    print(mem.read(4))
    print(mem.read(4))

    #print(struct.pack('I', 240))
    #print(struct.unpack('I', b'\xff\x00\x00\x00'))

    #data = []
    #data.append(struct.unpack('I', mem.read(4))[0])
    #print(data)



if __name__ == '__main__':
    mmap_io(0x00, 8, './hello.txt')
