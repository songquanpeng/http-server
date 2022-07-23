//
// Created by song on 7/23/2022.
//

#ifndef SERVER_HTTPRESPONSE_H
#define SERVER_HTTPRESPONSE_H

#include <unordered_map>
#include <string>
#include "Callbacks.h"
#include "Logger.h"

class HTTPResponse {
public:
    const static std::string TYPE_HTML;
    const static std::string TYPE_JSON;

    const static int OK;
    const static int NOT_FOUND;

    HTTPResponse *setField(const std::string &key, const std::string &value) {
        fields[key] = value;
        return this;
    }

    HTTPResponse *setContentType(const std::string &type = TYPE_HTML) {
        setField("Content-Type", type);
        return this;
    }

    HTTPResponse *setBody(const std::string &body) {
        body_ = body;
        return this;
    }

    HTTPResponse *setStatus(int status) {
        status_ = status;
        return this;
    }

    std::string toString() {
        if (!body_.empty()) {
            if (fields.count("Content-Type") == 0) {
                fields["Content-Type"] = TYPE_HTML;
            }
            fields["Content-Length"] = std::to_string(body_.size());
        }
        std::string statusString;
        switch (status_) {
            case 200:
                statusString = "200 OK";
                break;
            case 404:
                statusString = "404 Not Found";
                break;
            default:
                statusString = "200 OK";
                LOG_WARN("HTTPResponse::toString status code %d not recognized")
                break;
        }
        std::string str = "HTTP/1.1 " + statusString + "\r\n";
        for (auto &field: fields) {
            str += field.first + ": " + field.second + "\r\n";
        }
        str += "\n";
        str += body_;
        return str;
    }

    void setSender(HTTPSender sender) {
        sender_ = sender;
        senderIsSet = true;
    }

    void send() {
        if (senderIsSet) {
            sender_(toString());
        } else {
            LOG_ERROR("HTTPResponse::send try to send response but sender is not set")
        }
    }

    void send(const std::string &content) {
        body_ = content;
        send();
    }

private:
    int status_ = 200;
    std::unordered_map<std::string, std::string> fields;
    std::string body_;
    HTTPSender sender_;
    bool senderIsSet = false;
};


#endif //SERVER_HTTPRESPONSE_H
