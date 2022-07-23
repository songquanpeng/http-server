//
// Created by song on 7/23/2022.
//

#include "HTTPResponse.h"

// We have to initialize it here.
// Otherwise, "multiple definition of `HTTPResponse::TYPE_HTML[abi:cxx11]'"

const std::string HTTPResponse::TYPE_HTML = "text/html";
const std::string HTTPResponse::TYPE_JSON = "application/json";

const int HTTPResponse::OK = 200;
const int HTTPResponse::NOT_FOUND = 404;