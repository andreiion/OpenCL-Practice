CC=g++

CFLAGS=-g -Wall -std=c++11 -D FEP_OPENCL
LIB=-L/usr/local/cuda-8.0/lib64/
#LIB=/usr/lib/x86_64-linux-gnu/libOpenCL.so.1
#BUILDDIR = ./build/
BUILDDIR = .

all: $(BUILDDIR)/simple1

$(BUILDDIR)/simple1: main.cpp helper.cpp
	$(CC) $(CFLAGS) $(INC) $(LIB) main.cpp helper.cpp -lOpenCL -o $(BUILDDIR)/simple1

clean:
	rm -f $(BUILDDIR)/simple1
