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
#include <map>

#include "strings.hpp"

#ifndef __HTTP_MESSAGE__H__
#define __HTTP_MESSAGE__H__

namespace cxx_utils
{
    namespace net
    {
        namespace http
        {
            class cookie
            {
                std::string m_sCookieString;
            public:

                cookie(const std::string &cookieName,
                       const std::string &cookieVal, timeval *exp = 0,
                       const char *domain) :
                    m_sCookieString(cookieName + "=" + cookieVal)
                { }

                cookie(const std::string &cookieStr) :
                    m_sCookieString(cookieStr) {}

                bool expiration(timeval &expTime) const { return false; }
                bool domain(std::string &domain) const { return false; }
                bool name(std::string &name) const
                {
                    std::vector<std::string> nameval;
                    cxx_utils::string::utils::tokenize(nameval,
                                                       m_sCookieString, "= ");
                    if (nameval.empty() || nameval.size() < 2) {
                        return false;
                    }

                    name = nameval[0];
                    return true;
                }

                bool value(std::string &val) const
                {
                    size_t pos = m_sCookieString.find_first_of("=", 0);
                    if (pos == std::string::npos)
                        return false;
                    val = std::string(m_sCookieString.substr(pos+1));
                    return true;
                }

                bool operator==(const std::string &cookieStr) const
                {
                    std::string myname;
                    if (name(myname) && myname == cookieStr)
                        return true;

                    return m_sCookieString == cookieStr;
                }
            };

            class http_message
            {
                int32_t m_maj; int m_min;
            public:
                typedef std::map<std::string, std::string>  headers_set;
                http_message(uint32_t major = 1, uint32_t minor = 1) :
                    m_maj(major), m_min(minor) {}
                
            };
        }
    }
}
#endif
