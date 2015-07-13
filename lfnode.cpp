#include "lockfree-stack.hpp"

#include <iostream>
#include <thread>
#include <unistd.h>

using cxx_utils::concurrent::lfstack;

lfstack<int> stack;
std::atomic_int_fast64_t pop1;
std::atomic_int_fast64_t pop2;
std::atomic_int_fast64_t times;

void pusher(int n)
{
    for( unsigned int i = 0; i < 1000000; ++i )
        stack.push(n);
}

void popper()
{
    while( !stack.empty() )
    {
        try
        {
            int i = stack.pop();
            if ( i == 1 )
            {
                ++pop1;
            }
            else
                ++pop2;
            ++times;
        } catch (cxx_utils::concurrent::lfstack_pop_empty &e)
        {
            return;
        }
    }
}

int main()
{

    pop1  = 0;
    pop2  = 0;
    times = 0;
    
    std::thread t1(pusher, 1);
    std::thread t2(pusher, 2);

    t1.join();
    t2.join();

    //
    std::thread t3(popper);
    std::thread t4(popper);

    t3.join();
    t4.join();

    std::cout << "Got " << times << " pops 1(" << pop1 << "), 2(" << pop2 <<
        ")" << std::endl;

    return (times == 2000000 && (pop1 == pop2) == 1000000) ? 0 : 1;
}
