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

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace client {

ApiClient::PathSegment ApiClient::parsePathSegment(p_char8 data, v_buff_size size, v_buff_size& position) {
  for(v_buff_size i = position; i < size; i++){
    v_char8 a = data[i];
    if(a == '{'){
      auto result = PathSegment(std::string((char*) &data[position], i - position), PathSegment::SEG_PATH);
      position = i;
      return result;
    }
  }
  auto result = PathSegment(std::string((char*) &data[position], size - position), PathSegment::SEG_PATH);
  position = size;
  return result;
}

ApiClient::PathSegment ApiClient::parseVarSegment(p_char8 data, v_buff_size size, v_buff_size& position) {
  for(v_buff_size i = position; i < size; i++){
    v_char8 a = data[i];
    if(a == '}'){
      auto result = PathSegment(std::string((char*) &data[position], i - position), PathSegment::SEG_VAR);
      position = i + 1;
      return result;
    }
  }
  auto result = PathSegment(std::string((char*) &data[position], size - position), PathSegment::SEG_VAR);
  position = size;
  return result;
}

ApiClient::PathPattern ApiClient::parsePathPattern(p_char8 data, v_buff_size size) {
  v_buff_size pos = 0;
  PathPattern result;
  while (pos < size) {
    v_char8 a = data[pos];
    if(a == '{') {
      pos ++;
      result.push_back(parseVarSegment(data, size, pos));
    } else {
      result.push_back(parsePathSegment(data, size, pos));
    }
  }
  return result;
}

void ApiClient::formatPath(oatpp::data::stream::ConsistentOutputStream* stream,
                           const PathPattern& pathPattern,
                           const std::shared_ptr<StringToStringMap>& params) {

  for (auto it = pathPattern.begin(); it != pathPattern.end(); ++ it) {
    const PathSegment& seg = *it;
    if(seg.type == PathSegment::SEG_PATH) {
      stream->writeSimple(seg.text.data(), seg.text.size());
    } else {
      auto key = oatpp::String(seg.text.data(), seg.text.length(), false);
      auto& param = params->get(key, nullptr);
      if(!param){
        OATPP_LOGD(TAG, "Path parameter '%s' not provided in the api call", seg.text.c_str());
        throw std::runtime_error("[oatpp::web::client::ApiClient::formatPath()]: Path parameter missing");
      }
      stream->data::stream::OutputStream::writeSimple(param);
    }
  }

}

void ApiClient::addPathQueryParams(data::stream::ConsistentOutputStream* stream,
                                   const std::shared_ptr<StringToStringMap>& params) {
  bool first = true;
  auto curr = params->getFirstEntry();
  while (curr != nullptr) {
    if (curr->getValue()->getSize()) {
      if (first) {
        stream->writeSimple("?", 1);
        first = false;
      } else {
        stream->writeSimple("&", 1);
      }
      stream->writeSimple(curr->getKey());
      stream->writeSimple("=", 1);
      stream->writeSimple(curr->getValue());
    }
    curr = curr->getNext();
  }
}

oatpp::String ApiClient::formatPath(const PathPattern& pathPattern,
                                    const std::shared_ptr<StringToStringMap>& pathParams,
                                    const std::shared_ptr<StringToStringMap>& queryParams)
{
  oatpp::data::stream::BufferOutputStream stream;
  formatPath(&stream, pathPattern, pathParams);
  if(queryParams) {
    addPathQueryParams(&stream, queryParams);
  }
  return stream.toString();
}

web::protocol::http::Headers ApiClient::mapToHeaders(const std::shared_ptr<StringToStringMap>& params) {

  oatpp::web::protocol::http::Headers result;
  if(params) {
    auto curr = params->getFirstEntry();

    while (curr != nullptr) {
      if(curr->getValue()) {
        result.put_LockFree(curr->getKey(), curr->getValue());
      } else {
        OATPP_LOGE(TAG, "Header parameter '%s' not provided in the api call", curr->getKey()->c_str());
        throw std::runtime_error("[oatpp::web::client::ApiClient::mapToHeaders()]: Header parameter missing");
      }
      curr = curr->getNext();
    }
  }
  return result;

}

std::shared_ptr<RequestExecutor::ConnectionHandle> ApiClient::getConnection() {
  return m_requestExecutor->getConnection();
}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<RequestExecutor::ConnectionHandle>&> ApiClient::getConnectionAsync() {
  return m_requestExecutor->getConnectionAsync();
}

void ApiClient::invalidateConnection(const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle) {
  m_requestExecutor->invalidateConnection(connectionHandle);
}

std::shared_ptr<ApiClient::Response> ApiClient::executeRequest(const oatpp::String& method,
                                                               const PathPattern& pathPattern,
                                                               const std::shared_ptr<StringToStringMap>& headers,
                                                               const std::shared_ptr<StringToStringMap>& pathParams,
                                                               const std::shared_ptr<StringToStringMap>& queryParams,
                                                               const std::shared_ptr<RequestExecutor::Body>& body,
                                                               const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle)
{

  return m_requestExecutor->execute(method,
                                    formatPath(pathPattern, pathParams, queryParams),
                                    mapToHeaders(headers),
                                    body,
                                    connectionHandle);

}

oatpp::async::CoroutineStarterForResult<const std::shared_ptr<ApiClient::Response>&>
ApiClient::executeRequestAsync(const oatpp::String& method,
                               const PathPattern& pathPattern,
                               const std::shared_ptr<StringToStringMap>& headers,
                               const std::shared_ptr<StringToStringMap>& pathParams,
                               const std::shared_ptr<StringToStringMap>& queryParams,
                               const std::shared_ptr<RequestExecutor::Body>& body,
                               const std::shared_ptr<RequestExecutor::ConnectionHandle>& connectionHandle)
{

  return m_requestExecutor->executeAsync(method,
                                         formatPath(pathPattern, pathParams, queryParams),
                                         mapToHeaders(headers),
                                         body,
                                         connectionHandle);

}

}}}
