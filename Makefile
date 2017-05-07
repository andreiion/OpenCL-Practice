CC=g++

CFLAGS=-g -Wall -std=c++11 -D FEP_OPENCL
LIB=/usr/lib/x86_64-linux-gnu/libOpenCL.so.1
#BUILDDIR = ./build/
BUILDDIR = ./

all: $(BUILDDIR)/simple1

$(BUILDDIR)/simple1:
	$(CC) $(CFLAGS) $(INC) main.cpp -o $(BUILDDIR)/simple1 $(LIB)

clean:
	rm -f $(BUILDDIR)/simple1
