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

#ifndef oatpp_web_server_HttpError_hpp
#define oatpp_web_server_HttpError_hpp

#include "../protocol/http/Http.hpp"
#include "../../../../oatpp-lib/core/src/base/String.hpp"

namespace oatpp { namespace web { namespace server {
  
class HttpError : public std::runtime_error {
private:
  oatpp::web::protocol::http::Status m_status;
  oatpp::base::String::SharedWrapper m_message;
public:
  
  HttpError(const oatpp::web::protocol::http::Status& status,
            const oatpp::base::String::SharedWrapper& message)
    :std::runtime_error(status.description)
    , m_status(status)
    , m_message(message)
  {}
  
  oatpp::web::protocol::http::Status getStatus() {
    return m_status;
  }
  
  oatpp::base::String::SharedWrapper& getMessage(){
    return m_message;
  }
  
};
  
#define OATPP_ASSERT_HTTP(COND, STATUS, MESSAGE) \
if(!(COND)) { throw oatpp::web::server::HttpError(STATUS, MESSAGE); }
  
}}}

#endif /* oatpp_web_server_HttpError_hpp */
