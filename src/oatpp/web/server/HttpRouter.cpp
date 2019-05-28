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

#include "HttpRouter.hpp"

namespace oatpp { namespace web { namespace server {

HttpRouter::HttpRouter()
{}

const std::shared_ptr<HttpRouter::BranchRouter>& HttpRouter::getBranch(const StringKeyLabel& name){
  auto it = m_branchMap.find(name);
  if(it == m_branchMap.end()){
    m_branchMap[name] = BranchRouter::createShared();
  }
  return m_branchMap[name];
}

std::shared_ptr<HttpRouter> HttpRouter::createShared() {
  return std::make_shared<HttpRouter>();
}

void HttpRouter::route(const oatpp::String& method,
                               const oatpp::String& pathPattern,
                               const std::shared_ptr<HttpRequestHandler>& handler) {
  getBranch(method)->route(pathPattern, handler);
}

HttpRouter::BranchRouter::Route HttpRouter::getRoute(const StringKeyLabel& method, const StringKeyLabel& path){
  auto it = m_branchMap.find(method);
  if(it != m_branchMap.end()) {
    return m_branchMap[method]->getRoute(path);
  }
  return BranchRouter::Route();
}

void HttpRouter::logRouterMappings() {
  for(auto it : m_branchMap) {
    it.second->logRouterMappings();
  }
}

}}}