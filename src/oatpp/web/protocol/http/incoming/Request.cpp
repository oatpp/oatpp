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

#include "Request.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

Request::Request(const http::RequestStartingLine& startingLine,
                 const url::mapping::Pattern::MatchMap& pathVariables,
                 const http::Headers& headers,
                 const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                 const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder)
  : m_startingLine(startingLine)
  , m_pathVariables(pathVariables)
  , m_headers(headers)
  , m_bodyStream(bodyStream)
  , m_bodyDecoder(bodyDecoder)
  , m_queryParamsParsed(false)
{}

std::shared_ptr<Request> Request::createShared(const http::RequestStartingLine& startingLine,
                                               const url::mapping::Pattern::MatchMap& pathVariables,
                                               const http::Headers& headers,
                                               const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                               const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder) {
  return Shared_Incoming_Request_Pool::allocateShared(startingLine, pathVariables, headers, bodyStream, bodyDecoder);
}

const http::RequestStartingLine& Request::getStartingLine() const {
  return m_startingLine;
}

const url::mapping::Pattern::MatchMap& Request::getPathVariables() const {
  return m_pathVariables;
}

const http::Headers& Request::getHeaders() const {
  return m_headers;
}

const http::QueryParams& Request::getQueryParameters() const {
  if(!m_queryParamsParsed) {
    m_queryParams = oatpp::network::Url::Parser::labelQueryParams(m_pathVariables.getTail());
    m_queryParamsParsed = true;
  }
  return m_queryParams;
}

oatpp::String Request::getQueryParameter(const oatpp::data::share::StringKeyLabel& name) const {
  auto iter = getQueryParameters().find(name);
  if (iter == getQueryParameters().end()) {
    return nullptr;
  } else {
    return iter->second.toString();
  }
}

oatpp::String Request::getQueryParameter(const oatpp::data::share::StringKeyLabel& name, const oatpp::String& defaultValue) const {
  auto value = getQueryParameter(name);
  return value ? value : defaultValue;
}

std::shared_ptr<oatpp::data::stream::InputStream> Request::getBodyStream() const {
  return m_bodyStream;
}

std::shared_ptr<const http::incoming::BodyDecoder> Request::getBodyDecoder() const {
  return m_bodyDecoder;
}

oatpp::String Request::getHeader(const oatpp::data::share::StringKeyLabelCI_FAST& headerName) const{
  auto it = m_headers.find(headerName);
  if(it != m_headers.end()) {
    return it->second.toString();
  }
  return nullptr;
}

oatpp::String Request::getPathVariable(const oatpp::data::share::StringKeyLabel& name) const {
  return m_pathVariables.getVariable(name);
}

oatpp::String Request::getPathTail() const {
  return m_pathVariables.getTail();
}

void Request::transferBody(data::stream::WriteCallback* writeCallback) const {
  m_bodyDecoder->decode(m_headers, m_bodyStream.get(), writeCallback);
}

void Request::transferBodyToStream(oatpp::data::stream::OutputStream* toStream) const {
  m_bodyDecoder->decodeToStream(m_headers, m_bodyStream.get(), toStream);
}

oatpp::String Request::readBodyToString() const {
  return m_bodyDecoder->decodeToString(m_headers, m_bodyStream.get());
}

async::CoroutineStarter Request::transferBodyAsync(const std::shared_ptr<data::stream::AsyncWriteCallback>& writeCallback) const {
  return m_bodyDecoder->decodeAsync(m_headers, m_bodyStream, writeCallback);
}

async::CoroutineStarter Request::transferBodyToStreamAsync(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
  return m_bodyDecoder->decodeToStreamAsync(m_headers, m_bodyStream, toStream);
}

async::CoroutineStarterForResult<const oatpp::String&> Request::readBodyToStringAsync() const {
  return m_bodyDecoder->decodeToStringAsync(m_headers, m_bodyStream);
}

}}}}}