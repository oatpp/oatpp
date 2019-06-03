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

#ifndef oatpp_web_server_rest_Controller_hpp
#define oatpp_web_server_rest_Controller_hpp

#include "./Endpoint.hpp"

#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/url/mapping/Router.hpp"
#include "oatpp/web/protocol/http/incoming/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

#include <unordered_map>

namespace oatpp { namespace web { namespace server { namespace api {

/**
 * Class responsible for implementation and management of endpoints.<br>
 * For details see [ApiController](https://oatpp.io/docs/components/api-controller/).
 */
class ApiController : public oatpp::base::Countable {
protected:
  typedef ApiController __ControllerType;
public:
  /**
   * Convenience typedef for &id:oatpp::web::server::HttpRouter;.
   */
  typedef oatpp::web::server::HttpRouter Router;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::outgoing::ResponseFactory;.
   */
  typedef oatpp::web::protocol::http::outgoing::ResponseFactory ResponseFactory;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::incoming::Request;.
   */
  typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::outgoing::Request;.
   */
  typedef oatpp::web::protocol::http::outgoing::Request OutgoingRequest;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::incoming::Response;.
   */
  typedef oatpp::web::protocol::http::incoming::Response IncomingResponse;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::outgoing::Response;.
   */
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Status;.
   */
  typedef oatpp::web::protocol::http::Status Status;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::Header;.
   */
  typedef oatpp::web::protocol::http::Header Header;

  /**
   * Convenience typedef for &id:oatpp::web::protocol::http::QueryParams;.
   */
  typedef oatpp::web::protocol::http::QueryParams QueryParams;

  /**
   * Convenience typedef for &id:oatpp::web::server::api::Endpoint;.
   */
  typedef oatpp::web::server::api::Endpoint Endpoint;

  /**
   * Convenience typedef for list of &id:oatpp::web::server::api::Endpoint;.
   */
  typedef oatpp::collection::LinkedList<std::shared_ptr<Endpoint>> Endpoints;
  
public:

  /**
   * Convenience typedef for &id:oatpp::data::mapping::ObjectMapper;.
   */
  typedef oatpp::data::mapping::ObjectMapper ObjectMapper;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::String;.
   */
  typedef oatpp::data::mapping::type::String String;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Int32;.
   */
  typedef oatpp::data::mapping::type::Int32 Int32;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Int64;.
   */
  typedef oatpp::data::mapping::type::Int64 Int64;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Float32;.
   */
  typedef oatpp::data::mapping::type::Float32 Float32;

  /**
   * Convenience typedef for &id:atpp::data::mapping::type::Float64;.
   */
  typedef oatpp::data::mapping::type::Float64 Float64;

  /**
   * Convenience typedef for &id:oatpp::data::mapping::type::Boolean;.
   */
  typedef oatpp::data::mapping::type::Boolean Boolean;
  template <class T>
  using List = oatpp::data::mapping::type::List<T>;
  template <class Value>
  using Fields = oatpp::data::mapping::type::ListMap<String, Value>;

protected:
  
  /*
   * Endpoint Coroutine base class
   */
  template<class CoroutineT, class ControllerT>
  class HandlerCoroutine : public oatpp::async::CoroutineWithResult<CoroutineT, const std::shared_ptr<OutgoingResponse>&> {
  public:
    
    HandlerCoroutine(ControllerT* pController,
                     const std::shared_ptr<protocol::http::incoming::Request>& pRequest)
      : controller(pController)
      , request(pRequest)
    {}
    
    const ControllerT* controller;
    std::shared_ptr<protocol::http::incoming::Request> request;
    
  };
  
  /*
   * Handler which subscribes to specific URL in Router and delegates calls endpoints 
   */
  template<class T>
  class Handler : public oatpp::web::server::HttpRequestHandler {
  public:
    typedef std::shared_ptr<OutgoingResponse> (T::*Method)(const std::shared_ptr<protocol::http::incoming::Request>&);
    typedef oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
            (T::*MethodAsync)(const std::shared_ptr<protocol::http::incoming::Request>&);
  private:
    T* m_controller;
    Method m_method;
    MethodAsync m_methodAsync;
  public:
    Handler(T* controller, Method method, MethodAsync methodAsync)
      : m_controller(controller)
      , m_method(method)
      , m_methodAsync(methodAsync)
    {}
  public:
    
    static std::shared_ptr<Handler> createShared(T* controller, Method method, MethodAsync methodAsync){
      return std::make_shared<Handler>(controller, method, methodAsync);
    }
    
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<protocol::http::incoming::Request>& request) override {
      if(m_method != nullptr) {
        return (m_controller->*m_method)(request);
      } else {
        return m_controller->handleError(Status::CODE_500, "Using simple model for Async endpoint");
      }
    }
    
    oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
    handleAsync(const std::shared_ptr<protocol::http::incoming::Request>& request) override {
      if(m_methodAsync != nullptr) {
        return (m_controller->*m_methodAsync)(request);
      }
      throw oatpp::web::protocol::http::HttpError(Status::CODE_500, "Using Async model for non async enpoint");
    }
    
  };
  
protected:
  std::shared_ptr<Endpoints> m_endpoints;
  std::shared_ptr<handler::ErrorHandler> m_errorHandler;
  std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_defaultObjectMapper;
  std::unordered_map<std::string, std::shared_ptr<Endpoint::Info>> m_endpointInfo;
public:
  ApiController(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& defaultObjectMapper)
    : m_endpoints(Endpoints::createShared())
    , m_errorHandler(nullptr)
    , m_defaultObjectMapper(defaultObjectMapper)
  {}
public:
  
  template<class T>
  static std::shared_ptr<Endpoint> createEndpoint(const std::shared_ptr<Endpoints>& endpoints,
                                                  T* controller,
                                                  typename Handler<T>::Method method,
                                                  typename Handler<T>::MethodAsync methodAsync,
                                                  const std::shared_ptr<Endpoint::Info>& info){
    auto handler = Handler<T>::createShared(controller, method, methodAsync);
    auto endpoint = Endpoint::createShared(handler, info);
    endpoints->pushBack(endpoint);
    return endpoint;
  }
  
  /**
   * Subscribes all created endpoint-handlers to corresponding URLs in Router
   */
  void addEndpointsToRouter(const std::shared_ptr<Router>& router);
  
  /**
   * Get list of Endpoints created via ENDPOINT macro
   */
  std::shared_ptr<Endpoints> getEndpoints();
  
  /**
   * Set endpoint info by endpoint name. (Endpoint name is the 'NAME' parameter of the ENDPOINT macro)
   * Info should be set before call to addEndpointsToRouter();
   */
  void setEndpointInfo(const std::string& endpointName, const std::shared_ptr<Endpoint::Info>& info);
  
  /**
   * Get endpoint info by endpoint name. (Endpoint name is the 'NAME' parameter of the ENDPOINT macro)
   */
  std::shared_ptr<Endpoint::Info> getEndpointInfo(const std::string& endpointName);
  
  /**
   * [under discussion]
   * Do not use it directly. This method is under discussion.
   * Currently return Response created by ErrorHandler or throws HttpError if ErrorHandler is null
   */
  std::shared_ptr<OutgoingResponse> handleError(const Status& status, const oatpp::String& message) const;
  
  /**
   * [under discussion]
   * Set error handler to handle calls to handleError
   */
  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler);
  
  const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& getDefaultObjectMapper() const;
  
  // Helper methods
  
  std::shared_ptr<OutgoingResponse> createResponse(const Status& status,
                                                   const oatpp::String& str) const;
  
  std::shared_ptr<OutgoingResponse> createResponse(const Status& status,
                                                   const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& chunkedBuffer) const;
  
  std::shared_ptr<OutgoingResponse> createDtoResponse(const Status& status,
                                                      const oatpp::data::mapping::type::AbstractObjectWrapper& dto,
                                                      const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const;
  
  std::shared_ptr<OutgoingResponse> createDtoResponse(const Status& status,
                                                      const oatpp::data::mapping::type::AbstractObjectWrapper& dto) const;
  
};

}}}}

#endif /* oatpp_web_server_rest_Controller_hpp */
