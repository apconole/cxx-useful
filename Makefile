CC=gcc
CXX=g++

RM=rm

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

SAMPLE_PROGRAMS=cyclic_iterator_examples saturation_iterator_examples file_descriptor_examples

CYCLIC_ITERATOR_EXAMPLE_OBJS=ring_buffer_ex.cpp moving_average.cpp 
SATURATION_ITERATOR_EXAMPLE_OBJS=saturation_test.cpp
FILE_DESCRIPTOR_EXAMPLE_OBJS=pipe_ex.cpp simple_fdstream_ex.cpp

.PHONY: all check-syntax check-syntax-c check-syntax-cxx clean

all: $(SAMPLE_PROGRAMS)
	echo "Done"

check-syntax-c:
	-$(CC) $(CFLAGS) -fsyntax-only -Wno-variadic-macros -pedantic $(CHK_SOURCES_C)

check-syntax-cxx:
	-$(CXX) $(CXXFLAGS) -fsyntax-only -Wno-variadic-macros -pedantic $(CHK_SOURCES_CXX)

check-syntax: $(CHECK_SYNTAXES)

clean:
	$(RM) -rf $(SAMPLE_PROGRAMS) $(CYCLIC_ITERATOR_EXAMPLE_OBJS) $(SATURATION_ITERATOR_EXAMPLE_OBJS) $(FILE_DESCRIPTOR_EXAMPLE_OBJS)

cyclic_iterator_examples: cxxutils_examples_base.cpp $(CYCLIC_ITERATOR_EXAMPLE_OBJS) 
	$(CXX) $(CXXFLAGS) -DEXAMPLES_STRING="\"Cyclic Iterator\"" -o $@ $< $(CYCLIC_ITERATOR_EXAMPLE_OBJS)

saturation_iterator_examples: cxxutils_examples_base.cpp $(SATURATION_ITERATOR_EXAMPLE_OBJS)
	$(CXX) $(CXXFLAGS) -DEXAMPLES_STRING="\"Saturation Iterator\"" -o $@ $< $(SATURATION_ITERATOR_EXAMPLE_OBJS)

file_descriptor_examples: cxxutils_examples_base.cpp $(FILE_DESCRIPTOR_EXAMPLE_OBJS)
	$(CXX) $(CXXFLAGS) -DEXAMPLES_STRING="\"File Descriptor Buffer\"" -o $@ $< $(FILE_DESCRIPTOR_EXAMPLE_OBJS)
