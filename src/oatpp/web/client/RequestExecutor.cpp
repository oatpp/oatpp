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

#include "RequestExecutor.hpp"

#include <thread>
#include <chrono>

namespace oatpp { namespace web { namespace client {

RequestExecutor::RequestExecutionError::RequestExecutionError(v_int32 errorCode, const char* message, v_int32 readErrorCode)
  : std::runtime_error(message)
  , m_errorCode(errorCode)
  , m_message(message)
  , m_readErrorCode(readErrorCode)
{}

v_int32 RequestExecutor::RequestExecutionError::getErrorCode() const {
  return m_errorCode;
}

const char* RequestExecutor::RequestExecutionError::getMessage() const {
  return m_message;
}

v_int32 RequestExecutor::RequestExecutionError::getReadErrorCode() const {
  return m_readErrorCode;
}

RequestExecutor::RequestExecutor(const std::shared_ptr<RetryPolicy>& retryPolicy)
  : m_retryPolicy(retryPolicy)
{}

std::shared_ptr<RequestExecutor::Response> RequestExecutor::execute(
  const String& method,
  const String& path,
  const Headers& headers,
  const std::shared_ptr<Body>& body,
  const std::shared_ptr<ConnectionHandle>& connectionHandle
) {

  if(!m_retryPolicy) {

    auto ch = connectionHandle;
    if (!ch) {
      ch = getConnection();
    }

    return executeOnce(method, path, headers, body, ch);

  } else {

    RetryPolicy::Context context;
    auto ch = connectionHandle;

    while(true) {

      context.attempt ++;

      try {

        if (!ch) {
          ch = getConnection();
        }

        auto response = executeOnce(method, path, headers, body, ch);

        if(!m_retryPolicy->retryOnResponse(response->getStatusCode(), context) || !m_retryPolicy->canRetry(context)) {
          return response;
        }

      } catch (...) {
        if(!m_retryPolicy->canRetry(context)) {
          break;
        }
      }

      invalidateConnection(ch);
      ch.reset();

      v_int64 waitMicro = m_retryPolicy->waitForMicroseconds(context);
      v_int64 tick0 = oatpp::base::Environment::getMicroTickCount();
      v_int64 tick = tick0;
      while(tick < tick0 + waitMicro) {
        std::this_thread::sleep_for(std::chrono::microseconds(tick0 + waitMicro - tick));
        tick = oatpp::base::Environment::getMicroTickCount();
      }

    }

  }

  return nullptr;

}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<RequestExecutor::Response>&>
RequestExecutor::executeAsync(
  const String& method,
  const String& path,
  const Headers& headers,
  const std::shared_ptr<Body>& body,
  const std::shared_ptr<ConnectionHandle>& connectionHandle
) {

  class ExecutorCoroutine : public oatpp::async::CoroutineWithResult<ExecutorCoroutine, const std::shared_ptr<RequestExecutor::Response>&> {
  private:
    RequestExecutor* m_this;
    String m_method;
    String m_path;
    Headers m_headers;
    std::shared_ptr<Body> m_body;
    std::shared_ptr<ConnectionHandle> m_connectionHandle;
    RetryPolicy::Context m_context;
  public:

    ExecutorCoroutine(RequestExecutor* _this,
                      const String& method,
                      const String& path,
                      const Headers& headers,
                      const std::shared_ptr<Body>& body,
                      const std::shared_ptr<ConnectionHandle>& connectionHandle)
      : m_this(_this)
      , m_method(method)
      , m_path(path)
      , m_headers(headers)
      , m_body(body)
      , m_connectionHandle(connectionHandle)
    {}

    Action act() override {

      if(!m_connectionHandle) {
        return m_this->getConnectionAsync().callbackTo(&ExecutorCoroutine::onConnection);
      }

      return yieldTo(&ExecutorCoroutine::execute);

    }

    Action onConnection(const std::shared_ptr<ConnectionHandle>& connectionHandle) {
      m_connectionHandle = connectionHandle;
      return yieldTo(&ExecutorCoroutine::execute);
    }

    Action execute() {
      m_context.attempt ++;
      return m_this->executeOnceAsync(m_method, m_path, m_headers, m_body, m_connectionHandle).callbackTo(&ExecutorCoroutine::onResponse);
    }

    Action onResponse(const std::shared_ptr<RequestExecutor::Response>& response) {

      if( m_this->m_retryPolicy &&
          m_this->m_retryPolicy->retryOnResponse(response->getStatusCode(), m_context) &&
          m_this->m_retryPolicy->canRetry(m_context)
      ) {
        return yieldTo(&ExecutorCoroutine::retry);
      }

      return _return(response);
    }

    Action retry() {

      if(m_connectionHandle) {
        m_this->invalidateConnection(m_connectionHandle);
        m_connectionHandle.reset();
      }

      return waitFor(std::chrono::microseconds(m_this->m_retryPolicy->waitForMicroseconds(m_context))).next(yieldTo(&ExecutorCoroutine::act));

    }

    Action handleError(Error* error) override {

      if(m_this->m_retryPolicy && m_this->m_retryPolicy->canRetry(m_context)) {
        return yieldTo(&ExecutorCoroutine::retry);
      }

      if(m_connectionHandle) {
        m_this->invalidateConnection(m_connectionHandle);
        m_connectionHandle.reset();
      }

      return error;
    }

  };

  return ExecutorCoroutine::startForResult(this, method, path, headers, body, connectionHandle);

}

}}}