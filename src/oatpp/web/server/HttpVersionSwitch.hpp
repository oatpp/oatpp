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

#ifndef oatpp_web_server_HttpVersionSwitch_hpp
#define oatpp_web_server_HttpVersionSwitch_hpp

#include "oatpp/network/ConnectionHandler.hpp"

namespace oatpp { namespace web { namespace server {

class HttpVersionSwitch : public base::Countable, public network::ConnectionHandler {
 private:
  std::shared_ptr<network::ConnectionHandler> m_http1;
  std::shared_ptr<network::ConnectionHandler> m_http2;

 public:
  HttpVersionSwitch(const std::shared_ptr<network::ConnectionHandler> &http1, const std::shared_ptr<network::ConnectionHandler> &http2) : m_http1(http1), m_http2(http2) {}
  static std::shared_ptr<HttpVersionSwitch> createShared(const std::shared_ptr<network::ConnectionHandler> &http1, const std::shared_ptr<network::ConnectionHandler> &http2) {
    return std::make_shared<HttpVersionSwitch>(http1, http2);
  }

  void handleConnection(const std::shared_ptr<IOStream> &connection,
                        const std::shared_ptr<const ParameterMap> &params) override;

  void stop() override;

};

}}}
#endif //oatpp_web_server_HttpVersionSwitch_hpp
