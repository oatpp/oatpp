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

#ifndef oatpp_web_protocol_http_outgoing_Request_hpp
#define oatpp_web_protocol_http_outgoing_Request_hpp

#include "oatpp/web/protocol/http/outgoing/Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/data/Bundle.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Class http::outgoing::Request AKA OutgoingRequest represents client's outgoing request to server.
 */
class Request : public oatpp::base::Countable {
public:
  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Headers;.
   */
  typedef protocol::http::Headers Headers;
private:
  oatpp::data::share::StringKeyLabel m_method;
  oatpp::data::share::StringKeyLabel m_path;
  Headers m_headers;
  std::shared_ptr<Body> m_body;
  data::Bundle m_bundle;
public:

  /**
   * Constructor.
   * @param method - http method. &id:oatpp::data::share::StringKeyLabel;.
   * @param path - path to resource. &id:oatpp::data::share::StringKeyLabel;.
   * @param headers - &l:Request::Headers;.
   * @param body - `std::shared_ptr` to &id:oatpp::web::protocol::http::outgoing::Body;.
   */
  Request(const oatpp::data::share::StringKeyLabel& method,
          const oatpp::data::share::StringKeyLabel& path,
          const Headers& headers,
          const std::shared_ptr<Body>& body);
  
public:

  /**
   * Create shared Request.
   * @param method - http method. &id:oatpp::data::share::StringKeyLabel;.
   * @param path - path to resource. &id:oatpp::data::share::StringKeyLabel;.
   * @param headers - &l:Request::Headers;.
   * @param body - `std::shared_ptr` to &id:oatpp::web::protocol::http::outgoing::Body;.
   * @return - `std::shared_ptr` to Request.
   */
  static std::shared_ptr<Request> createShared(const oatpp::data::share::StringKeyLabel& method,
                                               const oatpp::data::share::StringKeyLabel& path,
                                               const Headers& headers,
                                               const std::shared_ptr<Body>& body);

  /**
   * Get http method.
   * @return - http method. &id:oatpp::data::share::StringKeyLabel;.
   */
  const oatpp::data::share::StringKeyLabel& getMethod() const;

  /**
   * Get path to resource.
   * @return - path to resource. &id:oatpp::data::share::StringKeyLabel;.
   */
  const oatpp::data::share::StringKeyLabel& getPath() const;

  /**
   * Get headers map.
   * @return - &l:Request::Headers;.
   */
  Headers& getHeaders();

  /**
   * Add http header.
   * @param key - &id:oatpp::String;.
   * @param value - &id:oatpp::String;.
   */
  void putHeader(const oatpp::String& key, const oatpp::String& value);

  /**
   * Add http header if not already exists.
   * @param key - &id:oatpp::String;.
   * @param value - &id:oatpp::String;.
   * @return - `true` if header was added.
   */
  bool putHeaderIfNotExists(const oatpp::String& key, const oatpp::String& value);

  /**
   * Replaces or adds header.
   * @param key - &id:oatpp::String;.
   * @param value - &id:oatpp::String;.
   * @return - `true` if header was replaces, `false` if header was added.
   */
  bool putOrReplaceHeader(const oatpp::String& key, const oatpp::String& value);

  /**
   * Replaces or adds header.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   * @return - `true` if header was replaces, `false` if header was added.
   */
  bool putOrReplaceHeader_Unsafe(const oatpp::data::share::StringKeyLabelCI& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Add http header.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   */
  void putHeader_Unsafe(const oatpp::data::share::StringKeyLabelCI& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Add http header if not already exists.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   * @return - `true` if header was added.
   */
  bool putHeaderIfNotExists_Unsafe(const oatpp::data::share::StringKeyLabelCI& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Get header value
   * @param headerName - &id:oatpp::data::share::StringKeyLabelCI;.
   * @return - &id:oatpp::String;.
   */
  oatpp::String getHeader(const oatpp::data::share::StringKeyLabelCI& headerName) const;

  /**
   * Put data to bundle.
   * @param key
   * @param polymorph
   */
  void putBundleData(const oatpp::String& key, const oatpp::Void& polymorph);

  /**
   * Get data from bundle by key.
   * @tparam WrapperType
   * @param key
   * @return
   */
  template<typename WrapperType>
  WrapperType getBundleData(const oatpp::String& key) const {
    return m_bundle.template get<WrapperType>(key);
  }

  /**
   * Get bundle object.
   * @return
   */
  const data::Bundle& getBundle() const;

  /**
   * Get http body.
   * @return - &id:oatpp::web::protocol::http::outgoing::Body;.
   */
  std::shared_ptr<Body> getBody();

  /**
   * Write request to stream.
   * @param stream - &id:oatpp::data::stream::OutputStream;.
   */
  void send(data::stream::OutputStream* stream);

  /**
   * Write request to stream in asynchronous manner.
   * @param _this
   * @param stream - &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  static oatpp::async::CoroutineStarter sendAsync(std::shared_ptr<Request> _this,
                                                  const std::shared_ptr<data::stream::OutputStream>& stream);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Request_hpp */
