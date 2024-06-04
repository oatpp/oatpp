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

#include "./ErrorHandler.hpp"
#include "oatpp/web/server/HttpServerError.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

namespace oatpp { namespace web { namespace server { namespace handler {

void DefaultErrorHandler::unwrapErrorStack(HttpServerErrorStacktrace& stacktrace, const std::exception& e) {

  stacktrace.stack.emplace_front(e.what());

  if (auto httpServerError = dynamic_cast<const HttpServerError*>(std::addressof(e))) {
    stacktrace.request = httpServerError->getRequest();
  } else if (auto httpError = dynamic_cast<const protocol::http::HttpError*>(std::addressof(e))) {
    for(auto& h : httpError->getHeaders().getAll_Unsafe()) {
      stacktrace.headers.putIfNotExists(h.first.toString(), h.second.toString());
    }
    stacktrace.status = httpError->getInfo().status;
  }

  try {
    std::rethrow_if_nested(e);
  } catch (const std::exception& nestedException) {
    unwrapErrorStack(stacktrace, nestedException);
  } catch (...) {
  }

}

std::shared_ptr<protocol::http::outgoing::Response> DefaultErrorHandler::handleError(const std::exception_ptr& error) {

  HttpServerErrorStacktrace stacktrace;
  stacktrace.status = protocol::http::Status::CODE_500;

  try {
    if(error) {
      std::rethrow_exception(error);
    }
  } catch (const std::exception& e) {
    unwrapErrorStack(stacktrace, e);
  } catch (...) {

  }

  return renderError(stacktrace);

}

std::shared_ptr<protocol::http::outgoing::Response> DefaultErrorHandler::renderError(const HttpServerErrorStacktrace& stacktrace) {

  data::stream::BufferOutputStream stream;
  stream << "server=" << protocol::http::Header::Value::SERVER << "\n";
  stream << "code=" << stacktrace.status.code << "\n";
  stream << "description=" << stacktrace.status.description << "\n";
  stream << "stacktrace:\n";

  for(auto& msg : stacktrace.stack) {
    stream << "  - " << msg << "\n";
  }

  auto response = protocol::http::outgoing::Response::createShared
    (stacktrace.status, protocol::http::outgoing::BufferBody::createShared(stream.toString()));

  response->putHeaderIfNotExists(protocol::http::Header::SERVER, protocol::http::Header::Value::SERVER);
  response->putHeaderIfNotExists(protocol::http::Header::CONNECTION, protocol::http::Header::Value::CONNECTION_CLOSE);

  for(const auto& pair : stacktrace.headers.getAll()) {
    response->putHeader_Unsafe(pair.first, pair.second);
  }

  return response;

}

}}}}
