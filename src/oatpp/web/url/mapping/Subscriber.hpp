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

#ifndef oatpp_web_mapping_url_Subscriber_hpp
#define oatpp_web_mapping_url_Subscriber_hpp


#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace url { namespace mapping {

/**
 * Abstract subscriber which can subscribe to incoming events from &id:oatpp::web::url::mapping::Router; and process those events.
 * @tparam Event - incoming event type.
 * @tparam Result - result of event processing.
 */
template<class Event, class Result>
class Subscriber {
public:
  /**
   * Convenience typedef for &id:oatpp::async::Action;
   */
  typedef oatpp::async::Action Action;
  typedef Action (oatpp::async::AbstractCoroutine::*AsyncCallback)(const Result&);
public:
  /**
   * Process event.
   * @param Event - some incoming data.
   * @return - some outgoing data.
   */
  virtual Result processEvent(const Event& event) = 0;

  /**
   * Process event in asynchronous manner.
   * @param parentCoroutine - caller coroutine as &id:oatpp::async::AbstractCoroutine;*.
   * @param callback - pointer to callback function.
   * @param event - some incoming data.
   * @return - &id:oatpp::async::Action;.
   */
  virtual Action processEventAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                   AsyncCallback callback,
                                   const Event& event) = 0;
};
  
}}}}

#endif /* oatpp_web_mapping_url_Subscriber_hpp */
