#ifndef MOXT_HTTPBASE_HPP
#define MOXT_HTTPBASE_HPP

#include "moxt/common.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = net::ssl;

typedef http::request<http::string_body> HttpRequest;
typedef http::response<http::string_body> HttpResponse;
typedef http::status HttpStatus;
typedef http::field HttpHeader;
typedef http::verb HttpVerb;

class CHttpResponse {
  public:
    uint64_t status_code;
    // std::map<std::string, std::string> headers;
    std::string body;
};

#endif