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

#include "Endpoint.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace server { namespace api {

oatpp::String Endpoint::Info::toString() {
  auto stream = oatpp::data::stream::ChunkedBuffer::createShared();
  
  stream << "\nEndpoint\n";
  
  if(name) {
    stream << "name: '" << name << "'\n";
  }
  
  if(path){
    stream << "path: '" << path << "'\n";
  }
  
  if(method){
    stream << "method: '" << method << "'\n";
  }
  
  if(body.name != nullptr){
    stream << "body: '" << body.name << "', type: '" << body.type->name << "'\n";
  }
  
  auto headerIt = headers.begin();
  while (headerIt != headers.end()) {
    auto header = *headerIt++;
    stream << "header: '" << header.name << "', type: '" << header.type->name << "'\n";
  }
  
  auto pathIt = pathParams.begin();
  while (pathIt != pathParams.end()) {
    auto param = *pathIt++;
    stream << "pathParam: '" << param.name << "', type: '" << param.type->name << "'\n";
  }
  
  return stream->toString();
}

}}}}
