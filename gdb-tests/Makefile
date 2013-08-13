FLAG=d
NAME=gdb-tests

include ../Makefile.env

# switch realease /debug mode
# possible values are: r for release, d for debug


LDLIBS=

CXXFLAGS+=-fno-inline -O0
# add -g if debugging
ifeq ($(FLAG), d)
  CXXFLAGS+=-g
else
  CXXFLAGS+=-DNDEBUG
endif

#OBJECTS := $(patsubst %.cpp,%.o, $(wildcard *.cpp))
OBJECTS := main.o

$(NAME).nexe: $(OBJECTS)
	@echo "Compiling $(NAME).nexe"
	@$(CXX) -o $(NAME).nexe $(OBJECTS) $(CXXFLAGS) $(LDLIBS)