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

#ifndef oatpp_web_protocol_http_outgoing_Response_hpp
#define oatpp_web_protocol_http_outgoing_Response_hpp

#include "oatpp/web/protocol/http/outgoing/Body.hpp"
#include "oatpp/web/protocol/http/encoding/EncoderProvider.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include "oatpp/network/ConnectionHandler.hpp"

#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

#include "oatpp/core/data/Bundle.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Class which stores information of outgoing http Response.
 */
class Response : public oatpp::base::Countable {
public:
  /**
   * Convenience typedef for Headers. <br>
   * See &id:oatpp::web::protocol::http::Headers;
   */
  typedef http::Headers Headers;

  /**
   * Convenience typedef for &id:oatpp::network::ConnectionHandler;.
   */
  typedef oatpp::network::ConnectionHandler ConnectionHandler;
private:
  Status m_status;
  Headers m_headers;
  std::shared_ptr<Body> m_body;
  std::shared_ptr<ConnectionHandler> m_connectionUpgradeHandler;
  std::shared_ptr<const ConnectionHandler::ParameterMap> m_connectionUpgradeParameters;
  data::Bundle m_bundle;
public:
  /**
   * Constructor.
   * @param status - http status.
   * @param body - response body.
   */
  Response(const Status& status, const std::shared_ptr<Body>& body);
public:

  /**
   * Create shared outgoing response with status and body.
   * @param status - http status.
   * @param body - response body.
   * @return
   */
  static std::shared_ptr<Response> createShared(const Status& status, const std::shared_ptr<Body>& body);

  /**
   * Get status.
   * @return - http status.
   */
  const Status& getStatus() const;

  /**
   * Get headers.
   * @return - &id:oatpp::web::protocol::http::Headers;
   */
  Headers& getHeaders();

  /**
   * Get body
   * @return - &id:oatpp::web::protocol::http::outgoing::Body;
   */
  std::shared_ptr<Body> getBody() const;

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
   * Set connection upgreade header. <br>
   * Use it together with corresponding headers being set when Response is created as: <br>
   * Response(&id:oatpp::web::protocol::http::Status::CODE_101;, nullptr);<br>
   * @param handler - `std::shared_ptr` to &id:oatpp::network::ConnectionHandler;.
   */
  void setConnectionUpgradeHandler(const std::shared_ptr<ConnectionHandler>& handler);

  /**
   * Get currently set connection upgrade handler.
   * @return - `std::shared_ptr` to &id:oatpp::network::ConnectionHandler;.
   */
  std::shared_ptr<ConnectionHandler> getConnectionUpgradeHandler();

  /**
   * Set connection upgrade parameters. <br>
   * Use it to set additional parameters for upgraded connection handling. See &l:Response::setConnectionUpgradeHandler ();.
   * @param parameters - `std::shared_ptr` to const &id:oatpp::network::ConnectionHandler::ParameterMap;.
   */
  void setConnectionUpgradeParameters(const std::shared_ptr<const ConnectionHandler::ParameterMap>& parameters);

  /**
   * Get connection upgrade parameters.
   * @return - `std::shared_ptr` to const &id:oatpp::network::ConnectionHandler::ParametersMap;.
   */
  std::shared_ptr<const ConnectionHandler::ParameterMap> getConnectionUpgradeParameters();

  /**
   * Write this Response to stream.
   * @param stream - pointer to &id:oatpp::data::stream::OutputStream;.
   * @param headersWriteBuffer - pointer to &id:oatpp::data::stream::BufferOutputStream;.
   * @param contentEncoder - pointer to &id:oatpp::web::protocol::http::encoding::EncoderProvider;.
   */
  void send(data::stream::OutputStream* stream,
            data::stream::BufferOutputStream* headersWriteBuffer,
            http::encoding::EncoderProvider* contentEncoderProvider);

  /**
   * Same as &l:Response::send (); but async.
   * @param _this - `this` response.
   * @param stream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   * @param headersWriteBuffer - `std::shared_ptr` to &id:oatpp::data::stream::BufferOutputStream;.
   * @param contentEncoderProvider - `std::shared_ptr` to &id:oatpp::web::protocol::http::encoding::EncoderProvider;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  static oatpp::async::CoroutineStarter sendAsync(const std::shared_ptr<Response>& _this,
                                                  const std::shared_ptr<data::stream::OutputStream>& stream,
                                                  const std::shared_ptr<data::stream::BufferOutputStream>& headersWriteBuffer,
                                                  const std::shared_ptr<http::encoding::EncoderProvider>& contentEncoderProvider);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Response_hpp */
