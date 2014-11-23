CC=gcc
CXX=g++

CFLAGS=-I. -O3 -Wall -Werror
CXXFLAGS=-I. -O3 -Wall -Werror

CHK_SOURCES_C=$(filter %.c, $(CHK_SOURCES))
CHK_SOURCES_CXX=$(filter %.cpp %.cc %.cxx, $(CHK_SOURCES))

ifeq ($(CHK_SOURCES_C),)
CHECK_SYNTAXES+=
else
CHECK_SYNTAXES+=check-syntax-c
endif

ifeq ($(CHK_SOURCES_CXX),)
CHECK_SYNTAXES+=
else
CHECK_SYNTAXES+=check-syntax-cxx
endif

SAMPLE_PROGRAMS=cyclic_iterator_examples saturation_iterator_examples

CYCLIC_ITERATOR_EXAMPLE_OBJS=ring_buffer_ex.cpp moving_average.cpp 
SATURATION_ITERATOR_EXAMPLE_OBJS=saturation_test.cpp

.PHONY: all check-syntax check-syntax-c check-syntax-cxx

all: $(SAMPLE_PROGRAMS)
	echo "Done"

check-syntax-c:
	$(CC) $(CFLAGS) -fsyntax-only -Wno-variadic-macros -pedantic $(CHK_SOURCES_C)

check-syntax-cxx:
	$(CXX) $(CXXFLAGS) -fsyntax-only -Wno-variadic-macros -pedantic $(CHK_SOURCES_CXX)

check-syntax: $(CHECK_SYNTAXES)

cyclic_iterator_examples: cyclic_iterator_examples.cpp $(CYCLIC_ITERATOR_EXAMPLE_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $< $(CYCLIC_ITERATOR_EXAMPLE_OBJS)

saturation_iterator_examples: saturation_iterator_examples.cpp $(SATURATION_ITERATOR_EXAMPLE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(SATURATION_ITERATOR_EXAMPLE_OBJS)
