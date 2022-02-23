/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
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

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

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
const Status Status::CODE_418(418, "I'm a Teapot");
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
const char* const Header::Value::CONNECTION_UPGRADE = "Upgrade";
  
const char* const Header::Value::SERVER = "oatpp/" OATPP_VERSION;
const char* const Header::Value::USER_AGENT = "oatpp/" OATPP_VERSION;
  
const char* const Header::Value::TRANSFER_ENCODING_CHUNKED = "chunked";
  
const char* const Header::Value::CONTENT_TYPE_APPLICATION_JSON = "application/json";

const char* const Header::Value::EXPECT_100_CONTINUE = "100-continue";
  
const char* const Header::ACCEPT = "Accept";
const char* const Header::AUTHORIZATION = "Authorization";
const char* const Header::WWW_AUTHENTICATE = "WWW-Authenticate";
const char* const Header::CONNECTION = "Connection";
const char* const Header::TRANSFER_ENCODING = "Transfer-Encoding";
const char* const Header::CONTENT_ENCODING = "Content-Encoding";
const char* const Header::CONTENT_LENGTH = "Content-Length";
const char* const Header::CONTENT_TYPE = "Content-Type";
const char* const Header::CONTENT_RANGE = "Content-Range";
const char* const Header::RANGE = "Range";
const char* const Header::HOST = "Host";
const char* const Header::USER_AGENT = "User-Agent";
const char* const Header::SERVER = "Server";
const char* const Header::UPGRADE = "Upgrade";


const char* const Header::CORS_ORIGIN = "Access-Control-Allow-Origin";
const char* const Header::CORS_METHODS = "Access-Control-Allow-Methods";
const char* const Header::CORS_HEADERS = "Access-Control-Allow-Headers";
const char* const Header::CORS_MAX_AGE = "Access-Control-Max-Age";

const char* const Header::ACCEPT_ENCODING = "Accept-Encoding";

const char* const Header::EXPECT = "Expect";

const char* const Range::UNIT_BYTES = "bytes";
const char* const ContentRange::UNIT_BYTES = "bytes";
  
oatpp::String Range::toString() const {
  data::stream::BufferOutputStream stream(256);
  stream.writeSimple(units->data(), units->size());
  stream.writeSimple("=", 1);
  stream.writeAsString(start);
  stream.writeSimple("-", 1);
  stream.writeAsString(end);
  return stream.toString();
}

Range Range::parse(oatpp::parser::Caret& caret) {

  auto unitsLabel = caret.putLabel();
  if(caret.findChar('=')) {
    unitsLabel.end();
    caret.inc();
  } else {
    caret.setError("'=' - expected");
    return Range();
  }

  auto startLabel = caret.putLabel();
  if(caret.findChar('-')) {
    startLabel.end();
    caret.inc();
  } else {
    caret.setError("'-' - expected");
    return Range();
  }

  auto endLabel = caret.putLabel();
  caret.findRN();
  endLabel.end();

  auto start = oatpp::utils::conversion::strToInt64((const char*) startLabel.getData());
  auto end = oatpp::utils::conversion::strToInt64((const char*) endLabel.getData());
  return Range(unitsLabel.toString(), start, end);
  
}

Range Range::parse(const oatpp::String& str) {
  oatpp::parser::Caret caret(str);
  return parse(caret);
}

oatpp::String ContentRange::toString() const {
  data::stream::BufferOutputStream stream(256);
  stream.writeSimple(units->data(), units->size());
  stream.writeSimple(" ", 1);
  stream.writeAsString(start);
  stream.writeSimple("-", 1);
  stream.writeAsString(end);
  stream.writeSimple("/", 1);
  if(isSizeKnown) {
    stream.writeAsString(size);
  } else {
    stream.writeSimple("*", 1);
  }
  return stream.toString();
}

ContentRange ContentRange::parse(oatpp::parser::Caret& caret) {

  auto unitsLabel = caret.putLabel();
  if(caret.findChar(' ')) {
    unitsLabel.end();
    caret.inc();
  } else {
    caret.setError("' ' - expected");
    return ContentRange();
  }

  auto startLabel = caret.putLabel();
  if(caret.findChar('-')) {
    startLabel.end();
    caret.inc();
  } else {
    caret.setError("'-' - expected");
    return ContentRange();
  }

  auto endLabel = caret.putLabel();
  if(caret.findChar('/')) {
    endLabel.end();
    caret.inc();
  } else {
    caret.setError("'/' - expected");
    return ContentRange();
  }

  auto sizeLabel = caret.putLabel();
  caret.findRN();
  sizeLabel.end();
  
  v_int64 start = oatpp::utils::conversion::strToInt64((const char*) startLabel.getData());
  v_int64 end = oatpp::utils::conversion::strToInt64((const char*) endLabel.getData());
  v_int64 size = 0;
  bool isSizeKnown = false;
  if(sizeLabel.getData()[0] != '*') {
    isSizeKnown = true;
    size = oatpp::utils::conversion::strToInt64((const char*) sizeLabel.getData());
  }
  
  return ContentRange(unitsLabel.toString(), start, end, size, isSizeKnown);
  
}

ContentRange ContentRange::parse(const oatpp::String& str) {
  oatpp::parser::Caret caret(str);
  return parse(caret);
}


oatpp::String HeaderValueData::getTitleParamValue(const data::share::StringKeyLabelCI& key) const {
  auto it = titleParams.find(key);
  if(it != titleParams.end()) {
    return it->second.toString();
  }
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parser
  
oatpp::data::share::StringKeyLabelCI Parser::parseHeaderNameLabel(const std::shared_ptr<std::string>& headersText,
                                                                  oatpp::parser::Caret& caret) {
  const char* data = caret.getData();
  for(v_buff_size i = caret.getPosition(); i < caret.getDataSize(); i++) {
    v_char8 a = data[i];
    if(a == ':' || a == ' '){
      oatpp::data::share::StringKeyLabelCI label(headersText, &data[caret.getPosition()], i - caret.getPosition());
      caret.setPosition(i);
      return label;
      
    }
  }
  return oatpp::data::share::StringKeyLabelCI(nullptr, nullptr, 0);
}
  
void Parser::parseRequestStartingLine(RequestStartingLine& line,
                                      const std::shared_ptr<std::string>& headersText,
                                      oatpp::parser::Caret& caret,
                                      Status& error) {

  auto methodLabel = caret.putLabel();
  if(caret.findChar(' ')){
    line.method = oatpp::data::share::StringKeyLabel(headersText, methodLabel.getData(), methodLabel.getSize());
    caret.inc();
  } else {
    error = Status::CODE_400;
    return;
  }

  auto pathLabel = caret.putLabel();
  if(caret.findChar(' ')){
    line.path = oatpp::data::share::StringKeyLabel(headersText, pathLabel.getData(), pathLabel.getSize());
    caret.inc();
  } else {
    error = Status::CODE_400;
    return;
  }

  auto protocolLabel = caret.putLabel();
  if(caret.findRN()){
    line.protocol = oatpp::data::share::StringKeyLabel(headersText, protocolLabel.getData(), protocolLabel.getSize());
    caret.skipRN();
  } else {
    error = Status::CODE_400;
    return;
  }
  
}
  
void Parser::parseResponseStartingLine(ResponseStartingLine& line,
                                       const std::shared_ptr<std::string>& headersText,
                                       oatpp::parser::Caret& caret,
                                       Status& error) {

  auto protocolLabel = caret.putLabel();
  if(caret.findChar(' ')){
    line.protocol = oatpp::data::share::StringKeyLabel(headersText, protocolLabel.getData(), protocolLabel.getSize());
    caret.inc();
  } else {
    error = Status::CODE_400;
    return;
  }

  line.statusCode = (v_int32)caret.parseInt();

  auto descriptionLabel = caret.putLabel();
  if(caret.findRN()){
    line.description = oatpp::data::share::StringKeyLabel(headersText, descriptionLabel.getData(), descriptionLabel.getSize());
    caret.skipRN();
  } else {
    error = Status::CODE_400;
    return;
  }
  
}
  
void Parser::parseOneHeader(Headers& headers,
                            const std::shared_ptr<std::string>& headersText,
                            oatpp::parser::Caret& caret,
                            Status& error)
{
  caret.skipChar(' ');
  auto name = parseHeaderNameLabel(headersText, caret);
  if(name.getData() != nullptr) {
    caret.skipChar(' ');
    if(!caret.canContinueAtChar(':', 1)) {
      error = Status::CODE_400;
      return;
    }
    caret.skipChar(' ');
    v_buff_size valuePos0 = caret.getPosition();
    caret.findRN();
    headers.put_LockFree(name, oatpp::data::share::StringKeyLabel(headersText, &caret.getData()[valuePos0], caret.getPosition() - valuePos0));
    caret.skipRN();
  } else {
    error = Status::CODE_431;
    return;
  }
}

void Parser::parseHeaders(Headers& headers,
                          const std::shared_ptr<std::string>& headersText,
                          oatpp::parser::Caret& caret,
                          Status& error)
{
  
  while (!caret.isAtRN()) {
    parseOneHeader(headers, headersText, caret, error);
    if(error.code != 0) {
      return;
    }
  }
  
  caret.skipRN();
  
}

void Parser::parseHeaderValueData(HeaderValueData& data, const oatpp::data::share::StringKeyLabel& headerValue, char separator) {

  oatpp::parser::Caret caret((const char*) headerValue.getData(), headerValue.getSize());

  const char charSet[5] = {' ', '=', separator, '\r', '\n'};
  const char charSet2[4] = {' ', separator, '\r', '\n'};

  while (caret.canContinue()) {

    caret.skipChar(' ');

    auto label = caret.putLabel();
    auto res = caret.findCharFromSet(charSet, 5);

    if (res == '=') {

      data::share::StringKeyLabelCI key(headerValue.getMemoryHandle(), label.getData(), label.getSize());
      caret.inc();

      if (caret.isAtChar('"')) {
        label = caret.parseStringEnclosed('"', '"', '\\');
      } else if (caret.isAtChar('\'')) {
        label = caret.parseStringEnclosed('\'', '\'', '\\');
      } else {
        label = caret.putLabel();
        caret.findCharFromSet(charSet2, 4);
      }

      data.titleParams[key] = data::share::StringKeyLabel(headerValue.getMemoryHandle(), label.getData(),
                                                          label.getSize());

    } else {
      data.tokens.insert(
        data::share::StringKeyLabelCI(headerValue.getMemoryHandle(), label.getData(), label.getSize()));
    }

    if (caret.isAtCharFromSet("\r\n", 2)) {
      break;
    } else if (caret.isAtChar(separator)) {
      caret.inc();
    }

  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utils

void Utils::writeHeaders(const Headers& headers, data::stream::ConsistentOutputStream* stream) {

  auto& map = headers.getAll_Unsafe();
  auto it = map.begin();
  while(it != map.end()) {
    stream->writeSimple(it->first.getData(), it->first.getSize());
    stream->writeSimple(": ", 2);
    stream->writeSimple(it->second.getData(), it->second.getSize());
    stream->writeSimple("\r\n", 2);
    it ++;
  }

}

}}}}
