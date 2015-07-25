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

#include <streambuf>
#include <vector>
#include <cstdio>
#include <cstring>

#ifdef WIN32
#include <fcntl.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#endif

#include <iostream>

#ifndef __FILE_DESCRIPTOR_BUFFER__H__
#define __FILE_DESCRIPTOR_BUFFER__H__

#ifdef WIN32
#define CXX_USEFUL_POPEN _popen
#define CXX_USEFUL_PCLOSE _pclose
#else
#define CXX_USEFUL_POPEN popen
#define CXX_USEFUL_PCLOSE pclose
#endif

namespace cxx_utils
{
    namespace io
    {
        /**
         * \brief An iostream style streambuf operating on stdio file descriptors
         * fdstreambuf takes a file descriptor, a buffer size, and a put-back
         * size.
         * The fdstreambuf only overloads the overflow and underflow methods to
         * provide a flush facility to C++ style streams.
         */
        class fd_buffer : public std::streambuf
        {
        public:
            /**
             * Construct a file-buffer from a file descriptor. It's best to use a
             * socket_buffer for socket descriptors; this keeps the distinction
             * strong.
             *
             * It's worth noting that this allows for the construction of process
             * streams from something like pipe()+popen() calls.
             */
            explicit fd_buffer(int nFileDes, std::size_t nBufSize = 1024,
                               std::size_t nPutBack = 8, bool bOwner = true)
                : m_cBuffer(std::max<std::size_t>(nBufSize, nPutBack) +
                            nPutBack),
                  m_nFileDes(nFileDes), m_nPutBack(nPutBack),
                  m_bOwner(bOwner)
            {
                char *end = BufferStart() + m_cBuffer.size();
                setg(end, end, end);
            }

            fd_buffer( int nFileDes, bool bOwner )
                : m_cBuffer(1032), m_nFileDes(nFileDes),
                  m_nPutBack(8), m_bOwner( bOwner )
            {
                char *end = BufferStart() + m_cBuffer.size();
                setg(end,end,end);
            }
            
            virtual ~fd_buffer()
            {
                if( m_nFileDes != -1 && m_bOwner ) fdclose(m_nFileDes);
            }

            std::streambuf::int_type underflow()
            {
                if( m_nFileDes == -1 )
                    return std::streambuf::traits_type::eof();
            
                if(gptr() < egptr())
                    return std::streambuf::traits_type::to_int_type(*gptr());

                char *base  = BufferStart();
                char *start = base;

                if (eback() == base) // true when this isn't the first fill
                {
                    // Make arrangements for putback characters
                    ::memmove(base, egptr() - m_nPutBack, m_nPutBack);
                    start += m_nPutBack;
                }

                ssize_t result =
                    internal_read(start, m_cBuffer.size()-(start-base));
                if( result <= 0 )
                {
                    return std::streambuf::traits_type::eof();
                }

                setg( base, start, start+result);
                return std::streambuf::traits_type::to_int_type(*gptr());
            }

            std::streambuf::int_type overflow(std::streambuf::int_type c)
            {
                if( m_nFileDes == -1 )
                    return std::streambuf::traits_type::eof();
                if( c != EOF)
                {
                    char z = c;
                    ssize_t result = internal_write((void *)(&z),
                                                    sizeof(char));
                    if( result != 1 )
                        return std::streambuf::traits_type::eof();
                }
                return std::streambuf::traits_type::to_int_type(c);
            }

            std::streamsize xsputn (const char* s, std::streamsize num)
            {
                if( m_nFileDes == -1 )
                    return std::streambuf::traits_type::eof();

                ssize_t result = internal_write((void *)s,num);
                if( result < 0 )
                    return std::streambuf::traits_type::eof();

                return std::streamsize(result);
            }

        protected:

            virtual std::streamsize showmanyc()
            {
                std::streamsize result = 0;
                // here we'll first do a basic fd check via select
                {
                    timeval tv;
                    tv.tv_sec = tv.tv_usec = 0;

                    fd_set fds;
                    FD_ZERO( &fds );
                    FD_SET(
#ifdef WIN32
                        (usigned int)
#endif
                        m_nFileDes, &fds
                           );
                    // note: select is part of POSIX, not C/C++; hopefully it
                    // exists on the target system.
                    if (0 < select(m_nFileDes + 1, &fds, NULL, NULL, &tv))
                    {
                        result = 1;
                    }
                }

                if( result <= 0 )
                {
                    result = internal_rd_ioctl();
                }

                return result;
            }

            virtual std::streamsize internal_rd_ioctl()
            {
                int numBytes = 0;
#ifdef FIONREAD
                if( 0 > ioctl(m_nFileDes, FIONREAD, (void *)&numBytes) )
                    return std::streamsize(-1);
#endif
                return numBytes;
            }
            
            virtual ssize_t internal_read(void *buf, size_t len)
            { return read(m_nFileDes, buf, len); }
            
            virtual ssize_t internal_write(void *buf, size_t len)
            { return write(m_nFileDes, buf, len); }

            virtual void fdclose(int fd)
            { close(fd); }
            
            char *BufferStart(){ return &m_cBuffer.front(); }
        
            std::vector<char> m_cBuffer;
            int               m_nFileDes;
            const std::size_t m_nPutBack;
            bool              m_bOwner;
        };

        /**
         * @brief A "Process-Input" stream, providing an easy way to read the
         * output of a platform external process.
         *
         * A pistream is a construct which allows calling an external
         * executable via a "popen" call, and provides an input stream to
         * access that process' stdout.
         */
        class pistream : public std::istream
        {
            FILE      *m_pFile;
            fd_buffer *m_pOpenedStream;
        public:
            pistream( const char *pBinary ) : std::istream(0),
                                              m_pFile(0), m_pOpenedStream(0)
            {
                if( pBinary )
                {
                    m_pFile = CXX_USEFUL_POPEN (pBinary, "r");
                    if( !m_pFile ) return;
                    m_pOpenedStream = new
                        cxx_utils::io::fd_buffer(fileno(m_pFile));
                    rdbuf(m_pOpenedStream);
                }
            }
            
            virtual ~pistream()
            {
                if( m_pFile ) CXX_USEFUL_PCLOSE( m_pFile );
                delete m_pOpenedStream;
            }
        };

        class postream : public std::ostream
        {
            FILE      *m_pFile;
            fd_buffer *m_pOpenedStream;
        public:
            postream( const char *pBinary ) : std::ostream(0),
                                              m_pFile(0), m_pOpenedStream(0)
            {
                if( pBinary )
                {
                    m_pFile = CXX_USEFUL_POPEN (pBinary, "w");
                    if( !m_pFile ) return;
                    m_pOpenedStream = new
                        cxx_utils::io::fd_buffer(fileno(m_pFile));
                    rdbuf(m_pOpenedStream);
                }
            }
            
            virtual ~postream()
            {
                if( m_pFile ) CXX_USEFUL_PCLOSE( m_pFile );
                delete m_pOpenedStream;
            }
        };
    }

    
}
#endif
