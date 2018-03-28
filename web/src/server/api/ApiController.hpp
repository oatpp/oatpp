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

#ifndef oatpp_web_server_rest_Controller_hpp
#define oatpp_web_server_rest_Controller_hpp

#include "./Endpoint.hpp"
#include "../handler/ErrorHandler.hpp"

#include "../HttpConnectionHandler.hpp"

#include "../../url/mapping/Router.hpp"
#include "../../protocol/http/outgoing/ResponseFactory.hpp"
#include "../../../../../oatpp/core/src/collection/LinkedList.hpp"

#include <unordered_map>

namespace oatpp { namespace web { namespace server { namespace api {
  
class ApiController : public oatpp::base::Controllable {
protected:
  typedef ApiController __ControllerType;
public:
  typedef oatpp::web::server::HttpRouter Router;
  typedef oatpp::web::protocol::http::outgoing::ResponseFactory OutgoingResponseFactory;
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
  typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;
  typedef oatpp::web::protocol::http::Status Status;
  typedef oatpp::web::protocol::http::Header Header;
  typedef oatpp::web::server::api::Endpoint Endpoint;
  typedef oatpp::collection::LinkedList<std::shared_ptr<Endpoint>> Endpoints;
  
public:
  typedef oatpp::data::mapping::ObjectMapper ObjectMapper;
  typedef oatpp::data::mapping::type::StringPtrWrapper String;
  typedef oatpp::data::mapping::type::Int32::PtrWrapper Int32;
  typedef oatpp::data::mapping::type::Int64::PtrWrapper Int64;
  typedef oatpp::data::mapping::type::Float32::PtrWrapper Float32;
  typedef oatpp::data::mapping::type::Float64::PtrWrapper Float64;
  typedef oatpp::data::mapping::type::Boolean::PtrWrapper Boolean;
  
protected:
  typedef oatpp::async::Action (oatpp::async::AbstractCoroutine::*AsyncCallback)(const std::shared_ptr<OutgoingResponse>&);
protected:
  
  template<class CoroutineT, class ControllerT>
  class HandlerCoroutine : public oatpp::async::CoroutineWithResult<CoroutineT, std::shared_ptr<OutgoingResponse>> {
  public:
    
    HandlerCoroutine(ControllerT* pController,
                     const std::shared_ptr<protocol::http::incoming::Request>& pRequest)
      : controller(pController)
      , request(pRequest)
    {}
    
    const ControllerT* controller;
    std::shared_ptr<protocol::http::incoming::Request> request;
    
  };
  
  template<class T>
  class Handler :
  public oatpp::base::Controllable,
  public oatpp::web::url::mapping::Subscriber<
    std::shared_ptr<protocol::http::incoming::Request>,
    std::shared_ptr<protocol::http::outgoing::Response>
  > {
  public:
    typedef std::shared_ptr<OutgoingResponse> (T::*Method)(const std::shared_ptr<protocol::http::incoming::Request>&);
    typedef Action (T::*MethodAsync)(oatpp::async::AbstractCoroutine*,
                                     AsyncCallback callback,
                                     const std::shared_ptr<protocol::http::incoming::Request>&);
  private:
    T* m_controller;
    Method m_method;
    MethodAsync m_methodAsync;
  protected:
    Handler(T* controller, Method method, MethodAsync methodAsync)
      : m_controller(controller)
      , m_method(method)
      , m_methodAsync(methodAsync)
    {}
  public:
    
    static std::shared_ptr<Handler> createShared(T* controller, Method method, MethodAsync methodAsync){
      return std::shared_ptr<Handler>(new Handler(controller, method, methodAsync));
    }
    
    std::shared_ptr<OutgoingResponse> processUrl(const std::shared_ptr<protocol::http::incoming::Request>& request) override {
      return (m_controller->*m_method)(request);
    }
    
    Action processUrlAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                           AsyncCallback callback,
                           const std::shared_ptr<protocol::http::incoming::Request>& request) override {
      if(m_methodAsync != nullptr) {
        return (m_controller->*m_methodAsync)(parentCoroutine, callback, request);
      } else {
        return parentCoroutine->callWithParams(reinterpret_cast<oatpp::async::AbstractCoroutine::FunctionPtr>(callback),
                                               m_controller->handleError(Status::CODE_500,
                                                                         "Using Async model for non async enpoint"));
      }
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
    , m_errorHandler(handler::DefaultErrorHandler::createShared())
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
  
  void addEndpointsToRouter(const std::shared_ptr<Router>& router){
    auto node = m_endpoints->getFirstNode();
    while (node != nullptr) {
      auto endpoint = node->getData();
      router->addSubscriber(endpoint->info->method, endpoint->info->path, endpoint->handler);
      node = node->getNext();
    }
  }
  
  std::shared_ptr<Endpoints> getEndpoints() {
    return m_endpoints;
  }
  
  void setEndpointInfo(const std::string& endpointName, const std::shared_ptr<Endpoint::Info>& info){
    m_endpointInfo[endpointName] = info;
  }
  
  std::shared_ptr<Endpoint::Info> getEndpointInfo(const std::string& endpointName) {
    return m_endpointInfo[endpointName];
  }
  
  std::shared_ptr<OutgoingResponse> handleError(const Status& status, const base::String::PtrWrapper& message) {
    return m_errorHandler->handleError(status, message);
  }
  
  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
    m_errorHandler = errorHandler;
    if(!m_errorHandler) {
      m_errorHandler = handler::DefaultErrorHandler::createShared();
    }
  }
  
  const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& getDefaultObjectMapper() const {
    return m_defaultObjectMapper;
  }
  
  // Helper methods
  
  std::shared_ptr<OutgoingResponse> createResponse(const Status& status,
                                                   const oatpp::base::String::PtrWrapper& str) const {
    return OutgoingResponseFactory::createShared(status, str);
  }
  
  std::shared_ptr<OutgoingResponse> createResponse(const Status& status,
                                                   const std::shared_ptr<oatpp::data::stream::ChunkedBuffer>& chunkedBuffer) const {
    return OutgoingResponseFactory::createShared(status, chunkedBuffer);
  }
  
  std::shared_ptr<OutgoingResponse> createDtoResponse(const Status& status,
                                                      const oatpp::data::mapping::type::AbstractPtrWrapper& dto,
                                                      const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const {
    return OutgoingResponseFactory::createShared(status, dto, objectMapper.get());
  }
  
  std::shared_ptr<OutgoingResponse> createDtoResponse(const Status& status,
                                                      const oatpp::data::mapping::type::AbstractPtrWrapper& dto) const {
    return OutgoingResponseFactory::createShared(status, dto, m_defaultObjectMapper.get());
  }
  
};

}}}}

#endif /* oatpp_web_server_rest_Controller_hpp */
