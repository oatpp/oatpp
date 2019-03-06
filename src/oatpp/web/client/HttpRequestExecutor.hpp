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

#ifndef oatpp_web_client_HttpRequestExecutor_hpp
#define oatpp_web_client_HttpRequestExecutor_hpp

#include "./RequestExecutor.hpp"

#include "oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp"
#include "oatpp/network/ConnectionProvider.hpp"

namespace oatpp { namespace web { namespace client {
  
class HttpRequestExecutor : public oatpp::base::Countable, public RequestExecutor {
private:
  typedef oatpp::web::protocol::http::Header Header;
protected:
  std::shared_ptr<oatpp::network::ClientConnectionProvider> m_connectionProvider;
  std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder> m_bodyDecoder;
public:
  class HttpConnectionHandle : public ConnectionHandle {
  public:
    HttpConnectionHandle(const std::shared_ptr<oatpp::network::ConnectionProvider::IOStream>& stream)
      : connection(stream)
    {}
    std::shared_ptr<oatpp::network::ConnectionProvider::IOStream> connection;
  };
public:
  HttpRequestExecutor(const std::shared_ptr<oatpp::network::ClientConnectionProvider>& connectionProvider,
                      const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& bodyDecoder =
                      std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>())
    : m_connectionProvider(connectionProvider)
    , m_bodyDecoder(bodyDecoder)
  {}
public:
  
  static std::shared_ptr<HttpRequestExecutor>
  createShared(const std::shared_ptr<oatpp::network::ClientConnectionProvider>& connectionProvider,
               const std::shared_ptr<const oatpp::web::protocol::http::incoming::BodyDecoder>& bodyDecoder =
               std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>()){
    return std::make_shared<HttpRequestExecutor>(connectionProvider, bodyDecoder);
  }
  
  std::shared_ptr<ConnectionHandle> getConnection() override;
  
  Action getConnectionAsync(oatpp::async::AbstractCoroutine* parentCoroutine, AsyncConnectionCallback callback) override;
  
  /**
   *  throws RequestExecutionError
   */
  std::shared_ptr<Response> execute(const String& method,
                                    const String& path,
                                    const Headers& headers,
                                    const std::shared_ptr<Body>& body,
                                    const std::shared_ptr<ConnectionHandle>& connectionHandle = nullptr) override;
  
  Action executeAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                      AsyncCallback callback,
                      const String& method,
                      const String& path,
                      const Headers& headers,
                      const std::shared_ptr<Body>& body,
                      const std::shared_ptr<ConnectionHandle>& connectionHandle = nullptr) override;
  
};
  
}}}

#endif /* oatpp_web_client_HttpRequestExecutor_hpp */
