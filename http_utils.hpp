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
#include <cctype>

#ifndef __HTTP_MESSAGE__H__
#define __HTTP_MESSAGE__H__

namespace cxx_utils
{
    namespace net
    {
        namespace http
        {
        class utils
        {
        public:
            static std::string webdate( time_t now )
            {
                char buf[30];
                struct tm curTm;
#ifndef WIN32
                gmtime_r(&now, &curTm);
#else
                memset( &curTm, 0, sizeof(curTm) );
                _gmtime64_s(&gmt, &now);
#endif
                strftime( buf, sizeof(buf), "%A, %d-%b-%y %H:%M:%S", &curTm);
                return std::string(buf);
            }
            
            static std::string urldecode( const std::string &urltext )
            {
                std::string result;

                for(size_t i = 0; i < urltext.length(); ++i)
                {
                    char c = urltext[i];
                    if (urltext[i] == '%')
                    {
                        if((i+2 < urltext.length()) &&
                           isxdigit(urltext[i+1]) && isxdigit(urltext[i+2]))
                        {
                            char *end;
                            std::string hexstr = urltext.substr(i+1, 2);
                            c = strtol(hexstr.c_str(), &end, 16);
                            if( end == NULL || *end != '\0')
                                return result;
                            i+=2;
                            if (c == '+')
                                c = ' ';
                        }
                        else
                            return result;
                    }
                    else if (urltext[i] == '+')
                    {
                        c = ' ';
                    }
                    result += c;
                }
                return result;
            }

            static std::string urlencode(const std::string uri)
            {
                const std::string UNSAFECHARS = " /$&+,:;=?@'\"<>#%{}|\\^~[]`";
                const char LOOKUP[] = "0123456789ABCDEF";
                std::string result;

                for (size_t i = 0; i < uri.length(); ++i)
                {
                    char c = uri[i];
                    if (c == ' ')
                    {
                        c = '+';
                    }

                    if (c <= 31 || c >= 127 ||
                        UNSAFECHARS.find(c) != std::string::npos)
                    {
                        result += '%';
                        result += LOOKUP[c >> 4];
                        result += LOOKUP[c & 0xf];
                    }
                    else if (c == '+')
                    {
                        result += '%';
                        result += '2';
                        result += 'B';
                    }
                    else 
                    {
                        result += c;
                    }
                }
                return result;
            }
        };
        }
    }
}
#endif
