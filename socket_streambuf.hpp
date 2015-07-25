// "cyclic_iterator" -*- C++ -*-

// Copyright (C) 2014 Aaron Conole
//
// This file is governed by the 'use this freely' common-sense license
// This means the following:
// - Take this header
// - #include it in your project (commercial, or non)
// - ???
// - profit

/** @file fd_buffer.hpp
 * File-descriptor streambuf object
 */

#pragma once

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include "fd_buffer.hpp"

#ifndef __SOCKET_DESCRIPTOR_BUFFER__H__
#define __SOCKET_DESCRIPTOR_BUFFER__H__

namespace cxx_utils
{
    namespace io
    {
        /**
         * \brief An iostream style streambuf operating on socket file descriptors
         * sockstreambuf takes either a string-style host:port and constructs a socket, or can optionally take an fd/SOCKET object which is an already allocated socket.
         */
        class socket_streambuf : public fd_buffer
        {
        public:
            socket_streambuf(const std::string &rHost) : fd_buffer(GetSocketFromHost(rHost)), m_bStreamSock(GetSocketStreamFlag(rHost, m_nFileDes)), m_sHostUri(rHost)
            {
                
            }

        protected:
            
            
            virtual std::streamsize internal_rd_ioctl()
            {
                int numBytes = 0;
#ifdef FIONREAD
                if( 0 > ioctl(m_nFileDes, FIONREAD, (void *)&numBytes) )
                    return std::streamsize(-1);
#endif
                return numBytes;
            }

        private:
            bool m_bStreamSock;
            const std::string m_sHostUri;
        };
    }
}
#endif
