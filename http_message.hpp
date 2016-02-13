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
            };

            class http_message
            {
            public:
                typedef std::map<std::string, std::string>  headers;
                typedef std::vector<cookie> cookiejar;

            protected:
                std::int32_t     m_maj;
                int              m_min;
                headers          m_httphdrs;
                cookiejar        m_cookies;
                std::string      m_body;
                std::string      m_accumulator;
            public:
                http_message(std::uint32_t major = 1, std::uint32_t minor = 1):
                    m_maj(major), m_min(minor), m_httphdrs(), m_cookies(),
                    m_body(), m_accumulator(){}
                virtual ~http_message(){}

                virtual void updated() = 0;

                virtual std::string serialize() const = 0; 

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

            class http_request : public http_message
            {
                enum parsing_states {
                    parsing_method,
                    parsing_uri,
                    parsing_version,
                    parsing_headers,
                    parsing_body,
                    parsing_done,

                    parsing_err
                };

            public:
                enum methods {
                    get_method,
                    post_method,
                    put_method,
                    delete_method,
                    head_method,
                    options_method,
                    trace_method,

                    max_method
                };

            protected:

                methods        m_curmethod;
                parsing_states m_curstate;
                std::string    m_uri;
                char           m_nextbreaktok;

                void update_method()
                {
                    m_accumulator =
                        cxx_utils::string::utils::trim(m_accumulator);

                    if (m_accumulator.size() > 10) {
                        m_curmethod = max_method;
                    }

                    // XXX: use a std::map for this mapping
                    else if (m_accumulator == "GET") {
                        m_curmethod = get_method;
                    } else if (m_accumulator == "PUT") {
                        m_curmethod = put_method;
                    } else if (m_accumulator == "POST") {
                        m_curmethod = post_method;
                    } else if (m_accumulator == "DELETE") {
                        m_curmethod = delete_method;
                    } else if (m_accumulator == "HEAD") {
                        m_curmethod = head_method;
                    } else if (m_accumulator == "OPTIONS") {
                        m_curmethod = options_method;
                    } else if (m_accumulator == "TRACE") {
                        m_curmethod = trace_method;
                    } else {
                        m_curmethod = max_method;
                    }

                    if (m_curmethod != max_method) {
                        m_curstate = parsing_uri;
                    } else {
                        m_curstate = parsing_err;
                    }

                    m_accumulator.clear();
                }
                void update_uri()
                {
                    m_accumulator =
                        cxx_utils::string::utils::trim(m_accumulator);
                    if (m_accumulator.size() == 0)
                        return;

                    m_uri = utils::urldecode(m_accumulator);
                    m_curstate = parsing_version;
                    m_nextbreaktok = '\n';
                }
                void update_version()
                {
                    m_accumulator =
                        cxx_utils::string::utils::trim(m_accumulator, " \r\n\t");
                    if (m_accumulator.empty() ||
                        m_accumulator.substr(0, 5) != "HTTP/") {
                        m_curstate = parsing_err;
                        return;
                    }

                    
                }
                void update_headers()
                {
                }
                void update_body()
                {
                }
            public:
                http_request() : http_message(), m_curmethod(max_method),
                                 m_curstate(parsing_method), m_uri(),
                                 m_nextbreaktok(' ')
                {
                }
                http_request(methods method, std::string uri,
                             std::uint32_t maj = 1, std::uint32_t min = 0) :
                    http_message(maj, min), m_curmethod(method), 
                    m_curstate(parsing_done), m_uri(uri), m_nextbreaktok(' ')
                {
                }
                virtual ~http_request()
                {
                }
                virtual void updated()
                {
                    if( m_accumulator.size() == 0 ||
                        *(m_accumulator.end() - 1) != m_nextbreaktok) {
                        return;
                    }

                    switch(m_curstate){
                    case parsing_done:
                    case parsing_err:
                    default:
                        m_curstate = parsing_err;
                        break;
                    case parsing_method:
                        update_method();
                        break;
                    case parsing_uri:
                        update_uri();
                        break;
                    case parsing_version:
                        update_version();
                        break;
                    case parsing_headers:
                        update_headers();
                        break;
                    case parsing_body:
                        update_body();
                        break;
                    }
                }
                virtual std::string serialize() const
                {
                    std::string result = "";

                    switch(m_curmethod) {
                    case get_method:
                        result += "GET";
                        break;
                    case put_method:
                        result += "PUT";
                        break;
                    case post_method:
                        result += "POST";
                        break;
                    case delete_method:
                        result += "DELETE";
                        break;
                    case head_method:
                        result += "HEAD";
                        break;
                    case options_method:
                        result += "OPTIONS";
                        break;
                    case trace_method:
                        result += "TRACE";
                        break;
                    default:
                        return "<ERROR>";
                    }

                    result += ' ';
                    result += m_uri;
                    result += " HTTP/";
                    result += std::to_string(m_maj);
                    result += ".";
                    result += std::to_string(m_min);
                    result += "\r\n";

                    
                    result += "\r\n";
                    return result;
                }
            };
        }
    }
}
#endif
