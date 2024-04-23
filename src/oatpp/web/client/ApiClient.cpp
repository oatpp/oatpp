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

#include "ApiClient.hpp"

#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace client {

std::shared_ptr<RequestExecutor::ConnectionHandle> ApiClient::getConnection() {
  return m_requestExecutor->getConnection();
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<RequestExecutor::ConnectionHandle>&> ApiClient::getConnectionAsync() {
  return m_requestExecutor->getConnectionAsync();
}

void ApiClient::invalidateConnection(const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle) {
  m_requestExecutor->invalidateConnection(connectionHandle);
}

std::shared_ptr<oatpp::data::mapping::ObjectMapper> ApiClient::getObjectMapper() {
  return m_objectMapper;
}

ApiClient::StringTemplate ApiClient::parsePathTemplate(const oatpp::String& name, const oatpp::String& text) {

  std::vector<StringTemplate::Variable> variables;
  utils::parser::Caret caret(text);

  while(caret.canContinue()) {

    if(caret.findChar('{')) {

      caret.inc();
      auto label = caret.putLabel();
      caret.findChar('}');
      label.end();

      StringTemplate::Variable var;
      var.posStart = label.getStartPosition() - 1;
      var.posEnd = label.getEndPosition();
      var.name = label.toString();

      variables.push_back(var);

    }

  }

  StringTemplate t(text, std::move(variables));
  auto extra = std::make_shared<PathTemplateExtra>();
  t.setExtraData(extra);

  extra->name = name;

  caret.setPosition(0);
  extra->hasQueryParams = caret.findChar('?');

  return t;

}

oatpp::String ApiClient::formatPath(const StringTemplate& pathTemplate,
                                    const std::unordered_map<oatpp::String, oatpp::String>& pathParams,
                                    const std::unordered_map<oatpp::String, oatpp::String>& queryParams)
{

  data::stream::BufferOutputStream stream;
  stream << pathTemplate.format(pathParams);

  if(queryParams.size() > 0) {
    auto extra = std::static_pointer_cast<PathTemplateExtra>(pathTemplate.getExtraData());
    bool first = !extra->hasQueryParams;
    for(const auto& q : queryParams) {
      oatpp::String value = q.second;
      if(value) {
        if (first) {
          stream.writeCharSimple('?');
          first = false;
        } else {
          stream.writeCharSimple('&');
        }
        stream << q.first << "=" << value;
      }
    }
  }

  return stream.toString();

}

std::shared_ptr<ApiClient::Response> ApiClient::executeRequest(const oatpp::String& method,
                                                               const StringTemplate& pathTemplate,
                                                               const Headers& headers,
                                                               const std::unordered_map<oatpp::String, oatpp::String>& pathParams,
                                                               const std::unordered_map<oatpp::String, oatpp::String>& queryParams,
                                                               const std::shared_ptr<RequestExecutor::Body>& body,
                                                               const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle)
{

  return m_requestExecutor->execute(method,
                                    formatPath(pathTemplate, pathParams, queryParams),
                                    headers,
                                    body,
                                    connectionHandle);

}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ApiClient::Response>&>
ApiClient::executeRequestAsync(const oatpp::String& method,
                               const StringTemplate& pathTemplate,
                               const Headers& headers,
                               const std::unordered_map<oatpp::String, oatpp::String>& pathParams,
                               const std::unordered_map<oatpp::String, oatpp::String>& queryParams,
                               const std::shared_ptr<RequestExecutor::Body>& body,
                               const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle)
{

  return m_requestExecutor->executeAsync(method,
                                         formatPath(pathTemplate, pathParams, queryParams),
                                         headers,
                                         body,
                                         connectionHandle);

}

}}}
