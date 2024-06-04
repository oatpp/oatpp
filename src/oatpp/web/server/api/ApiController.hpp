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

#ifndef oatpp_web_server_api_Controller_hpp
#define oatpp_web_server_api_Controller_hpp

#include "./Endpoint.hpp"

#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/web/server/HttpServerError.hpp"
#include "oatpp/web/protocol/http/incoming/Response.hpp"
#include "oatpp/web/protocol/http/outgoing/Request.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"

#include "oatpp/utils/Conversion.hpp"
#include "oatpp/base/Log.hpp"

#include <list>
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
   * Convenience typedef for &id:oatpp::web::server::HttpRequestHandler;.
   */
  typedef oatpp::web::server::HttpRequestHandler RequestHandler;

  /**
   * Convenience typedef for &id:oatpp::web::server::handler::AuthorizationHandler;.
   */
  typedef oatpp::web::server::handler::AuthorizationHandler AuthorizationHandler;
  
public:

  /**
   * Convenience typedef for &id:oatpp::data::mapping::ObjectMapper;.
   */
  typedef oatpp::data::mapping::ObjectMapper ObjectMapper;

  /**
   * Convenience typedef for &id:oatpp::data::type::String;.
   */
  typedef oatpp::String String;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int8;.
   */
  typedef oatpp::Int8 Int8;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt8;.
   */
  typedef oatpp::UInt8 UInt8;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int16;.
   */
  typedef oatpp::Int16 Int16;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt16;.
   */
  typedef oatpp::UInt16 UInt16;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int32;.
   */
  typedef oatpp::Int32 Int32;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt32;.
   */
  typedef oatpp::UInt32 UInt32;

  /**
   * Convenience typedef for &id:oatpp::data::type::Int64;.
   */
  typedef oatpp::Int64 Int64;

  /**
   * Convenience typedef for &id:oatpp::data::type::UInt64;.
   */
  typedef oatpp::UInt64 UInt64;

  /**
   * Convenience typedef for &id:oatpp::data::type::Float32;.
   */
  typedef oatpp::Float32 Float32;

  /**
   * Convenience typedef for &id:atpp::data::type::Float64;.
   */
  typedef oatpp::Float64 Float64;

  /**
   * Convenience typedef for &id:oatpp::data::type::Boolean;.
   */
  typedef oatpp::Boolean Boolean;

  /*
   * Convenience typedef for std::function<std::shared_ptr<Endpoint::Info>()>.
   */
  typedef std::function<std::shared_ptr<Endpoint::Info>()> EndpointInfoBuilder;

  template <class T>
  using Object = oatpp::Object<T>;

  template <class T>
  using List = oatpp::List<T>;

  template <class Value>
  using Fields = oatpp::Fields<Value>;

  template <class T>
  using Enum = oatpp::data::type::Enum<T>;

protected:
  
  /*
   * Endpoint Coroutine base class
   */
  template<class CoroutineT, class ControllerT>
  class HandlerCoroutine : public oatpp::async::CoroutineWithResult<CoroutineT, const std::shared_ptr<OutgoingResponse>&> {
  public:
    
    HandlerCoroutine(ControllerT* pController, const std::shared_ptr<IncomingRequest>& pRequest)
      : controller(pController)
      , request(pRequest)
    {}
    
    ControllerT* const controller;
    std::shared_ptr<IncomingRequest> request;
    
  };
  
  /*
   * Handler which subscribes to specific URL in Router and delegates calls endpoints 
   */
  template<class T>
  class Handler : public RequestHandler {
  public:
    typedef std::shared_ptr<OutgoingResponse> (T::*Method)(const std::shared_ptr<IncomingRequest>&);
    typedef oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
            (T::*MethodAsync)(const std::shared_ptr<IncomingRequest>&);

  private:

    class ErrorHandlingCoroutine : public oatpp::async::CoroutineWithResult<ErrorHandlingCoroutine, const std::shared_ptr<OutgoingResponse>&> {
    private:
      Handler* m_handler;
      std::shared_ptr<IncomingRequest> m_request;
    public:

      ErrorHandlingCoroutine(Handler* handler, const std::shared_ptr<IncomingRequest>& request)
        : m_handler(handler)
        , m_request(request)
      {}

      async::Action act() override {
        return (m_handler->m_controller->*m_handler->m_methodAsync)(m_request)
          .callbackTo(&ErrorHandlingCoroutine::onResponse);
      }

      async::Action onResponse(const std::shared_ptr<OutgoingResponse>& response) {
        return this->_return(response);
      }

      async::Action handleError(async::Error* error) override {

        std::exception_ptr ePtr = error->getExceptionPtr();
        if(!ePtr) {
          ePtr = std::make_exception_ptr(*error);
        }

        try {
          try {
            std::rethrow_exception(ePtr);
          } catch (...) {
            std::throw_with_nested(HttpServerError(m_request, "[ApiController]: Error processing async request"));
          }
        } catch (...) {
          ePtr = std::current_exception();
          auto response = m_handler->m_controller->handleError(ePtr);
          if (response != nullptr) {
            return this->_return(response);
          }
        }

        return new async::Error(ePtr);

      }

    };

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
    
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {

      if(m_method == nullptr) {
        if(m_methodAsync == nullptr) {
          throw HttpServerError(request, "[ApiController]: Error. Handler method is nullptr");
        }
        throw HttpServerError(request, "[ApiController]: Error. Non-async call to async endpoint");
      }

      try {
        try {
          return (m_controller->*m_method)(request);
        } catch (...) {
          std::throw_with_nested(HttpServerError(request, "[ApiController]: Error processing request"));
        }
      } catch (...) {
        auto ePtr = std::current_exception();
        auto response = m_controller->handleError(ePtr);
        if (response != nullptr) {
          return response;
        }
        std::rethrow_exception(ePtr);
      }
      
    }
    
    oatpp::async::CoroutineStarterForResult<const std::shared_ptr<OutgoingResponse>&>
    handleAsync(const std::shared_ptr<protocol::http::incoming::Request>& request) override {

      if(m_methodAsync == nullptr) {
        if(m_method == nullptr) {
          throw HttpServerError(request, "[ApiController]: Error. Handler method is nullptr");
        }
        throw HttpServerError(request, "[ApiController]: Error. Async call to non-async endpoint");
      }

      //if(m_controller->m_errorHandler) {
        return ErrorHandlingCoroutine::startForResult(this, request);
      //}

      //return (m_controller->*m_methodAsync)(request);

    }

    Method setMethod(Method method) {
      auto prev = m_method;
      m_method = method;
      return prev;
    }

    Method getMethod() {
      return m_method;
    }

    MethodAsync setMethodAsync(MethodAsync methodAsync) {
      auto prev = m_methodAsync;
      m_methodAsync = methodAsync;
      return prev;
    }

    MethodAsync getMethodAsync() {
      return m_methodAsync;
    }
    
  };

protected:

  /*
   * Set endpoint info by endpoint name. (Endpoint name is the 'NAME' parameter of the ENDPOINT macro)
   * Info should be set before call to addEndpointsToRouter();
   */
  void setEndpointInfo(const std::string& endpointName, const std::shared_ptr<Endpoint::Info>& info);

  /*
   * Get endpoint info by endpoint name. (Endpoint name is the 'NAME' parameter of the ENDPOINT macro)
   */
  std::shared_ptr<Endpoint::Info> getEndpointInfo(const std::string& endpointName);

  /*
   * Set endpoint Request handler.
   * @param endpointName
   * @param handler
   */
  void setEndpointHandler(const std::string& endpointName, const std::shared_ptr<RequestHandler>& handler);

  /*
   * Get endpoint Request handler.
   * @param endpointName
   * @return
   */
  std::shared_ptr<RequestHandler> getEndpointHandler(const std::string& endpointName);
  
protected:
  Endpoints m_endpoints;
  std::shared_ptr<handler::ErrorHandler> m_errorHandler;
  std::shared_ptr<handler::AuthorizationHandler> m_defaultAuthorizationHandler;
  std::shared_ptr<mime::ContentMappers> m_contentMappers;
  std::unordered_map<std::string, std::shared_ptr<Endpoint::Info>> m_endpointInfo;
  std::unordered_map<std::string, std::shared_ptr<RequestHandler>> m_endpointHandlers;
  const oatpp::String m_routerPrefix;
public:

  ApiController(const std::shared_ptr<mime::ContentMappers>& contentMappers, const oatpp::String &routerPrefix = nullptr)
    : m_contentMappers(contentMappers)
    , m_routerPrefix(routerPrefix)
  {

  }

  ApiController(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& defaultObjectMapper, const oatpp::String &routerPrefix = nullptr)
    : m_contentMappers(std::make_shared<mime::ContentMappers>())
    , m_routerPrefix(routerPrefix)
  {
    m_contentMappers->setDefaultMapper(defaultObjectMapper);
  }
public:
  
  template<class T>
  static std::shared_ptr<Endpoint> createEndpoint(Endpoints& endpoints,
                                                  const std::shared_ptr<Handler<T>>& handler,
                                                  const EndpointInfoBuilder& infoBuilder)
  {
    auto endpoint = Endpoint::createShared(handler, infoBuilder);
    endpoints.append(endpoint);
    return endpoint;
  }
  
  /**
   * Get list of Endpoints created via ENDPOINT macro
   */
  const Endpoints& getEndpoints();

  /**
   * Set error handler to handle errors that occur during the endpoint's execution
   */
  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler);

  /**
   * Handle the exception using the registered ErrorHandler or if no handler has been set, uses the DefaultErrorHandler::handleError
   * @note Does not rethrow an exception anymore, OutgoingResponse has to be returned by the caller!
   * @note If this handler fails to handle the exception, it will be handled by the connection handlers ErrorHandler.
   */
  std::shared_ptr<OutgoingResponse> handleError(const std::exception_ptr& exceptionPtr) const;

  /**
   * [under discussion]
   * Set authorization handler to handle calls to handleAuthorization.
   * Must be called before controller is added to a router or swagger-doc if an endpoint uses the AUTHORIZATION macro
   */
  void setDefaultAuthorizationHandler(const std::shared_ptr<handler::AuthorizationHandler>& authorizationHandler);

  /**
   * Get authorization handler.
   * @return
   */
  std::shared_ptr<handler::AuthorizationHandler> getDefaultAuthorizationHandler();

  /**
   * [under discussion]
   * Do not use it directly. This method is under discussion.
   * Currently returns AuthorizationObject created by AuthorizationHandler or return DefaultAuthorizationObject by DefaultAuthorizationHandler if AuthorizationHandler is null
   */
  std::shared_ptr<handler::AuthorizationObject> handleDefaultAuthorization(const String &authHeader) const;

  /**
   * Get content mappers
   * @return
   */
  const std::shared_ptr<mime::ContentMappers>& getContentMappers() const;
  
  // Helper methods
  
  std::shared_ptr<OutgoingResponse> createResponse(const Status& status,
                                                   const oatpp::String& str) const;
  
  std::shared_ptr<OutgoingResponse> createResponse(const Status& status) const;

  std::shared_ptr<OutgoingResponse> createDtoResponse(const Status& status,
                                                      const oatpp::Void& dto,
                                                      const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) const;
  
  std::shared_ptr<OutgoingResponse> createDtoResponse(const Status& status,
                                                      const oatpp::Void& dto) const;

  std::shared_ptr<ApiController::OutgoingResponse> createDtoResponse(const Status& status,
                                                                     const oatpp::Void& dto,
                                                                     const std::vector<oatpp::String>& acceptableContentTypes) const;

public:

  template<typename T>
  struct TypeInterpretation {

    static T fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
      (void) text;
      success = false;
      OATPP_LOGe("[oatpp::web::server::api::ApiController::TypeInterpretation::fromString()]",
                 "Error. No conversion from '{}' to '{}' is defined.", "oatpp::String", typeName)
      throw std::runtime_error("[oatpp::web::server::api::ApiController::TypeInterpretation::fromString()]: Error. "
                               "No conversion from 'oatpp::String' to '" + *typeName + "' is defined. "
                               "Please define type conversion.");
    }

  };

};

template<>
struct ApiController::TypeInterpretation <oatpp::String> {
  static oatpp::String fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    success = true;
    return text;
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::Int8> {
  static oatpp::Int8 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    //TODO: check the range and perhaps throw an exception if the variable doesn't fit
    return static_cast<Int8::UnderlyingType>(utils::Conversion::strToInt32(text, success));
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::UInt8> {
  static oatpp::UInt8 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    //TODO: check the range and perhaps throw an exception if the variable doesn't fit
    return static_cast<UInt8::UnderlyingType>(utils::Conversion::strToUInt32(text, success));
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::Int16> {
  static oatpp::Int16 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    //TODO: check the range and perhaps throw an exception if the variable doesn't fit
    return static_cast<Int16::UnderlyingType>(utils::Conversion::strToInt32(text, success));
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::UInt16> {
  static oatpp::UInt16 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    //TODO: check the range and perhaps throw an exception if the variable doesn't fit
    return static_cast<UInt16::UnderlyingType>(utils::Conversion::strToUInt32(text, success));
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::Int32> {
  static oatpp::Int32 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    return utils::Conversion::strToInt32(text, success);
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::UInt32> {
  static oatpp::UInt32 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    return utils::Conversion::strToUInt32(text, success);
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::Int64> {
  static oatpp::Int64 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    return utils::Conversion::strToInt64(text, success);
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::UInt64> {
  static oatpp::UInt64 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    return utils::Conversion::strToUInt64(text, success);
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::Float32> {
  static oatpp::Float32 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    return utils::Conversion::strToFloat32(text, success);
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::Float64> {
  static oatpp::Float64 fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    return utils::Conversion::strToFloat64(text, success);
  }
};

template<>
struct ApiController::TypeInterpretation <oatpp::Boolean> {
  static oatpp::Boolean fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    (void) typeName;
    return utils::Conversion::strToBool(text, success);
  }
};

template<class T, class I>
struct ApiController::TypeInterpretation <data::type::EnumObjectWrapper<T, I>> {

  typedef data::type::EnumObjectWrapper<T, I> EnumOW;
  typedef typename I::UnderlyingTypeObjectWrapper UTOW;

  static EnumOW fromString(const oatpp::String& typeName, const oatpp::String& text, bool& success) {
    const auto& parsedValue = ApiController::TypeInterpretation<UTOW>::fromString(typeName, text, success);
    if(success) {
      data::type::EnumInterpreterError error = data::type::EnumInterpreterError::OK;
      const auto& result = I::fromInterpretation(parsedValue, false, error);
      if(error == data::type::EnumInterpreterError::OK) {
        return result.template cast<EnumOW>();
      }
      success = false;
    }
    return nullptr;
  }

};

}}}}

#endif /* oatpp_web_server_api_Controller_hpp */
