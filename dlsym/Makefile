FLAG=d
NAME=dlsym

include ../Makefile.env

# switch realease /debug mode
# possible values are: r for release, d for debug


LDLIBS=-lbfd -liberty

CXXFLAGS+=-fno-rtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
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
	$(CXX) -o $(NAME).nexe $(OBJECTS) $(CXXFLAGS) $(LDLIBS)

	

