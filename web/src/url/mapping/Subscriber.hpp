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

#ifndef oatpp_network_url_Subscriber_hpp
#define oatpp_network_url_Subscriber_hpp

#include "../../../../../oatpp/core/src/base/PtrWrapper.hpp"
#include "../../../../../oatpp/core/src/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace url { namespace mapping {
  
template<class Param, class ReturnType>
class Subscriber {
public:
  typedef oatpp::async::Action Action;
  typedef Action (oatpp::async::AbstractCoroutine::*AsyncCallback)(const ReturnType&);
public:
  virtual ReturnType processUrl(const Param& param) = 0;
  virtual Action processUrlAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                 AsyncCallback callback,
                                 const Param& param) = 0;
};
  
}}}}

#endif /* oatpp_network_url_Subscriber_hpp */
