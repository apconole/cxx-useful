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
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>

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
                       const char *domain = "") :
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

                std::string value() const
                {
                    return m_sCookieString;
                }
            };

            class http_message
            {
            public:
                typedef std::map<std::string, std::string> headers;
                typedef std::vector<cookie>                cookiejar;

            protected:
                std::int32_t     m_maj;
                int              m_min;
                headers          m_httphdrs;
                cookiejar        m_cookies;
                std::string      m_body;
                std::string      m_accumulator;

                int parse_version()
                {
                    m_accumulator =
                        cxx_utils::string::utils::trim(m_accumulator,
                                                       " \r\n\t");
                    if (m_accumulator.empty() ||
                        m_accumulator.substr(0, 5) != "HTTP/") {
                        return -1;
                    }

                    std::string data=m_accumulator.substr(5,
                                                          std::string::npos);
                    m_accumulator.clear();
                    try {
                        m_maj = std::stoi(
                            data.substr(0, data.find_first_of(".")));
                        m_min = std::stoi(
                            data.substr(data.find_first_of(".")+1,
                                        std::string::npos));
                        return 0;
                    } catch (std::invalid_argument &e) {
                        return -1;
                    }
                }

            public:
                http_message(std::uint32_t major=1, std::uint32_t minor=1):
                    m_maj(major), m_min(minor), m_httphdrs(), m_cookies(),
                    m_body(), m_accumulator(){}
                virtual ~http_message(){}

                virtual void updated() = 0;

                virtual std::string serialize() const = 0; 

                const std::string &body() const { return m_body; }

                bool get_header(const std::string &hdr, std::string &val) const
                {
                    for( headers::const_iterator it = m_httphdrs.cbegin();
                         it != m_httphdrs.cend(); ++it ) {
                        if( cxx_utils::string::utils::istringcmp(it->first,
                                                                 hdr) ) {
                            val = it->second;
                            return true;
                        }
                    }
                    return false;
                }

                bool get_header(const char *hdr, std::string &val) const
                {
                    return get_header(std::string(hdr), val);
                }

                void set_header(const std::string &hdr, const std::string &val)
                {
                    m_httphdrs[hdr] = val;
                }

                void set_header(const char *hdr, const char *val)
                {
                    set_header(std::string(hdr), std::string(val));
                }

                virtual
                http_message &operator<<(const std::string &rhs)
                {
                    size_t endpos = rhs.size();
                    for(size_t pos = 0; pos < endpos; ++pos) {
                        m_accumulator += rhs[pos];
                        updated();
                    }
                    return *this;
                }

                friend
                std::ostream &operator<<(std::ostream &output,
                                         const http_message &rmsg)
                {
                    output << rmsg.serialize();
                    return output;
                }
            };
        }
    }
}

#include "http_response.hpp"
#include "http_request.hpp"

#endif
