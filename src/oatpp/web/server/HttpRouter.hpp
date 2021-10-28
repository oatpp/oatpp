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

#include "./HttpRequestHandler.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/server/api/Endpoint.hpp"
#include "oatpp/web/url/mapping/Router.hpp"

namespace oatpp { namespace web { namespace server {

/**
 * HttpRouter is responsible for routing http requests by method and path-pattern.
 */
template<typename RouterEndpoint>
class HttpRouterTemplate : public oatpp::base::Countable {
private:
  /**
   * Convenience typedef for &id:oatpp::data::share::StringKeyLabel;.
   */
  typedef data::share::StringKeyLabel StringKeyLabel;
public:

  /**
   * &id:oatpp::web::url::mapping::Router;
   */
  typedef web::url::mapping::Router<RouterEndpoint> BranchRouter;

  /**
   * Http method to &l:HttpRouter::BranchRouter; map.
   * Meaning that for each http method like ["GET", "POST", ...] there is a separate &l:HttpRouter::BranchRouter;.
   */
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

  /**
   * Default Constructor.
   */
  HttpRouterTemplate() = default;

  /**
   * Create shared HttpRouter.
   * @return - `std::shared_ptr` to HttpRouter.
   */
  static std::shared_ptr<HttpRouterTemplate> createShared() {
    return std::make_shared<HttpRouterTemplate>();
  }

  /**
   * Route URL to Endpoint by method, and pathPattern.
   * @param method - http method like ["GET", "POST", etc.].
   * @param pathPattern - url path pattern. ex.: `"/path/to/resource/with/{param1}/{param2}"`.
   * @param endpoint - router endpoint.
   */
  void route(const oatpp::String& method, const oatpp::String& pathPattern, const RouterEndpoint& endpoint) {
    getBranch(method)->route(pathPattern, endpoint);
  }

  /**
   * Resolve http method and path to &id:oatpp::web::url::mapping::Router::Route;
   * @param method - http method like ["GET", "POST", etc.].
   * @param url - url path. "Path" part of url only.
   * @return - &id:oatpp::web::url::mapping::Router::Route;.
   */
  typename BranchRouter::Route getRoute(const StringKeyLabel& method, const StringKeyLabel& path){
    auto it = m_branchMap.find(method);
    if(it != m_branchMap.end()) {
      return m_branchMap[method]->getRoute(path);
    }
    return typename BranchRouter::Route();
  }

  /**
   * Print out all router mapping.
   */
  void logRouterMappings() {
    for(auto it : m_branchMap) {
      it.second->logRouterMappings(it.first);
    }
  }
  
};

/**
 * Default HttpRouter.
 */
class HttpRouter : public HttpRouterTemplate<std::shared_ptr<HttpRequestHandler>> {
private:
  std::list<std::shared_ptr<server::api::ApiController>> m_controllers;
public:

  /**
   * Create shared HttpRouter.
   * @return
   */
  static std::shared_ptr<HttpRouter> createShared();

  using HttpRouterTemplate::route;
  void route(const std::shared_ptr<server::api::Endpoint>& endpoint);
  void route(const server::api::Endpoints& endpoints);

  /**
   * Add controller and route its' endpoints.
   * @param controller
   * @return - `std::shared_ptr` to the controller added.
   */
  std::shared_ptr<server::api::ApiController> addController(const std::shared_ptr<server::api::ApiController>& controller);

};
  
}}}

#endif /* oatpp_web_server_HttpRouter_hpp */
