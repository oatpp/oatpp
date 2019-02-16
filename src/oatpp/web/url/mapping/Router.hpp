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

#ifndef oatpp_web_url_mapping_Router_hpp
#define oatpp_web_url_mapping_Router_hpp

#include "./Subscriber.hpp"
#include "./Pattern.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

#include "oatpp/core/Types.hpp"


#include "oatpp/core/base/Controllable.hpp"
#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace web { namespace url { namespace mapping {
  
template<class Param, class ReturnType>
class Router : public base::Controllable{
public:
  typedef Subscriber<Param, ReturnType> UrlSubscriber;
private:
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
public:
  
  class Route {
  private:
    UrlSubscriber* m_subscriber;
  public:
    
    Route()
      : m_subscriber(nullptr)
    {}
    
    Route(UrlSubscriber* subscriber, const Pattern::MatchMap& pMatchMap)
      : m_subscriber(subscriber)
      , matchMap(pMatchMap)
    {}
    
    ReturnType processUrl(const Param& param) const {
      return m_subscriber->processUrl(param);
    }
    
    oatpp::async::Action processUrlAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                         typename UrlSubscriber::AsyncCallback callback,
                                         const Param& param) const {
      return m_subscriber->processUrlAsync(parentCoroutine, callback, param);
    }
    
    explicit operator bool() const {
      return m_subscriber != nullptr;
    }
    
    Pattern::MatchMap matchMap;
    
  };

public:
  
  class Pair : public base::Controllable{
  public:
    Pair(const std::shared_ptr<Pattern>& pPattern, const std::shared_ptr<UrlSubscriber>& pSubscriber)
      : pattern(pPattern)
      , subscriber(pSubscriber)
    {}
  public:
    
    static std::shared_ptr<Pair> createShared(const std::shared_ptr<Pattern>& pattern, const std::shared_ptr<UrlSubscriber>& subscriber){
      return std::make_shared<Pair>(pattern, subscriber);
    }
    
    const std::shared_ptr<Pattern> pattern;
    const std::shared_ptr<UrlSubscriber> subscriber;
    
  };
  
private:
  std::shared_ptr<oatpp::collection::LinkedList<std::shared_ptr<Pair>>> m_subscribers;
public:
  Router()
    : m_subscribers(oatpp::collection::LinkedList<std::shared_ptr<Pair>>::createShared())
  {}
public:
  
  static std::shared_ptr<Router> createShared(){
    return std::make_shared<Router>();
  }
  
  void addSubscriber(const oatpp::String& urlPattern,
                     const std::shared_ptr<UrlSubscriber>& subscriber){
    auto pattern = Pattern::parse(urlPattern);
    auto pair = Pair::createShared(pattern, subscriber);
    m_subscribers->pushBack(pair);
  }
  
  Route getRoute(const StringKeyLabel& url){
    auto curr = m_subscribers->getFirstNode();
    while(curr != nullptr) {
      const std::shared_ptr<Pair>& pair = curr->getData();
      curr = curr->getNext();
      Pattern::MatchMap match;
      if(pair->pattern->match(url, match)) {
        return Route(pair->subscriber.get(), match);
      }
    }
    return Route();
  }
  
  void logRouterMappings() {
    auto curr = m_subscribers->getFirstNode();
    while(curr != nullptr){
      const std::shared_ptr<Pair>& pair = curr->getData();
      curr = curr->getNext();
      auto mapping = pair->pattern->toString();
      OATPP_LOGD("Router", "url '%s' -> mapped", (const char*) mapping->getData());
    }
  }
  
};
  
}}}}

#endif /* oatpp_web_url_mapping_Router_hpp */
