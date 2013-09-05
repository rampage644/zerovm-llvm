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

all: tests

jit.o: jit.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

$(NAME).a: $(OBJECTS)
	@echo "Compiling $(NAME).a"
	$(AR) rcs $(NAME).a $(OBJECTS) 

tests.o: tests.cpp
	$(CXX) -c -o tests.o tests.cpp $(CXXFLAGS)	-I.

tests: tests.o $(NAME).a
	$(CXX) -o tests.nexe tests.o -Wl,--whole-archive -lzdl -Wl,--no-whole-archive -lgtest -lbfd -liberty -L.
	@python run.py
	@zerovm -PQst tests.manifest || true
	@cat stdout.tests

debug: tests
	@zgdb tests.nexe

clean:
	@rm *.o *.nexe *.a *.conf *.log std* *.manifest

install:
	install -D -m 0755 $(NAME).a $(ZVM_PREFIX_ABSPATH)/x86_64-nacl/lib
	

