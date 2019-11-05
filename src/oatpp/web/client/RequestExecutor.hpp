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

#ifndef oatpp_web_client_RequestExecutor_hpp
#define oatpp_web_client_RequestExecutor_hpp

#include "RetryPolicy.hpp"

#include "oatpp/web/protocol/http/incoming/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace client {

/**
 * Abstract RequestExecutor.
 * RequestExecutor is class responsible for making remote requests.
 */
class RequestExecutor {
public:
  /**
   * Convenience typedef for &id:oatpp::String;.
   */
  typedef oatpp::String String;

  /**
   * Convenience typedef for &id:oatpp::async::Action;.
   */
  typedef oatpp::async::Action Action;
public:
  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Headers;.
   */
  typedef oatpp::web::protocol::http::Headers Headers;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::incoming::Response;.
   */
  typedef oatpp::web::protocol::http::incoming::Response Response;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::outgoing::Body;.
   */
  typedef oatpp::web::protocol::http::outgoing::Body Body;
public:
  
  /**
   * ConnectionHandle is always specific to a RequestExecutor.
   * You can't pass ConnectionHandle retrieved by one RequestExecutor implementation
   * to another
   */
  class ConnectionHandle {
  public:
    virtual ~ConnectionHandle() {}
  };
  
public:
  typedef Action (oatpp::async::AbstractCoroutine::*AsyncCallback)(const std::shared_ptr<Response>&);
  typedef Action (oatpp::async::AbstractCoroutine::*AsyncConnectionCallback)(const std::shared_ptr<ConnectionHandle>&);
public:

  /**
   * Class representing Request Execution Error.
   */
  class RequestExecutionError : public std::runtime_error {
  public:
    /**
     * Error code for "can't connect" error.
     */
    constexpr static const v_int32 ERROR_CODE_CANT_CONNECT = 1;

    /**
     * Error code for "can't parse starting line" error.
     */
    constexpr static const v_int32 ERROR_CODE_CANT_PARSE_STARTING_LINE = 2;

    /**
     * Error code for "can't parse headers" error.
     */
    constexpr static const v_int32 ERROR_CODE_CANT_PARSE_HEADERS = 3;

    /**
     * Error code for "can't read response" error.
     */
    constexpr static const v_int32 ERROR_CODE_CANT_READ_RESPONSE = 4;

    /**
     * Error code for "no response" error.
     */
    constexpr static const v_int32 ERROR_CODE_NO_RESPONSE = 5;
  private:
    v_int32 m_errorCode;
    const char* m_message;
    v_int32 m_readErrorCode;
  public:

    /**
     * Constructor.
     * @param errorCode - error code.
     * @param message - error message.
     * @param readErrorCode - io error code.
     */
    RequestExecutionError(v_int32 errorCode, const char* message, v_int32 readErrorCode = 0);

    /**
     * Get error code.
     * @return - error code.
     */
    v_int32 getErrorCode() const;

    /**
     * Get error message.
     * @return - error message.
     */
    const char* getMessage() const;
    
    /**
     *  This value is valid if errorCode == &l:RequestExecutor::RequestExecutionError::ERROR_CODE_CANT_READ_RESPONSE; <br>
     *  For more information about the read error you get check out:
     *  <ul>
     *    <li>&id:oatpp::data::stream::IOStream; for possible error codes.</li>
     *    <li>Implementation-specific behaviour of corresponding Connection and ConnectionProvider.</li>
     *  </ul>
     */
    v_int32 getReadErrorCode() const;
    
  };

private:
  std::shared_ptr<RetryPolicy> m_retryPolicy;
public:

  /**
   * Constructor.
   * @param retryPolicy - &id:oatpp::web::client::RetryPolicy;.
   */
  RequestExecutor(const std::shared_ptr<RetryPolicy>& retryPolicy);

  /**
   * Virtual destructor.
   */
  virtual ~RequestExecutor() = default;

  /**
   * Obtain &l:RequestExecutor::ConnectionHandle; which then can be passed to &l:RequestExecutor::execute ();.
   * @return std::shared_ptr to &l:RequestExecutor::ConnectionHandle;.
   */
  virtual std::shared_ptr<ConnectionHandle> getConnection() = 0;

  /**
   * Same as &l:RequestExecutor::getConnection (); but Async.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ConnectionHandle>&> getConnectionAsync() = 0;

  /**
   * Invalidate connection.
   * @param connectionHandle
   */
  virtual void invalidateConnection(const std::shared_ptr<ConnectionHandle>& connectionHandle) = 0;

  /**
   * Execute request once without any retries.
   * @param method - method ex: ["GET", "POST", "PUT", etc.].
   * @param path - path to resource.
   * @param headers - headers map &l:RequestExecutor::Headers;.
   * @param body - `std::shared_ptr` to &l:RequestExecutor::Body; object.
   * @param connectionHandle - &l:RequestExecutor::ConnectionHandle;
   * @return - &id:oatpp::web::protocol::http::incoming::Response;.
   */
  virtual std::shared_ptr<Response> executeOnce(const String& method,
                                                const String& path,
                                                const Headers& headers,
                                                const std::shared_ptr<Body>& body,
                                                const std::shared_ptr<ConnectionHandle>& connectionHandle) = 0;

  /**
   * Same as &l:RequestExecutor::executeOnce (); but Async.
   * @param method - method ex: ["GET", "POST", "PUT", etc.].
   * @param path - path to resource.
   * @param headers - headers map &l:RequestExecutor::Headers;.
   * @param body - `std::shared_ptr` to &l:RequestExecutor::Body; object.
   * @param connectionHandle - &l:RequestExecutor::ConnectionHandle;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  virtual oatpp::async::CoroutineStarterForResult<const std::shared_ptr<Response>&>
  executeOnceAsync(const String& method,
                   const String& path,
                   const Headers& headers,
                   const std::shared_ptr<Body>& body,
                   const std::shared_ptr<ConnectionHandle>& connectionHandle) = 0;

  /**
   * Execute request taking into account retry policy.
   * @param method - method ex: ["GET", "POST", "PUT", etc.].
   * @param path - path to resource.
   * @param headers - headers map &l:RequestExecutor::Headers;.
   * @param body - `std::shared_ptr` to &l:RequestExecutor::Body; object.
   * @param connectionHandle - &l:RequestExecutor::ConnectionHandle;
   * @return - &id:oatpp::web::protocol::http::incoming::Response;.
   */
  virtual std::shared_ptr<Response> execute(const String& method,
                                            const String& path,
                                            const Headers& headers,
                                            const std::shared_ptr<Body>& body,
                                            const std::shared_ptr<ConnectionHandle>& connectionHandle);

  /**
   * Same as &l:RequestExecutor::execute (); but Async.
   * @param method - method ex: ["GET", "POST", "PUT", etc.].
   * @param path - path to resource.
   * @param headers - headers map &l:RequestExecutor::Headers;.
   * @param body - `std::shared_ptr` to &l:RequestExecutor::Body; object.
   * @param connectionHandle - &l:RequestExecutor::ConnectionHandle;.
   * @return - &id:oatpp::async::CoroutineStarterForResult;.
   */
  oatpp::async::CoroutineStarterForResult<const std::shared_ptr<Response>&>
  virtual executeAsync(const String& method,
                       const String& path,
                       const Headers& headers,
                       const std::shared_ptr<Body>& body,
                       const std::shared_ptr<ConnectionHandle>& connectionHandle);

};
  
}}}

#endif /* oatpp_web_client_RequestExecutor_hpp */
