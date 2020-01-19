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

#ifndef oatpp_web_protocol_http_utils_CommunicationUtils_hpp
#define oatpp_web_protocol_http_utils_CommunicationUtils_hpp

#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/encoding/ProviderCollection.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace utils {

/**
 * Helper class for communication utils.
 */
class CommunicationUtils {
public:
  /**
   * Connection state - close.
   */
  static constexpr v_int32 CONNECTION_STATE_CLOSE = 0;

  /**
   * Connection state - keep alive.
   */
  static constexpr v_int32 CONNECTION_STATE_KEEP_ALIVE = 1;

  /**
   * Connection state - upgrade.
   */
  static constexpr v_int32 CONNECTION_STATE_UPGRADE = 2;
private:
  static bool headerEqualsCI_FAST(const oatpp::data::share::MemoryLabel& headerValue, const char* value);
public:

  /**
   * Consider keep connection alive taking into account request headers, response headers and protocol version.<br>
   * Corresponding header will be set to response if not existed before. <br>
   * return one of (CONNECTION_STATE_CLOSE, CONNECTION_STATE_KEEP_ALIVE, CONNECTION_STATE_UPGRADE).
   * @param request - `std::shared_ptr` to &id:oatpp::web::protocol::http::incoming::Request;
   * @param response - `std::shared_ptr` to &id:oatpp::web::protocol::http::outgoing::Response;
   * @return - one of values:
   * <ul>
   *   <li>&l:CommunicationUtils::CONNECTION_STATE_CLOSE;</li>
   *   <li>&l:CommunicationUtils::CONNECTION_STATE_KEEP_ALIVE;</li>
   *   <li>&l:CommunicationUtils::CONNECTION_STATE_UPGRADE;</li>
   * </ul>
   */
  static v_int32 considerConnectionState(const std::shared_ptr<protocol::http::incoming::Request>& request,
                                         const std::shared_ptr<protocol::http::outgoing::Response>& response);

  static std::shared_ptr<encoding::EncoderProvider> selectEncoder(const std::shared_ptr<http::incoming::Request>& request,
                                                                  const std::shared_ptr<http::encoding::ProviderCollection>& providers);
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_utils_CommunicationUtils_hpp */
