import mmap
import sys
import os
import struct

def mmap_io(base_addr, slength=8, fname='/dev/mem'):

    word = 4
    mask = ~(word - 1) 
    print(f'mask is {mask}') #11111100
    print(f'pagesize is {mmap.PAGESIZE}') # 12 bit with msb 1

    print(f'base_addr before mask is {base_addr}')
    base_addr = base_addr & ~(mmap.PAGESIZE - 1)
    print(f'base_addr after mask is {base_addr}')
    base_addr_offset = 0

    stop = base_addr + slength * word
    print(f'stop is {stop}')

    if (stop % mask):
        stop = (stop + self.word) & ~(self.word - 1)
    print(f'stop is {stop}')

    length = stop - base_addr
    print(f'real length is {length}')

    f = os.open(fname, os.O_RDWR | os.O_SYNC)

    mem = mmap.mmap(f, slength, mmap.MAP_SHARED,
            mmap.PROT_READ | mmap.PROT_WRITE,
            offset=base_addr)

    mem.seek(0)
    print(mem.read(8))
    mem.write(struct.pack('I', 564))
    print(mem.read(4))


    #print(struct.pack('I', 240))
    #print(struct.unpack('I', b'\xff\x00\x00\x00'))

    #data = []
    #data.append(struct.unpack('I', mem.read(4))[0])
    #print(data)



if __name__ == '__main__':
    mmap_io(0x0, 0x10, './hello.txt')

