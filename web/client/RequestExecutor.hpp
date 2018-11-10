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

#ifndef oatpp_web_client_RequestExecutor_hpp
#define oatpp_web_client_RequestExecutor_hpp

#include "oatpp/web/protocol/http/incoming/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/Body.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace client {
  
class RequestExecutor {
public:
  typedef oatpp::String String;
  typedef oatpp::async::Action Action;
public:
  typedef oatpp::web::protocol::http::Protocol::Headers Headers;
  typedef oatpp::web::protocol::http::incoming::Response Response;
  typedef oatpp::web::protocol::http::outgoing::Body Body;
public:
  
  /**
   * ConnectionHandle is always specific to a RequestExecutor
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
  
  class RequestExecutionError : public std::runtime_error {
  public:
    constexpr static const v_int32 ERROR_CODE_CANT_CONNECT = 1;
    constexpr static const v_int32 ERROR_CODE_CANT_PARSE_STARTING_LINE = 2;
    constexpr static const v_int32 ERROR_CODE_CANT_PARSE_HEADERS = 3;
    constexpr static const v_int32 ERROR_CODE_CANT_READ_RESPONSE = 4;
    constexpr static const v_int32 ERROR_CODE_NO_RESPONSE = 5;
  private:
    v_int32 m_errorCode;
    const char* m_message;
    v_int32 m_readErrorCode;
  public:
    
    RequestExecutionError(v_int32 errorCode, const char* message, v_int32 readErrorCode = 0)
      :std::runtime_error(message)
      , m_errorCode(errorCode)
      , m_message(message)
      , m_readErrorCode(readErrorCode)
    {}
    
    v_int32 getErrorCode() const {
      return m_errorCode;
    }
    
    const char* getMessage() const {
      return m_message;
    }
    
    /**
     *  This value is valid if errorCode == ERROR_CODE_CANT_READ_RESPONSE
     *  For more information about the read error you get check out:
     *  - oatpp::data::stream::IOStream for possible error codes
     *  - implementation of Connection provided by your ConnectionProvider for implementation-specific behaviour
     */
    v_int32 getReadErrorCode() const {
      return m_readErrorCode;
    }
    
  };
  
public:
  
  /**
   * Obtain ConnectionHandle which then can be passed to execute()
   */
  virtual std::shared_ptr<ConnectionHandle> getConnection() = 0;
  
  /**
   * Same as getConnection but Async
   */
  virtual Action getConnectionAsync(oatpp::async::AbstractCoroutine* parentCoroutine, AsyncConnectionCallback callback) = 0;
  
  virtual std::shared_ptr<Response> execute(const String& method,
                                            const String& path,
                                            const std::shared_ptr<Headers>& headers,
                                            const std::shared_ptr<Body>& body,
                                            const std::shared_ptr<ConnectionHandle>& connectionHandle = nullptr) = 0;
  
  virtual Action executeAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                              AsyncCallback callback,
                              const String& method,
                              const String& path,
                              const std::shared_ptr<Headers>& headers,
                              const std::shared_ptr<Body>& body,
                              const std::shared_ptr<ConnectionHandle>& connectionHandle = nullptr) = 0;
  
};
  
}}}

#endif /* oatpp_web_client_RequestExecutor_hpp */
