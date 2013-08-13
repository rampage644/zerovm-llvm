ARCH=x86_64-nacl

CC=${ARCH}-gcc
AR=${ARCH}-ar
CXX=${ARCH}-g++
RANLIB=${ARCH}-ranlib

CXXFLAGS=-Wall -Wno-long-long -msse4.1 -m64

ZVM_PREFIX_ABSPATH = $(abspath $(ZVM_PREFIX))
ZEROVM=${ZVM_PREFIX_ABSPATH}/bin/zerovm

NAME=libzdl

OBJECTS := zdl.o

all: tests

zdl.o: zdl.c
	@$(CXX) -c -o $@ zdl.c $(CXXFLAGS)

tests.o: tests.cpp
	@$(CXX) -c -o tests.o tests.cpp $(CXXFLAGS)	-I.

$(NAME).a: $(OBJECTS)
	@echo "Compiling $(NAME).a"
	@$(AR) rcs $(NAME).a $(OBJECTS) 

tests: tests.o $(NAME).a
	@$(CXX) -o tests.nexe tests.o -lzdl -lgtest -L.
	@python run.py
	@zerovm -PQst tests.manifest || true
	@cat stdout.tests

debug: tests
	@zgdb tests.nexe
	

