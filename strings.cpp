#include "strings.hpp"
#include <vector>
#include <iostream>
#include <list>
int main(int argc, const char *argv[])
{
    std::cout << "Testing tokenization" << std::endl;

    std::string sInputRain = "The rain in spain falls mainly on the plain!";
    std::string sInputLazy = "The  Quick\nBrown\tfox   jumped\r\nover the lazy dog";

    std::vector<std::string> resultVec;
    std::list<std::string> resultLst;
    
    cxx_utils::string::utils::tokenize(resultVec,sInputRain, " ");
    
    cxx_utils::string::utils::tokenize(resultLst,sInputLazy);

    if(resultVec.size() != resultLst.size())
        std::cout << "Must have been an error tokenizing ("
                  << resultVec.size() << ":" << resultLst.size() <<  ")";

    return 0;
}
