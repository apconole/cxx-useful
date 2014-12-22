#include "cyclic_iterator.hpp"

#include "print_vector.hpp"

#include <string>
#include <vector>
#include <iostream>

using namespace cxx_utils::iterator;
using namespace cxx_utils::io;

void ring_buffer_main_ex()
{
    // declare a vector
    std::vector<std::string> cLotsOfStrings(3, std::string(""));

    // time to get a cyclic iterator
    std::cout << "Number of entries: " << cLotsOfStrings.size() << std::endl;

    if( cLotsOfStrings.begin() == cLotsOfStrings.end() )
    {
        std::cout << "This will result in an error - bailing" << std::endl;
        return;
    }
    
    cyclic_iterator<std::vector<std::string>::iterator> iCycles(cLotsOfStrings.begin(), cLotsOfStrings.end());
    
    std::string sInput;
    std::cout << "Vector ring buffer example" << std::endl;
    std::cout << "Enter text, a line at a time; when done enter 'q'"
              << std::endl;
    std::cout << "Values will be stored in a ring-buffer of size: "
              << cLotsOfStrings.size() << std::endl;
    do
    {
        std::cout << ">>> ";
        std::cout.flush();

        std::getline(std::cin, sInput);
        if( sInput != "q" )
            *iCycles++ = sInput;
    }while(!sInput.empty() && bool(std::cin) && sInput != "q");
    std::cout << "Current contents of buffer {" << cLotsOfStrings
              << "}" << std::endl;
    
}

namespace cxx_utils_examples
{
    class RunTheRingBufferExample
    {
    public:
        RunTheRingBufferExample(){ ring_buffer_main_ex(); }
        ~RunTheRingBufferExample(){}
    };

    static RunTheRingBufferExample s_rtrbe;
}
