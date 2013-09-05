ARCH=x86_64-nacl

CC=${ARCH}-gcc
AR=${ARCH}-ar
CXX=${ARCH}-g++
RANLIB=${ARCH}-ranlib

CXXFLAGS=-Wall -Wno-long-long -msse4.1 -m64 -fno-exceptions

ZVM_PREFIX_ABSPATH = $(abspath $(ZVM_PREFIX))
ZEROVM=${ZVM_PREFIX_ABSPATH}/bin/zerovm

NAME=libjit

OBJECTS := jit.o

all: $(NAME).a

jit.o: jit.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

$(NAME).a: $(OBJECTS)
	@echo "Compiling $(NAME).a"
	$(AR) rcs $(NAME).a $(OBJECTS) 

clean:
	@rm *.o *.a 

install:
	install -D -m 0755 $(NAME).a $(ZVM_PREFIX_ABSPATH)/x86_64-nacl/lib
	

