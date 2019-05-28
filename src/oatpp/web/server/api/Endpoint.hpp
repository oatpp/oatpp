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

#ifndef oatpp_web_server_rest_Endpoint_hpp
#define oatpp_web_server_rest_Endpoint_hpp

#include "oatpp/web/server/HttpRequestHandler.hpp"

#include <list>
#include <unordered_map>

namespace oatpp { namespace web { namespace server { namespace api {

/**
 * Endpoint - class which holds information about endpoint.
 * It holds API documentation info, and pointer to RequestHandler
 */
class Endpoint : public oatpp::base::Countable {
public:

  /**
   * Convenience typedef for &id:oatpp::web::server::HttpRequestHandler;.
   */
  typedef oatpp::web::server::HttpRequestHandler RequestHandler;
public:

  /**
   * Info holds API documentation information about endpoint
   */
  class Info : public oatpp::base::Countable {
  public:

    /**
     * Param holds info about parameter
     */
    struct Param {
      
      Param();
      Param(const oatpp::String& pName, oatpp::data::mapping::type::Type* pType);

      oatpp::String name;
      oatpp::data::mapping::type::Type* type;

      oatpp::String description;
      oatpp::Boolean required = true;
      oatpp::Boolean deprecated = false;
      oatpp::Boolean allowEmptyValue;
      
    };

    /**
     * Parameters container
     */
    class Params {
    private:
      std::list<oatpp::String> m_order;
      std::unordered_map<oatpp::String, Param> m_params;
    public:

      const std::list<oatpp::String>& getOrder() const;

      /**
       * Add parameter name to list order
       * @param name
       * @return new or existing parameter
       */
      Param& add(const oatpp::String& name, oatpp::data::mapping::type::Type* type);

      /**
       * Add parameter name to list order
       * @tparam T
       * @param name
       * @return new or existing parameter
       */
      template<class T>
      Param& add(const oatpp::String& name) {
        return add(name, T::Class::getType());
      }

      /**
       * Get or add param by name
       * @param name
       * @return
       */
      Param& operator [](const oatpp::String& name);

    };

    /**
     * Info about content type and schema
     */
    struct ContentTypeAndSchema {
      oatpp::String contentType;
      oatpp::data::mapping::type::Type* schema;
    };
    
  public:

    Info();

    static std::shared_ptr<Info> createShared();
    
    oatpp::String name;
    oatpp::String summary;
    oatpp::String description;
    oatpp::String path;
    oatpp::String method;
    
    Param body;
    oatpp::String bodyContentType;
    
    std::list<ContentTypeAndSchema> consumes;

    Params headers;
    Params pathParams;
    Params queryParams;
    
    /**
     *  ResponseCode to {ContentType, Type} mapping.
     *  Example responses[Status::CODE_200] = {"application/json", MyDto::ObjectWrapper::Class::getType()};
     */
    std::unordered_map<oatpp::web::protocol::http::Status, ContentTypeAndSchema> responses;
    
    oatpp::String toString();
    
    template<class T>
    void addConsumes(const oatpp::String& contentType) {
      consumes.push_back({contentType, T::Class::getType()});
    }
    
    template<class T>
    void addResponse(const oatpp::web::protocol::http::Status& status, const oatpp::String& contentType) {
      responses[status] = {contentType, T::Class::getType()};
    }
    
  };
public:

  Endpoint(const std::shared_ptr<RequestHandler>& pHandler, const std::shared_ptr<Info>& pInfo);
  
  static std::shared_ptr<Endpoint> createShared(const std::shared_ptr<RequestHandler>& handler,
                                                const std::shared_ptr<Info>& info);
  
  const std::shared_ptr<RequestHandler> handler;
  const std::shared_ptr<Info> info;
  
};
  
}}}}

#endif /* oatpp_web_server_rest_Endpoint_hpp */
