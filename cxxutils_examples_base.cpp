#include <iostream>

#ifndef EXAMPLES_STRING
#define EXAMPLES_STRING "???"
#endif

namespace cxx_utils_examples
{
    void RunExamples()
    {
        std::cout << "After examples have run." << std::endl;
    }
}

int main()
{
    std::cout << EXAMPLES_STRING << " Examples." << std::endl;
    cxx_utils_examples::RunExamples();
    return 0;
}
