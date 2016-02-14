#include "http_utils.hpp"
#include "http_message.hpp"
#include <iostream>
using cxx_utils::net::http::utils;

int main()
{
    struct test_str {
        const char *pre_decode;
        const char *post_decode_expected;
    } decode_tests[] = {
        {
            "http%3A%2F%2Ffoo.bar%2Ffoo%3Fq%3D3%26bar%3D%2B7%2F3%26boo%3Da%3Db%26bli%3Da+b",
            "http://foo.bar/foo?q=3&bar= 7/3&boo=a=b&bli=a b"
        },
        {
            "a+b+c+d",
            "a b c d"
        },
        {
            "test%gLI",
            "test"
        },
        {
            "a%3db%26c%3Dd",
            "a=b&c=d"
        }
    }, encode_tests[] = {
        {
            "http://foo.bar/foo?q=3&bar= 7/3&boo=a=b&bli=a b",
            "http%3A%2F%2Ffoo.bar%2Ffoo%3Fq%3D3%26bar%3D%2B7%2F3%26boo%3Da%3Db%26bli%3Da%2Bb"
        }
    };

    for( size_t i = 0; i < (sizeof(decode_tests) / sizeof(decode_tests[0]));
         ++i )
    {
        std::string into(decode_tests[i].pre_decode);
        if (std::string(decode_tests[i].post_decode_expected) !=
            utils::urldecode(into))
        {
            std::cout << "Failed to decode [" << into << "] into ["
                      << decode_tests[i].post_decode_expected << "], for test "
                      << i << std::endl;
            std::cout << "Got [" << utils::urldecode(into) << "]"
                      << std::endl;
            return 1;
        }
    }

    for( size_t i = 0; i < (sizeof(encode_tests) / sizeof(encode_tests[0]));
         ++i )
    {
        std::string into(encode_tests[i].pre_decode);
        if (std::string(encode_tests[i].post_decode_expected) !=
            utils::urlencode(into))
        {
            std::cout << "Failed to encode [" << into << "] into ["
                      << encode_tests[i].post_decode_expected << "], for test "
                      << i << std::endl;
            std::cout << "Got [" << utils::urlencode(into) << "]"
                      << std::endl;
            return 1;
        }
    }

    std::cout << "URL Encode / Decode finished" << std::endl;

    std::string req = "GET /index.html HTTP/1.0\r\nUser-Agent: Fiction\r\nServer: localhost\r\n\r\n";

    cxx_utils::net::http::http_request request;
    request << req;

    std::cout << request;
    
    return 0;
}
