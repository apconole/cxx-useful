#include "fd_buffer.hpp"

#include <iostream>

void check_outstream()
{
    cxx_utils::io::fd_buffer output(1, false);
    std::ostream out( &output );
    out << "31 hex: " << std::hex << 31 << std::endl;
}

void check_iostreams()
{
    cxx_utils::io::fd_buffer output(1, false);
    cxx_utils::io::fd_buffer input(0, false);

    std::ostream out( &output );
    std::istream in( &input );

    char c;
    int i;
    for(i=1; i<= 20; ++i)
    {
        in.get(c);
        out << c << std::flush;
        if( i && !(i % 8))
        {
            in.unget();
            in.unget();
        }
    }
    out << std::endl;
}

namespace cxx_utils_examples
{
    class RunTheIOStreamExamples
    {
    public:
        RunTheIOStreamExamples(){ check_outstream(); check_iostreams(); }
        ~RunTheIOStreamExamples(){}
    };
    
    static RunTheIOStreamExamples s_rtdle;
}
