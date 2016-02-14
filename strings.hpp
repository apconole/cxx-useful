// "http_message" -*- C++ -*-

// Copyright (C) 2016 Aaron Conole
//
// This file is governed by the 'use this freely' common-sense license
// This means the following:
// - Take this header
// - #include it in your project (commercial, or non)
// - ???
// - profit

///
///

#pragma once

#include <string>

#ifndef __STRING_UTILS__H__
#define __STRING_UTILS__H__

namespace cxx_utils
{
    namespace string
    {
        class utils
        {
        public:
            template<typename T>
            static void tokenize(T &output,
                                 const std::string &input,
                                 const std::string &delims = std::string(" \r\n\t"))
            {
                size_t position = 0;
                size_t adv_position = 0;

                output.clear();

                position = input.find_first_of(delims, position);
                while( position != std::string::npos )
                {
                    std::string token =
                        input.substr(adv_position, position - adv_position);
                    if(token != "")
                        output.push_back(token);
                    position++;
                    adv_position = position;
                    position = input.find_first_of(delims, position);
                }

                if( adv_position < input.length() )
                {
                    output.push_back( input.substr ( adv_position ) );
                }

            }

            static
            std::string trim(const std::string &trimstring,
                             const std::string &spaces = " \t")
            {
                const size_t startpos = trimstring.find_first_not_of(spaces);
                if( startpos == std::string::npos )
                    return "";

                const size_t endpos = trimstring.find_last_not_of(spaces);
                const size_t total = endpos - startpos + 1;

                return trimstring.substr(startpos, total);
            }

            static inline bool istringcmp( const std::string& str1,
                                           const std::string& str2,
                                           bool substringmatches=false)
            {
                std::string str1Cpy(str1);
                std::string str2Cpy(str2);
                std::transform(str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(),
                               ::tolower );
                std::transform(str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(),
                               ::tolower );
                if( substringmatches )
                {
                    if( str1Cpy.length() < str2Cpy.length() )
                        return str1Cpy == str2Cpy.substr(0, str1Cpy.length());
                    else
                        return str2Cpy == str1Cpy.substr(0, str2Cpy.length());
                }
                return ( str1Cpy == str2Cpy );
            }

        };
    }
}

#endif
