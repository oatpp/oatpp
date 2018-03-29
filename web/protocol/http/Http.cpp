/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "./Http.hpp"

#include "oatpp/test/Checker.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http {
  
const Status Status::CODE_100(100, "Continue");
const Status Status::CODE_101(101, "Switching");
const Status Status::CODE_102(102, "Processing");

const Status Status::CODE_200(200, "OK");
const Status Status::CODE_201(201, "Created");
const Status Status::CODE_202(202, "Accepted");
const Status Status::CODE_203(203, "Non-Authoritative Information");
const Status Status::CODE_204(204, "No Content");
const Status Status::CODE_205(205, "Reset Content");
const Status Status::CODE_206(206, "Partial Content");
const Status Status::CODE_207(207, "Multi-Status");
const Status Status::CODE_226(226, "IM Used");

const Status Status::CODE_300(300, "Multiple Choices");
const Status Status::CODE_301(301, "Moved Permanently");
const Status Status::CODE_302(302, "Moved Temporarily");
const Status Status::CODE_303(303, "See Other");
const Status Status::CODE_304(304, "Not Modified");
const Status Status::CODE_305(305, "Use Proxy");
const Status Status::CODE_306(306, "Reserved");
const Status Status::CODE_307(307, "Temporary Redirect");

const Status Status::CODE_400(400, "Bad Request");
const Status Status::CODE_401(401, "Unauthorized");
const Status Status::CODE_402(402, "Payment Required");
const Status Status::CODE_403(403, "Forbidden");
const Status Status::CODE_404(404, "Not Found");
const Status Status::CODE_405(405, "Method Not Allowed");
const Status Status::CODE_406(406, "Not Acceptable");
const Status Status::CODE_407(407, "Proxy Authentication Required");
const Status Status::CODE_408(408, "Request Timeout");
const Status Status::CODE_409(409, "Conflict");
const Status Status::CODE_410(410, "Gone");
const Status Status::CODE_411(411, "Length Required");
const Status Status::CODE_412(412, "Precondition Failed");
const Status Status::CODE_413(413, "Request Entity Too Large");
const Status Status::CODE_414(414, "Request-URI Too Large");
const Status Status::CODE_415(415, "Unsupported Media Type");
const Status Status::CODE_416(416, "Requested Range Not Satisfiable");
const Status Status::CODE_417(417, "Expectation Failed");
const Status Status::CODE_422(422, "Unprocessable Entity");
const Status Status::CODE_423(423, "Locked");
const Status Status::CODE_424(424, "Failed Dependency");
const Status Status::CODE_425(425, "Unordered Collection");
const Status Status::CODE_426(426, "Upgrade Required");
const Status Status::CODE_428(428, "Precondition Required");
const Status Status::CODE_429(429, "Too Many Requests");
const Status Status::CODE_431(431, "Request Header Fields Too Large");
const Status Status::CODE_434(434, "Requested host unavailable");
const Status Status::CODE_444(444, "Close connection withot sending headers");
const Status Status::CODE_449(449, "Retry With");
const Status Status::CODE_451(451, "Unavailable For Legal Reasons");

const Status Status::CODE_500(500, "Internal Server Error");
const Status Status::CODE_501(501, "Not Implemented");
const Status Status::CODE_502(502, "Bad Gateway");
const Status Status::CODE_503(503, "Service Unavailable");
const Status Status::CODE_504(504, "Gateway Timeout");
const Status Status::CODE_505(505, "HTTP Version Not Supported");
const Status Status::CODE_506(506, "Variant Also Negotiates");
const Status Status::CODE_507(507, "Insufficient Storage");
const Status Status::CODE_508(508, "Loop Detected");
const Status Status::CODE_509(509, "Bandwidth Limit Exceeded");
const Status Status::CODE_510(510, "Not Extended");
const Status Status::CODE_511(511, "Network Authentication Required");

const char* const Header::Value::CONNECTION_CLOSE = "close";
const char* const Header::Value::CONNECTION_KEEP_ALIVE = "keep-alive";
  
const char* const Header::Value::SERVER = "oatpp/0.18";
const char* const Header::Value::USER_AGENT = "oatpp/0.18";
  
const char* const Header::Value::TRANSFER_ENCODING_CHUNKED = "chunked";
  
const char* const Header::Value::CONTENT_TYPE_APPLICATION_JSON = "application/json";
  
const char* const Header::ACCEPT = "Accept";
const char* const Header::AUTHORIZATION = "Authorization";
const char* const Header::CONNECTION = "Connection";
const char* const Header::TRANSFER_ENCODING = "Transfer-Encoding";
const char* const Header::CONTENT_ENCODING = "Content-Encoding";
const char* const Header::CONTENT_LENGTH = "Content-Length";
const char* const Header::CONTENT_TYPE = "Content-Type";
const char* const Header::HOST = "Host";
const char* const Header::USER_AGENT = "User-Agent";
const char* const Header::SERVER = "Server";
  
std::shared_ptr<RequestStartingLine> Protocol::parseRequestStartingLine(oatpp::parser::ParsingCaret& caret) {
  
  auto line = RequestStartingLine::createShared();
  oatpp::parser::ParsingCaret::Label methodLabel(caret);
  if(caret.findChar(' ')){
    line->method = methodLabel.toString(true);
    caret.inc();
  } else {
    caret.setError("Invalid starting line");
    return nullptr;
  }
  
  oatpp::parser::ParsingCaret::Label pathLabel(caret);
  if(caret.findChar(' ')){
    line->path = pathLabel.toString(true);
    caret.inc();
  } else {
    caret.setError("Invalid starting line");
    return nullptr;
  }
  
  oatpp::parser::ParsingCaret::Label protocolLabel(caret);
  if(caret.findRN()){
    line->protocol = protocolLabel.toString(true);
    caret.skipRN();
  } else {
    caret.setError("Invalid starting line");
    return nullptr;
  }
  
  return line;
  
}

std::shared_ptr<ResponseStartingLine> Protocol::parseResponseStartingLine(oatpp::parser::ParsingCaret& caret) {
  
  auto line = ResponseStartingLine::createShared();
  oatpp::parser::ParsingCaret::Label protocolLabel(caret);
  if(caret.findChar(' ')){
    line->protocol = protocolLabel.toString(true);
    caret.inc();
    if(!line->protocol->startsWith((p_char8)"HTTP", 4)){
      caret.setError("Unknown protocol");
      return nullptr;
    }
  } else {
    caret.setError("Invalid starting line");
    return nullptr;
  }
  
  line->statusCode = caret.parseInt32();
  
  oatpp::parser::ParsingCaret::Label descriptionLabel(caret);
  if(caret.findRN()){
    line->description = descriptionLabel.toString(true);
    caret.skipRN();
  } else {
    caret.setError("Invalid starting line");
    return nullptr;
  }
  
  return line;
  
}
  
std::shared_ptr<oatpp::base::String> Protocol::parseHeaderName(oatpp::parser::ParsingCaret& caret) {
  p_char8 data = caret.getData();
  oatpp::parser::ParsingCaret::Label label(caret);
  for(v_int32 i = caret.getPosition(); i < caret.getSize(); i++) {
    v_char8 a = data[i];
    if(a == ':' || a == ' '){
      caret.setPosition(i);
      label.end();
      return label.toString(true);
    }
  }
  return nullptr;
}

  
std::shared_ptr<Protocol::Headers> Protocol::parseHeaders(oatpp::parser::ParsingCaret& caret, Status& error) {
  
  auto headers = Headers::createShared();
  
  while (!caret.isAtRN()) {
    
    caret.findNotSpaceChar();
    auto name = parseHeaderName(caret);
    if(name) {
      caret.findNotSpaceChar();
      if(!caret.canContinueAtChar(':', 1)) {
        error = Status::CODE_400;
        return nullptr;
      }
      caret.findNotSpaceChar();
      oatpp::parser::ParsingCaret::Label label(caret);
      caret.findRN();
      headers->put(name, label.toString(true));
      caret.skipRN();
    } else {
      error = Status::CODE_431;
      return nullptr;
    }
    
  }
  
  caret.skipRN();
  return headers;
  
}

}}}}
