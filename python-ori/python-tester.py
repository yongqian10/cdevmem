import mmap
import sys
import os

def mmap_io(base_addr, length=8, fname='/dev/mem'):
    f = os.open(fname, os.O_RDWR | os.O_SYNC)

    mem = mmap.mmap(f, length, mmap.MAP_SHARED,
            mmap.PROT_READ | mmap.PROT_WRITE,
            offset=base_addr)

    print(mem)


if __name__ == '__main__':
    mmap_io(0xFF200000, 8, '/dev/mem')
