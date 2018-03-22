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

#include "../../../../../../oatpp-lib/core/src/data/stream/Stream.hpp"
#include "../../../../../../oatpp-lib/core/src/collection/ListMap.hpp"
#include "../../../../../../oatpp-lib/core/src/async/Routine.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {
  
class Body {
protected:
  typedef oatpp::async::Action Action;
protected:
  typedef oatpp::collection::ListMap<base::String::PtrWrapper, base::String::PtrWrapper> Headers;
  typedef oatpp::data::stream::OutputStream OutputStream;
public:
  virtual void declareHeaders(const std::shared_ptr<Headers>& headers) = 0;
  
  /**
   *  Do not call this method if stream::write is non blocking!
   *  For fast (not network) BLOCKING streams only!!!
   */
  virtual void writeToStream(const std::shared_ptr<OutputStream>& stream) = 0;
  
  /**
   *  For NON blocking streams only!!!
   */
  virtual Action writeToStreamAsync(const std::shared_ptr<OutputStream>& stream) = 0;
};
  
}}}}}

#endif /* oatpp_web_protocol_http_outgoing_Body_hpp */
