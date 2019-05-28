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

#include "./Pattern.hpp"

#include "oatpp/core/Types.hpp"

#include <utility>
#include <list>

namespace oatpp { namespace web { namespace url { namespace mapping {

/**
 * Class responsible to map "Path" to "Route" by "Path-Pattern".
 * @tparam Endpoint - endpoint of the route.
 */
template<class Endpoint>
class Router : public base::Countable {
private:

  /**
   * Pair &id:oatpp::web::url::mapping::Pattern; to Endpoint.
   */
  typedef std::pair<std::shared_ptr<Pattern>, std::shared_ptr<Endpoint>> Pair;

  /**
   * Convenience typedef &id:oatpp::data::share::StringKeyLabel;.
   */
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
public:

  /**
   * Resolved "Route" for "path-pattern"
   */
  class Route {
  private:
    Endpoint* m_endpoint;
  public:

    /**
     * Default constructor.
     */
    Route()
      : m_endpoint(nullptr)
    {}

    /**
     * Constructor.
     * @param pEndpoint - route endpoint.
     * @param pMatchMap - Match map of resolved path containing resolved path variables.
     */
    Route(Endpoint* endpoint, const Pattern::MatchMap& pMatchMap)
      : m_endpoint(endpoint)
      , matchMap(pMatchMap)
    {}

    /**
     * Get endpoint of the route.
     */
    Endpoint* getEndpoint() {
      return m_endpoint;
    }

    /**
     * Match map of resolved path containing resolved path variables.
     */
    Pattern::MatchMap matchMap;
    
    explicit operator bool() const {
      return m_endpoint != nullptr;
    }
    
  };
  
private:
  std::list<Pair> m_endpointsByPattern;
public:
  
  static std::shared_ptr<Router> createShared(){
    return std::make_shared<Router>();
  }

  /**
   * Add `path-pattern` to `endpoint` mapping.
   * @param pathPattern - path pattern for endpoint.
   * @param endpoint - route endpoint.
   */
  void route(const oatpp::String& pathPattern, const std::shared_ptr<Endpoint>& endpoint) {
    auto pattern = Pattern::parse(pathPattern);
    m_endpointsByPattern.push_back({pattern, endpoint});
  }

  /**
   * Resolve path to corresponding endpoint.
   * @param path
   * @return - &id:Router::Route;.
   */
  Route getRoute(const StringKeyLabel& path){

    for(auto& pair : m_endpointsByPattern) {
      Pattern::MatchMap matchMap;
      if(pair.first->match(path, matchMap)) {
        return Route(pair.second.get(), matchMap);
      }
    }

    return Route();
  }
  
  void logRouterMappings() {

    for(auto& pair : m_endpointsByPattern) {
      auto mapping = pair.first->toString();
      OATPP_LOGD("Router", "url '%s' -> mapped", (const char*) mapping->getData());
    }

  }
  
};
  
}}}}

#endif /* oatpp_web_url_mapping_Router_hpp */
