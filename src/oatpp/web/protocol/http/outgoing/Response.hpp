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
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/network/server/ConnectionHandler.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

/**
 * Class which stores information of outgoing http Response.
 */
class Response : public oatpp::base::Countable, public std::enable_shared_from_this<Response> {
public:
  /**
   * Convenience typedef for Headers. <br>
   * See &id:oatpp::web::protocol::http::Headers;
   */
  typedef http::Headers Headers;

  /**
   * Convenience typedef for &id:oatpp::network::server::ConnectionHandler;.
   */
  typedef oatpp::network::server::ConnectionHandler ConnectionHandler;
public:
  OBJECT_POOL(Outgoing_Response_Pool, Response, 32)
  SHARED_OBJECT_POOL(Shared_Outgoing_Response_Pool, Response, 32)
private:
  Status m_status;
  Headers m_headers;
  std::shared_ptr<Body> m_body;
  std::shared_ptr<ConnectionHandler> m_connectionUpgradeHandler;
  std::shared_ptr<const ConnectionHandler::ParameterMap> m_connectionUpgradeParameters;
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
   * Add http header.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI_FAST;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   */
  void putHeader(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Add http header if not already exists.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI_FAST;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   * @return - `true` if header was added.
   */
  bool putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Set connection upgreade header. <br>
   * Use it together with corresponding headers being set when Response is created as: <br>
   * Response(&id:oatpp::web::protocol::http::Status::CODE_101;, nullptr);<br>
   * @param handler - `std::shared_ptr` to &id:oatpp::network::server::ConnectionHandler;.
   */
  void setConnectionUpgradeHandler(const std::shared_ptr<ConnectionHandler>& handler);

  /**
   * Get currently set connection upgrade handler.
   * @return - `std::shared_ptr` to &id:oatpp::network::server::ConnectionHandler;.
   */
  std::shared_ptr<ConnectionHandler> getConnectionUpgradeHandler();

  /**
   * Set connection upgrade parameters. <br>
   * Use it to set additional parameters for upgraded connection handling. See &l:Response::setConnectionUpgradeHandler ();.
   * @param parameters - `std::shared_ptr` to const &id:oatpp::network::server::ConnectionHandler::ParameterMap;.
   */
  void setConnectionUpgradeParameters(const std::shared_ptr<const ConnectionHandler::ParameterMap>& parameters);

  /**
   * Get connection upgrade parameters.
   * @return - `std::shared_ptr` to const &id:oatpp::network::server::ConnectionHandler::ParametersMap;.
   */
  std::shared_ptr<const ConnectionHandler::ParameterMap> getConnectionUpgradeParameters();

  /**
   * Write this Response to stream.
   * @param stream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   */
  void send(const std::shared_ptr<data::stream::OutputStream>& stream);

  /**
   * Same as &l:Response::send (); but async.
   * @param stream - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  oatpp::async::CoroutineStarter sendAsync(const std::shared_ptr<data::stream::OutputStream>& stream);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Response_hpp */
