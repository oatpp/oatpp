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
#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http2 {

typedef oatpp::data::share::StringKeyLabelCI HeaderKey;

/**
 * Typedef for headers map. Headers map key is case-insensitive.
 * For more info see &id:oatpp::data::share::LazyStringMap;.
 */
typedef oatpp::data::share::LazyStringMultimap<HeaderKey> Headers;

typedef std::vector<v_uint8> Payload;

namespace error {
  enum ErrorCode : v_uint8 {
    PROTOCOL_ERROR = 0x01,
    INTERNAL_ERROR = 0x02,
    FLOW_CONTROL_ERROR = 0x03,
    SETTINGS_TIMEOUT = 0x04,
    STREAM_CLOSED = 0x05,
    FRAME_SIZE_ERROR = 0x06,
    REFUSED_STREAM = 0x07,
    CANCEL = 0x08,
    COMPRESSION_ERROR = 0x09,
    CONNECT_ERROR = 0x0a,
    ENHANCE_YOUR_CALM = 0x0b,
    INADEQUATE_SECURITY = 0x0c,
    HTTP_1_1_REQUIRED = 0x0d
  };
  class Http2Error : public std::runtime_error {
   public:
    explicit Http2Error(const std::string &str) : std::runtime_error(str) {};
    explicit Http2Error(const char *str) : std::runtime_error(str) {};
    virtual ~Http2Error() = default;
    virtual const ErrorCode getH2ErrorCode() = 0;
    virtual const char* getH2ErrorCodeString() = 0;
  };

  #define HTTP2ERRORTYPE(x, c) \
    class Http2##x : public Http2Error { \
     public:                 \
      explicit Http2##x(const std::string& str) : Http2Error(str) {}; \
      explicit Http2##x(const char*str) : Http2Error(str) {}; \
      const ErrorCode getH2ErrorCode() override {return protocol::http2::error::ErrorCode::c;} \
      const char* getH2ErrorCodeString() override {return #c;}                           \
    };

  HTTP2ERRORTYPE(ProtocolError, PROTOCOL_ERROR)
  HTTP2ERRORTYPE(StreamClosed, STREAM_CLOSED)
  HTTP2ERRORTYPE(FrameSizeError, FRAME_SIZE_ERROR)
  HTTP2ERRORTYPE(CompressionError, COMPRESSION_ERROR)

  #undef HTTP2ERRORTYPE
}

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

 class Header : public oatpp::base::Countable {
  private:
   static const char* TAG;
  public:

   enum FrameType : v_uint8 {
     DATA = 0x00,
     HEADERS = 0x01,
     PRIORITY = 0x02,
     RST_STREAM = 0x03,
     SETTINGS = 0x04,
     PUSH_PROMISE = 0x05,
     PING = 0x06,
     GOAWAY = 0x07,
     WINDOW_UPDATE = 0x08,
     CONTINUATION = 0x09
   };

   class Flags {
    public:
     enum Header {
       HEADER_END_STREAM = 0x01,
       HEADER_END_HEADERS = 0x04,
       HEADER_PADDED = 0x08,
       HEADER_PRIORITY = 0x20
     };

     enum Data {
       DATA_END_STREAM = 0x01,
       DATA_PADDED = 0x08,
     };

     enum Ping {
       PING_ACK = 0x01,
     };

     enum Settings {
       SETTINGS_ACK = 0x01,
     };
   };

   class ErrorType {
    public:
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
   v_uint32 m_length;
   v_uint8 m_flags;
   FrameType m_type;
   v_uint32 m_streamId;

  public:
   static constexpr unsigned HeaderSize = 9;

   Header(v_uint32 length, v_uint8 flags, FrameType type, v_uint32 streamId);
   static std::shared_ptr<Header> createShared(v_uint32 length, v_uint8 flags, FrameType type, v_uint32 streamId) {
     return std::make_shared<Header>(length, flags, type, streamId);
   }
   static std::shared_ptr<Header> createShared(const std::shared_ptr<data::stream::InputStreamBufferedProxy> stream);

   v_io_size writeToStream(data::stream::OutputStream *stream);
   oatpp::String toString();

   v_uint32 getLength() const;
   v_uint8 getFlags() const;
   FrameType getType() const;
   v_uint32 getStreamId() const;

   static const char* frameTypeStringRepresentation(FrameType t);
 };

protected:
 Header m_header;
 std::vector<v_uint8> m_payload;

public:
 Frame(v_int32 length, v_uint8 flags, Header::FrameType type, v_uint32 streamId, std::vector<v_uint8> payload)
   : m_header(length, flags, type, streamId)
   , m_payload(std::move(payload)) {}

};


}}}}
#endif //oatpp_web_protocol_http2_Http2_hpp
