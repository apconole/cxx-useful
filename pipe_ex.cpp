#include "fd_buffer.hpp"


void test_IpConfigOutput()
{
#ifndef WIN32
    cxx_utils::io::pistream ipconfig("/sbin/ifconfig");
#else
    cxx_utils::io::pistream ipconfig("ipconfig");
#endif
    std::string line;
    while(std::getline(ipconfig,line))
    {
        std::cout << "> " << line << std::endl;
    }

}

namespace cxx_utils_examples
{
    class RunThePipeStreamExamples
    {
    public:
        RunThePipeStreamExamples(){ test_IpConfigOutput(); }
        ~RunThePipeStreamExamples(){}
    };
    
    static RunThePipeStreamExamples s_rtdle;
}
