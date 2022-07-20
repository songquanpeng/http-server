//
// Created by song on 7/20/2022.
//

#ifndef SERVER_HTTPREQUEST_H
#define SERVER_HTTPREQUEST_H

#include <unordered_map>
#include <string>

class HTTPRequest {
public:
    std::string method;
    std::string url;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};


#endif //SERVER_HTTPREQUEST_H
