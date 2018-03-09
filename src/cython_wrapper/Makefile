CC = gcc

LIB_DIR = ../../build/

default: pycdevmem

pycdevmem: setup.py pycdevmem.pyx ${LIB_DIR}/libcdevmem.a
	python3 setup.py build_ext --inplace && rm -f pycdevmem.c && rm -rf build

$(LIB_DIR)/libcdevmem.a:
	make

clean: 
	rm *.so