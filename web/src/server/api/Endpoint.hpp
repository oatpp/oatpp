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

#ifndef oatpp_web_server_rest_Endpoint_hpp
#define oatpp_web_server_rest_Endpoint_hpp

#include "../../url/mapping/Router.hpp"

#include "../../protocol/http/incoming/Request.hpp"
#include "../../protocol/http/outgoing/Response.hpp"

#include <list>

namespace oatpp { namespace web { namespace server { namespace api {
  
class Endpoint : public oatpp::base::Controllable {
public:
  typedef oatpp::web::url::mapping::Subscriber<
    std::shared_ptr<protocol::http::incoming::Request>,
    std::shared_ptr<protocol::http::outgoing::Response>
  > RequestHandler;
public:
  
  class Info : public oatpp::base::Controllable {
  public:
    
    class Param {
    public:
      
      Param()
        : name(nullptr)
        , type(nullptr)
      {}
      
      Param(const char* pName,
            oatpp::data::mapping::type::Type* pType)
        : name(pName)
        , type(pType)
      {}
      
      const char* name;
      oatpp::data::mapping::type::Type* type;
      
    };
  public:
    Info()
    {}
  public:
    static std::shared_ptr<Info> createShared(){
      return std::shared_ptr<Info>(new Info());
    }
    
    base::String::PtrWrapper name;
    base::String::PtrWrapper path;
    base::String::PtrWrapper method;
    
    Param body;
    
    std::list<Param> headers;
    std::list<Param> pathParams;
    std::list<Param> queryParams;
    
    std::list<Param> returnTypes;
    
    oatpp::base::PtrWrapper<oatpp::base::String> toString();
    
  };
public:
  Endpoint(const std::shared_ptr<RequestHandler>& pHandler,
           const std::shared_ptr<Info>& pInfo)
    : handler(pHandler)
    , info(pInfo)
  {}
public:
  
  static std::shared_ptr<Endpoint> createShared(const std::shared_ptr<RequestHandler>& handler,
                                          const std::shared_ptr<Info>& info){
    return std::shared_ptr<Endpoint>(new Endpoint(handler, info));
  }
  
  const std::shared_ptr<RequestHandler> handler;
  const std::shared_ptr<Info> info;
  
};
  
}}}}

#endif /* oatpp_web_server_rest_Endpoint_hpp */
