#include "saturation_iterator.hpp"

#include "print_vector.hpp"

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>

using namespace cxx_utils;

double PerformMultiplyInLinearDomain( const double &rDbl, const double &rDbl2 )
{
    double dElem1Lin = std::pow(double(10.0f), rDbl/10.0f);
    double dElem2Lin = std::pow(double(10.0f), rDbl2/10.0f);

    return double(10.0) * std::log10(dElem1Lin * dElem2Lin);
}

void saturation_main()
{
    // implement a besel function like tapped delay line

    // weights are -15, -30, -45, -99
    std::vector<double> vCoefficients;
    vCoefficients.push_back(-15.0);
    vCoefficients.push_back(-30.0);
    vCoefficients.push_back(-45.0);
    vCoefficients.push_back(-99.0);

    std::vector<double> vWeights;
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);
    vWeights.push_back(((double)rand() / (RAND_MAX))*-100);

    // 16 taps

    saturation_iterator<std::vector<double>::iterator> iTapCoefficients( vCoefficients.begin(), vCoefficients.end());

    double dTapLine = std::inner_product
        /*
        <
        std::vector<double>::iterator,
        saturation_iterator<std::vector<double>::iterator>, double,
        PerformMultiplyInLinearDomain, std::plus<double> >*/
        (vWeights.begin(), vWeights.end(), iTapCoefficients, 0.0,
         PerformMultiplyInLinearDomain, std::plus<double>());

    std::cout << "Taps : {" << vWeights << "} create result: " <<
        dTapLine << std::endl;
    
}

namespace cxx_utils_examples
{
    class RunTheTapDelayLineExample
    {
    public:
        RunTheTapDelayLineExample(){ saturation_main(); }
        ~RunTheTapDelayLineExample(){}
    };
    
    static RunTheTapDelayLineExample s_rtdle;
}
