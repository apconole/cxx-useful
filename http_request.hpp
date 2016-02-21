#ifndef __HTTP_MESSAGE__H__
#error Cannot include this file directly
#endif

#ifndef __HTTP_REQUEST_MESSAGE_H__
#define __HTTP_REQUEST_MESSAGE_H__
namespace cxx_utils
{
    namespace net
    {
        namespace http
        {
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
                http_response::codes m_suggestedcode;
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
                        m_suggestedcode = http_response::not_implemented;
                    }

                    m_accumulator.clear();
                }
                void update_uri()
                {
                    m_accumulator =
                        cxx_utils::string::utils::trim(m_accumulator);
                    if (m_accumulator.size() == 0)
                        return;

                    m_uri = (m_accumulator);
                    m_accumulator.clear();
                    m_curstate = parsing_version;
                    m_nextbreaktok = '\n';
                }
                void update_version()
                {
                    if ( parse_version() < 0 ) {
                        m_suggestedcode =
                            http_response::http_version_not_supported;
                        m_curstate = parsing_err;
                    } else
                        m_curstate = parsing_headers;
                }
                void update_headers()
                {
                    if( m_accumulator == "\r\n" || m_accumulator == "\n" ){
                        //This case indicates completed header
                        std::string headerlen;
                        m_accumulator.clear();
                        if (!get_header(std::string("Content-length"),
                                        headerlen) ||
                            headerlen.empty()) {
                            m_curstate = parsing_done;
                        } else {
                            m_curstate = parsing_body;
                        }
                        return;
                    }

                    m_accumulator =
                        cxx_utils::string::utils::trim(m_accumulator, " \r\n\t");

                    size_t coldelim = m_accumulator.find_first_of(":");

                    if (coldelim == std::string::npos) {
                        return;
                    }

                    if (cxx_utils::string::utils::istringcmp
                        (m_accumulator.substr(0, coldelim),
                         std::string("Cookie"))) {
                        cookie c(m_accumulator.substr(coldelim+1,
                                                      std::string::npos));
                        m_cookies.push_back(c);
                    } else {
                        set_header(m_accumulator.substr(0, coldelim),
                                   cxx_utils::string::utils::trim
                                   (m_accumulator.substr(coldelim+1,
                                                         std::string::npos)));
                    }
                    m_accumulator.clear();
                }

                void update_body()
                {
                    m_body += m_accumulator;
                    m_accumulator.clear();

                    std::string content_length;
                    get_header(std::string("Content-length"), content_length);
                    std::istringstream buf(content_length);
                    size_t len;
                    buf >> len;
                    if( m_body.size() == len) {
                        m_curstate = parsing_done;
                    }
                }
            public:
                http_request() : http_message(),
                                 m_suggestedcode(http_response::max_code),
                                 m_curmethod(max_method),
                                 m_curstate(parsing_method), m_uri(),
                                 m_nextbreaktok(' ')
                {
                }

                http_request(methods method, std::string uri,
                             std::uint32_t maj = 1, std::uint32_t min = 0) :
                    http_message(maj, min),
                    m_suggestedcode(http_response::max_code),
                    m_curmethod(method), 
                    m_curstate(parsing_done), m_uri(uri), m_nextbreaktok(' ')
                {
                }

                virtual ~http_request()
                {
                }

                http_response::codes suggested() const
                {
                    return m_suggestedcode;
                }

                void clear_suggested()
                {
                    m_suggestedcode = http_response::max_code;
                }

                virtual void updated()
                {
                    if( m_accumulator.size() == 0 ||
                        *(m_accumulator.end() - 1) != m_nextbreaktok) {
                        return;
                    }

                    switch(m_curstate){
                    case parsing_done:
                        m_suggestedcode =
                            http_response::request_entity_too_large;
                        m_curstate = parsing_err;
                    case parsing_err:
                        break;
                    default:
                        m_suggestedcode = http_response::bad_request;
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

                    std::string hdrval;

                    result += "Date: ";
                    if( !get_header("Date", hdrval) )
                        result += cxx_utils::net::http::utils::webdate
                            (time(NULL)) ;
                    else
                        result += hdrval;
                    result += "\r\n";

                    result += "User-Agent: ";
                    if( !get_header("User-Agent", hdrval) )
                        result += "cxxutils 0.1";
                    else
                        result += hdrval;
                    result += "\r\n";

                    for(http_message::headers::const_iterator it =
                            m_httphdrs.cbegin();
                        it != m_httphdrs.cend(); ++it) {
                        if(cxx_utils::string::utils::istringcmp(it->first,
                                                                "Date") ||
                           cxx_utils::string::utils::istringcmp(it->first,
                                                                "User-Agent") ||
                           cxx_utils::string::utils::istringcmp(it->first,
                                                                "Content-Length")) {
                            continue;
                        }
                        result += it->first;
                        result += ": ";
                        result += it->second;
                        result += "\r\n";
                    }

                    for(cookiejar::const_iterator ick = m_cookies.cbegin();
                        ick != m_cookies.cend(); ++ick) {
                        result += "Cookie: ";
                        result += ick->value();
                        result += "\r\n";
                    }

                    if (!m_body.empty()) {
                        result += "Content-Length: " +
                            std::to_string(m_body.length());
                        result += "\r\n";
                        result += m_body;
                    }
                    result += "\r\n";
                    return result;
                }
            };
        }
    }
}
#endif

