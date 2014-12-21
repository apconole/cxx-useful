#include "fd_buffer.hpp"

#include <iostream>

#ifdef WIN32
#define POPEN _popen
#else
#define POPEN popen
#endif

class pistream : public std::istream
{
    FILE *m_pFile;
    cxx_utils::io::fd_buffer *m_pOpenedStream;
public:
    pistream( const char *pBinary ) : std::istream(0),
                                      m_pFile(0), m_pOpenedStream(0)
    {
        if( pBinary )
        {
            m_pFile = POPEN (pBinary, "r");
            if( !m_pFile ) return;
            m_pOpenedStream = new cxx_utils::io::fd_buffer(fileno(m_pFile));
            rdbuf(m_pOpenedStream);
        }
    }

    virtual ~pistream()
    {
        delete m_pOpenedStream;
    }
};

void test_IpConfigOutput()
{
#ifndef WIN32
    pistream ipconfig("/sbin/ifconfig");
#else
    pistream ipconfig("ipconfig");
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
