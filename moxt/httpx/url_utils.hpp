#pragma once

#include <boost/regex.hpp>
#include <string>

/**
 * Implement url_utils
 */
class url_utils {
  public:
    static void parse_uri(const std::string &url, std::string &host,
                          std::string &port, std::string &path) {
        boost::smatch match;
        static const boost::regex pattern(
            "^(.*:)//([A-Za-z0-9\\-\\.]+)(:[0-9]+)?(.*)$");
        if (regex_search(url, match, pattern)) {
            std::string proto = match[1]; // we always use https or wss
            std::string _port = match[3];
            host = match[2];
            if (_port.empty()) {
                bool is_ssl =
                    (!proto.compare("https:") || !proto.compare("wss:"));
                port = is_ssl ? "443" : "80";
            } else {
                port = _port.substr(1);
            }
            if (match.size() >= 5) {
                path = match[4];
            }
        } else {
            assert(0);
        }
    }

    static void parse_uri(const std::string &url, std::string &host,
                          std::string &port) {
        std::string path;
        parse_uri(url, host, port, path);
    }

}; // url_utils