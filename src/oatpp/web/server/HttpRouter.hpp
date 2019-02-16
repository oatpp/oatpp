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

#ifndef oatpp_web_server_HttpRouter_hpp
#define oatpp_web_server_HttpRouter_hpp

#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"

#include "oatpp/web/url/mapping/Router.hpp"

namespace oatpp { namespace web { namespace server {
  
class HttpRouter : public oatpp::base::Controllable {
private:
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
public:
  typedef oatpp::web::url::mapping::Router<
    std::shared_ptr<oatpp::web::protocol::http::incoming::Request>,
    std::shared_ptr<oatpp::web::protocol::http::outgoing::Response>
  > BranchRouter;
  typedef BranchRouter::UrlSubscriber Subscriber;
  typedef std::unordered_map<StringKeyLabel, std::shared_ptr<BranchRouter>> BranchMap;
protected:
  BranchMap m_branchMap;
protected:
  
  const std::shared_ptr<BranchRouter>& getBranch(const StringKeyLabel& name){
    auto it = m_branchMap.find(name);
    if(it == m_branchMap.end()){
      m_branchMap[name] = BranchRouter::createShared();
    }
    return m_branchMap[name];
  }
  
public:
  HttpRouter() {}
public:
  
  static std::shared_ptr<HttpRouter> createShared() {
    return std::make_shared<HttpRouter>();
  }
  
  void addSubscriber(const oatpp::String& method,
                     const oatpp::String& urlPattern,
                     const std::shared_ptr<Subscriber>& subscriber) {
    getBranch(method)->addSubscriber(urlPattern, subscriber);
  }
  
  BranchRouter::Route getRoute(const StringKeyLabel& method,
                               const StringKeyLabel& url){
    auto it = m_branchMap.find(method);
    if(it != m_branchMap.end()) {
      return m_branchMap[method]->getRoute(url);
    }
    return BranchRouter::Route();
  }
  
  void logRouterMappings() {
    for(auto it : m_branchMap) {
      it.second->logRouterMappings();
    }
  }
  
};
  
}}}

#endif /* oatpp_web_server_HttpRouter_hpp */
