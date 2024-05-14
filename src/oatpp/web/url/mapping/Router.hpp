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

#include "oatpp/Types.hpp"

#include <utility>
#include <list>

namespace oatpp { namespace web { namespace url { namespace mapping {

/**
 * Class responsible to map "Path" to "Route" by "Path-Pattern".
 * @tparam Endpoint - endpoint of the route.
 */
template<typename Endpoint>
class Router : public base::Countable {
private:

  /**
   * Pair &id:oatpp::web::url::mapping::Pattern; to Endpoint.
   */
  typedef std::pair<std::shared_ptr<Pattern>, Endpoint> Pair;

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
    bool m_valid;
    Endpoint m_endpoint;
    Pattern::MatchMap m_matchMap;
  public:

    /**
     * Default constructor.
     */
    Route()
      : m_valid(false)
    {}

    /**
     * Constructor.
     * @param pEndpoint - route endpoint.
     * @param pMatchMap - Match map of resolved path containing resolved path variables.
     */
    Route(const Endpoint& endpoint, Pattern::MatchMap&& matchMap)
      : m_valid(true)
      , m_endpoint(endpoint)
      , m_matchMap(matchMap)
    {}

    /**
     * Get Endpoint.
     */
    const Endpoint& getEndpoint() {
      return m_endpoint;
    }

    /**
     * Match map of resolved path containing resolved path variables.
     */
    const Pattern::MatchMap& getMatchMap() {
      return m_matchMap;
    }

    /**
     * Check if route is valid.
     * @return
     */
    bool isValid() {
      return m_valid;
    }
    
    explicit operator bool() const {
      return m_valid;
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
  void route(const oatpp::String& pathPattern, const Endpoint& endpoint) {
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
        return Route(pair.second, std::move(matchMap));
      }
    }

    return Route();
  }
  
  void logRouterMappings(const oatpp::data::share::StringKeyLabel &branch) {

    for(auto& pair : m_endpointsByPattern) {
      auto mapping = pair.first->toString();
      OATPP_LOGd("Router", "url '{} {}' -> mapped", reinterpret_cast<const char*>(branch.getData()), mapping)
    }

  }
  
};
  
}}}}

#endif /* oatpp_web_url_mapping_Router_hpp */
