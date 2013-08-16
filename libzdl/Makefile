ARCH=x86_64-nacl

CC=${ARCH}-gcc
AR=${ARCH}-ar
CXX=${ARCH}-g++
RANLIB=${ARCH}-ranlib

CXXFLAGS=-Wall -Wno-long-long -msse4.1 -m64

ZVM_PREFIX_ABSPATH = $(abspath $(ZVM_PREFIX))
ZEROVM=${ZVM_PREFIX_ABSPATH}/bin/zerovm

NAME=libzdl

OBJECTS := zdl.o dummy_symbols_libc.o

all: tests

zdl.o: zdl.c
	$(CC) -c -o $@ zdl.c

dummy_symbols_libc.o: dummy_symbols_libc.c
	$(CC) -c -o $@ dummy_symbols_libc.c	-O0

tests.o: tests.cpp
	$(CXX) -c -o tests.o tests.cpp $(CXXFLAGS)	-I.

$(NAME).a: $(OBJECTS)
	@echo "Compiling $(NAME).a"
	$(AR) rcs $(NAME).a $(OBJECTS) 

tests: tests.o $(NAME).a
	$(CXX) -o tests.nexe tests.o -lzdl -lgtest -lbfd -liberty -L. 
	@python run.py
	@zerovm -PQst tests.manifest || true
	@cat stdout.tests

debug: tests
	@zgdb tests.nexe

clean:
	@rm *.o *.nexe *.a *.conf *.log std* *.manifest

install:
	install -D -m 0755 $(NAME).a $(ZVM_PREFIX_ABSPATH)/x86_64-nacl/lib
	

