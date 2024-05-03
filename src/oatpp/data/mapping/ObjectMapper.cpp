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

#include "ObjectMapper.hpp"

#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace data { namespace mapping {

void ErrorStack::push(const oatpp::String& error) {
  stack.emplace_back(error);
}

void ErrorStack::splice(ErrorStack& errorStack) {
  stack.splice(stack.end(), errorStack.stack);
}

oatpp::String ErrorStack::stacktrace() const {
  stream::BufferOutputStream ss;
  for(auto& s : stack) {
    ss << s << "\n";
  }
  return ss.toString();
}

bool ErrorStack::empty() const {
  return stack.empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MappingError

MappingError::MappingError(const ErrorStack& errorStack)
  : std::runtime_error(errorStack.empty() ? "[oatpp::data::mapping::MappingError]"
                                          : errorStack.stack.front().getValue("<empty-error-stack>"))
  , m_stack(errorStack)
{}

MappingError::MappingError(ErrorStack&& errorStack)
  : std::runtime_error(errorStack.empty() ? "[oatpp::data::mapping::MappingError]"
                                          : errorStack.stack.front().getValue("<empty-error-stack>"))
  , m_stack(std::move(errorStack))
{}

const ErrorStack& MappingError::errorStack() const {
  return m_stack;
}

ErrorStack& MappingError::errorStack() {
  return m_stack;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ObjectMapper

ObjectMapper::ObjectMapper(const Info& info)
  : m_info(info)
{}

const ObjectMapper::Info& ObjectMapper::getInfo() const {
  return m_info;
}

oatpp::String ObjectMapper::writeToString(const type::Void& variant) const {
  stream::BufferOutputStream stream;
  ErrorStack errorStack;
  write(&stream, variant, errorStack);
  if(!errorStack.empty()) {
    throw MappingError(std::move(errorStack));
  }
  return stream.toString();
}

}}}
