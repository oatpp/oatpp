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

#ifndef oatpp_web_protocol_http_outgoing_Body_hpp
#define oatpp_web_protocol_http_outgoing_Body_hpp

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/collection/ListMap.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class Body {
protected:
  typedef oatpp::async::Action Action;
protected:
  typedef oatpp::collection::ListMap<oatpp::String, oatpp::String> Headers;
  typedef oatpp::data::stream::OutputStream OutputStream;
public:
  
  /**
   * declare headers describing body
   */
  virtual void declareHeaders(const std::shared_ptr<Headers>& headers) noexcept = 0;
  
  /**
   * write content to stream
   */
  virtual void writeToStream(const std::shared_ptr<OutputStream>& stream) noexcept = 0;
  
  virtual Action writeToStreamAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                    const Action& actionOnReturn,
                                    const std::shared_ptr<OutputStream>& stream) = 0;
  
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Body_hpp */
