#ifndef __HTTP_MESSAGE__H__
#error Cannot include this file directly
#endif

#ifndef __HTTP_RESPONSE_MESSAGE_H__
#define __HTTP_RESPONSE_MESSAGE_H__
namespace cxx_utils
{
    namespace net
    {
        namespace http
        {
            class http_response : public http_message
            {
                enum parsing_states {
                    parsing_version,
                    parsing_status,
                    parsing_reason,
                    parsing_headers,
                    parsing_body,
                    parsing_done,

                    parsing_err
                };

            public:
                enum codes {
                    /* 1xx: informational */
                    cont = 100,
                    switching_protocols = 101,

                    /* 2xx: success */
                    okay = 200,
                    created = 201,
                    accepted = 202,
                    non_authoritative = 203,
                    no_content = 204,
                    reset_content = 205,
                    partial_content = 206,
                    
                    /* 3xx: redirection */
                    multiple_choices=300,
                    moved_permanently=301,
                    found=302,
                    see_other=303,
                    not_modified=304,
                    use_proxy=305,
                    temporary_redirect=307,

                    /* 4xx: client error */
                    bad_request=400,
                    unauthorized=401,
                    payment_required=402,
                    forbidden=403,
                    not_found=404,
                    method_not_allowed=405,
                    not_acceptable=406,
                    proxy_auth_required=407,
                    request_time_out=408,
                    conflict=409,
                    gone=410,
                    length_required=411,
                    precondition_failed=412,
                    request_entity_too_large=413,
                    request_uri_too_large=414,
                    unsupported_media_type=415,
                    request_range_not_satisfiable=416,
                    expectation_failed=417,

                    /* 5xx: server error */
                    internal_error=500,
                    not_implemented=501,
                    bad_gateway=502,
                    service_unavailable=503,
                    gateway_timeout=504,
                    http_version_not_supported=505,

                    max_code=999
                };

            protected:

                codes          m_curcode;
                parsing_states m_curstate;
                std::string    m_uri;
                char           m_nextbreaktok;

            public:
            };
        }
    }
}
#endif
