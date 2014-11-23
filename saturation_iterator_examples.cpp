#include <iostream>

namespace cxx_utils_examples
{
    void RunExamples()
    {
        std::cout << "After examples have run." << std::endl;
    }
}

int main()
{
    std::cout << "Saturation Iterator Examples." << std::endl;
    cxx_utils_examples::RunExamples();
    return 0;
}
