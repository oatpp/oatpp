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
#include "oatpp/web/url/mapping/Router.hpp"

namespace oatpp { namespace web { namespace server {

/**
 * HttpRouter is responsible for routing http requests by method and path-pattern.
 */
class HttpRouter : public oatpp::base::Countable {
private:
  /**
   * Convenience typedef for &id:oatpp::data::share::StringKeyLabel;.
   */
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
public:

  /**
   * &id:oatpp::web::url::mapping::Router; of &id:oatpp::web::server::HttpRequestHandler;.
   */
  typedef oatpp::web::url::mapping::Router<HttpRequestHandler> BranchRouter;

  /**
   * Http method to &l:HttpRouter::BranchRouter; map.
   * Meaning that for each http method like ["GET", "POST", ...] there is a separate &l:HttpRouter::BranchRouter;.
   */
  typedef std::unordered_map<StringKeyLabel, std::shared_ptr<BranchRouter>> BranchMap;
protected:
  BranchMap m_branchMap;
protected:
  
  const std::shared_ptr<BranchRouter>& getBranch(const StringKeyLabel& name);
  
public:

  /**
   * Constructor.
   */
  HttpRouter();

  /**
   * Create shared HttpRouter.
   * @return - `std::shared_ptr` to HttpRouter.
   */
  static std::shared_ptr<HttpRouter> createShared();

  /**
   * Route URL to Handler by method, and pathPattern.
   * @param method - http method like ["GET", "POST", etc.].
   * @param pathPattern - url path pattern. ex.: `"/path/to/resource/with/{param1}/{param2}"`.
   * @param handler - &id:oatpp::web::server::HttpRequestHandler;.
   */
  void route(const oatpp::String& method, const oatpp::String& pathPattern, const std::shared_ptr<HttpRequestHandler>& handler);

  /**
   * Resolve http method and path to &id:oatpp::web::url::mapping::Router::Route;
   * @param method - http method like ["GET", "POST", etc.].
   * @param url - url path. "Path" part of url only.
   * @return - &id:oatpp::web::url::mapping::Router::Route;.
   */
  BranchRouter::Route getRoute(const StringKeyLabel& method, const StringKeyLabel& path);

  /**
   * Print out all router mapping.
   */
  void logRouterMappings();
  
};
  
}}}

#endif /* oatpp_web_server_HttpRouter_hpp */
