// "cyclic_iterator" -*- C++ -*-

// Copyright (C) 2014 Aaron Conole
//
// This file is governed by the 'use this freely' common-sense license
// This means the following:
// - Take this header
// - #include it in your project (commercial, or non)
// - ???
// - profit

///
/// cyclic_iterator
/// Implementation of a cyclical iterator for continuous (modulo) traversal of an iterable
/// container / type.
/// This has proven useful for computing dot-products and running FIR filters using the
/// C++ std::inner_product() function.
///

/** @file cyclic_iterator
 * This is a cxx-useful header
 */

#pragma once

#ifndef __PRINT_VECTOR__H__
#define __PRINT_VECTOR__H__

#include <iostream>
#include <vector>

namespace cxx_utils
{
    namespace io
    {
        /**
         * @brief Returns the std::ios_base allocated number for the stream delimiter used by the vector output.
         */
        inline int delimid( )
        {
            static int iDelimIdx = std::ios_base::xalloc();
            return iDelimIdx;
        }

        /**
         * \brief Sets the stream delimiter used by the vector output.
         */
        inline std::ios_base &
        setdelim(std::ios_base &iosb, char c)
        {
            iosb.iword( delimid() ) = c;
            return iosb;
        }

        /**
         * \brief Gets the stream delimiter used by the vector output.
         */
        inline char getdelim( std::ios_base &iosb )
        {
            if( iosb.iword(delimid()) )
                return iosb.iword(delimid());
            return ',';
        }

    
        /**
         * @brief Writes a 'simple' vector to a stream using an ostream inserter.
         *
         * 'simple' implies a vector of non-template type. IE: a std::vector of some class is fine.
         * However, a std::vector of std::vector will not work.
         */
        template <typename T>
        std::ostream &operator<<(std::ostream &os, const std::vector<T> &outvec)
        {
            //simplistic first cut...

            char cDelim[] = { getdelim(os), 0 };

            std::string sString( cDelim );

            if( !outvec.empty() )
            {
                std::copy( outvec.begin(), outvec.end() - 1, std::ostream_iterator<T>(os, sString.c_str()));
                os << *(outvec.end() - 1);
            }
            else
            {
                os << "";
            }

            return os;
        }

    }
}

#endif
