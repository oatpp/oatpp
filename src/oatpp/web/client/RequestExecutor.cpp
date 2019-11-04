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

std::shared_ptr<RequestExecutor::Response> RequestExecutor::execute(
  const String& method,
  const String& path,
  const Headers& headers,
  const std::shared_ptr<Body>& body,
  const std::shared_ptr<ConnectionHandle>& connectionHandle
) {

  auto ch = connectionHandle;
  if(!ch) {
    ch = getConnection();
  }

  return executeOnce(method, path, headers, body, ch);

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
      return m_this->executeOnceAsync(m_method, m_path, m_headers, m_body, m_connectionHandle).callbackTo(&ExecutorCoroutine::onResponse);
    }

    Action onResponse(const std::shared_ptr<RequestExecutor::Response>& response) {
      return _return(response);
    }

  };

  return ExecutorCoroutine::startForResult(this, method, path, headers, body, connectionHandle);

}

}}}