/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#ifndef oatpp_web_protocol_http2_Http2_hpp
#define oatpp_web_protocol_http2_Http2_hpp

#include "oatpp/network/tcp/Connection.hpp"

#include "oatpp/web/protocol/CommunicationError.hpp"

#include "oatpp/core/parser/Caret.hpp"
#include "oatpp/core/data/share/LazyStringMap.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>
#include <unordered_set>

namespace oatpp { namespace web { namespace protocol { namespace http2 {

typedef oatpp::data::share::StringKeyLabelCI HeaderKey;

/**
 * Typedef for headers map. Headers map key is case-insensitive.
 * For more info see &id:oatpp::data::share::LazyStringMap;.
 */
typedef oatpp::data::share::LazyStringMultimap<HeaderKey> Headers;

typedef std::vector<v_uint8> Payload;

namespace Header {
  static const HeaderKey AUTHORITY(":authority");
  static const HeaderKey METHOD(":method");
  static const HeaderKey PATH(":path");
  static const HeaderKey SCHEME(":scheme");
  static const HeaderKey STATUS(":status");
  static const HeaderKey ACCEPT_CHARSET("accept-charset");
  static const HeaderKey ACCEPT_ENCODING("accept-encoding");
  static const HeaderKey ACCEPT_LANGUAGE("accept-language");
  static const HeaderKey ACCEPT_RANGES("accept-ranges");
  static const HeaderKey ACCEPT("accept");
  static const HeaderKey ACCESS_CONTROL_ALLOW_ORIGIN("access-control-allow-origin");
  static const HeaderKey AGE("age");
  static const HeaderKey ALLOW("allow");
  static const HeaderKey AUTHORIZATION("authorization");
  static const HeaderKey CACHE_CONTROL("cache-control");
  static const HeaderKey CONTENT_DISPOSITION("content-disposition");
  static const HeaderKey CONTENT_ENCODING("content-encoding");
  static const HeaderKey CONTENT_LANGUAGE("content-language");
  static const HeaderKey CONTENT_LENGTH("content-length");
  static const HeaderKey CONTENT_LOCATION("content-location");
  static const HeaderKey CONTENT_RANGE("content-range");
  static const HeaderKey CONTENT_TYPE("content-type");
  static const HeaderKey COOKIE("cookie");
  static const HeaderKey DATE("date");
  static const HeaderKey ETAG("etag");
  static const HeaderKey EXPECT("expect");
  static const HeaderKey EXPIRES("expires");
  static const HeaderKey FROM("from");
  static const HeaderKey HOST("host");
  static const HeaderKey IF_MATCH("if-match");
  static const HeaderKey IF_MODIFIED_SINCE("if-modified-since");
  static const HeaderKey IF_NONE_MATCH("if-none-match");
  static const HeaderKey IF_RANGE("if-range");
  static const HeaderKey IF_UNMODIFIED_SINCE("if-unmodified-since");
  static const HeaderKey LAST_MODIFIED("last-modified");
  static const HeaderKey LINK("link");
  static const HeaderKey LOCATION("location");
  static const HeaderKey MAX_FORWARDS("max-forwards");
  static const HeaderKey PROXY_AUTHENTICATE("proxy-authenticate");
  static const HeaderKey PROXY_AUTHORIZATION("proxy-authorization");
  static const HeaderKey RANGE("range");
  static const HeaderKey REFERER("referer");
  static const HeaderKey REFRESH("refresh");
  static const HeaderKey RETRY_AFTER("retry-after");
  static const HeaderKey SERVER("server");
  static const HeaderKey SET_COOKIE("set-cookie");
  static const HeaderKey STRICT_TRANSPORT_SECURITY("strict-transport-security");
  static const HeaderKey TRANSFER_ENCODING("transfer-encoding");
  static const HeaderKey USER_AGENT("user-agent");
  static const HeaderKey VARY("vary");
  static const HeaderKey VIA("via");
  static const HeaderKey WWW_AUTHENTICATE("www-authenticate");
}

class Frame : public oatpp::base::Countable {
public:

 class FrameHeader : public oatpp::base::Countable {
  public:

   class Types {
    public:
     static const v_uint8 TYPE_DATA = 0;
     static const v_uint8 HEADERS = 1;
     static const v_uint8 PRIORITY = 2;
     static const v_uint8 RST_STREAM = 3;
     static const v_uint8 SETTINGS = 4;
     static const v_uint8 PUSH_PROMISE = 5;
     static const v_uint8 PING = 6;
     static const v_uint8 GOAWAY = 7;
     static const v_uint8 WINDOW_UPDATE = 8;
     static const v_uint8 CONTINUATION = 9;
   };

   class Flags {
    public:
     static const v_uint8 SETTINGS_ACK = 1;
     static const v_uint8 PING_ACK = 1;
     static const v_uint8 HEADERS_END_HEADERS = 4;
     static const v_uint8 END_STREAM = 1;
     static const v_uint8 PADDED = 8;
     static const v_uint8 PRIORITY = 32;
   };

   class ErrorType {
     static const v_uint8 ERR_NONE = 0;
     static const v_uint8 ERR_PROTOCOL = 1;
     static const v_uint8 ERR_INTERNAL = 2;
     static const v_uint8 ERR_FLOW_CONTROL = 3;
     static const v_uint8 ERR_SETTINGS_TIMEOUT = 4;
     static const v_uint8 ERR_STREAM_CLOSED = 5;
     static const v_uint8 ERR_FRAME_SIZE = 6;
     static const v_uint8 ERR_REFUSED_STREAM = 7;
     static const v_uint8 ERR_CANCEL = 8;
     static const v_uint8 ERR_COMPRESSION = 9;
     static const v_uint8 ERR_CONNECT = 10;
     static const v_uint8 ERR_ENHANCE_YOUR_CALM = 11;
     static const v_uint8 ERR_INADEQUATE_SECURITY = 12;
   };


  private:
   static constexpr unsigned HeaderSize = 9;
   uint8_t m_data[HeaderSize];

  public:
   FrameHeader(v_int32 length, v_uint8 flags, v_uint8 type, v_uint32 streamId);
 };

protected:
 FrameHeader m_header;
 std::vector<v_uint8> m_payload;

public:
 Frame(v_int32 length, v_uint8 flags, v_uint8 type, v_uint32 streamId, std::vector<v_uint8> payload)
   : m_header(length, flags, type, streamId)
   , m_payload(std::move(payload)) {}

   static std::list<Frame> createHeaderFrames(v_uint32 streamId, const Headers &hdr, v_io_size maxFrameSize = (16*1024)-1);
};


}}}}
#endif //oatpp_web_protocol_http2_Http2_hpp
