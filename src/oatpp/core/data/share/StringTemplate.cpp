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

#include "StringTemplate.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace data { namespace share {

StringTemplate::StringTemplate(const oatpp::String& text, std::vector<Variable>&& variables)
  : m_text(text)
  , m_variables(variables)
{
  v_buff_size prevPos = 0;
  for(v_int32 i = 0; i < m_variables.size(); i++) {
    const auto& var = m_variables[i];

    if(var.posStart < prevPos) {
      throw std::runtime_error("[oatpp::data::share::StringTemplate::StringTemplate()]: Error. The template variable positions can't intersect.");
    }

    if(var.posEnd < var.posStart) {
      throw std::runtime_error("[oatpp::data::share::StringTemplate::StringTemplate()]: Error. The template variable can't have a negative size.");
    }

    if(var.posEnd >= m_text->getSize()) {
      throw std::runtime_error("[oatpp::data::share::StringTemplate::StringTemplate()]: Error. The template variable can't out-bound the template text.");
    }
  }
}

void StringTemplate::format(stream::ConsistentOutputStream* stream, const std::vector<oatpp::String>& params) const {

  if(params.size() != m_variables.size()) {
    throw std::runtime_error("[oatpp::data::share::StringTemplate::format()]: Error. Wrong number of arguments.");
  }

  v_buff_size prevPos = 0;
  for(v_int32 i = 0; i < m_variables.size(); i++) {

    const auto& var = m_variables[i];
    const auto& param = params[i];

    if(prevPos < var.posStart) {
      stream->writeSimple(m_text->getData() + prevPos, var.posStart - prevPos);
    }

    stream->writeSimple(param->getData(), param->getSize());

    prevPos = var.posEnd + 1;

  }

  if(prevPos < m_text->getSize()) {
    stream->writeSimple(m_text->getData() + prevPos, m_text->getSize() - prevPos);
  }

}

void StringTemplate::format(stream::ConsistentOutputStream* stream, const std::unordered_map<oatpp::String, oatpp::String>& params) const {

  v_buff_size prevPos = 0;
  for(v_int32 i = 0; i < m_variables.size(); i++) {

    const auto& var = m_variables[i];
    auto paramIt = params.find(var.name);
    if(paramIt == params.end()) {
      throw std::runtime_error("[oatpp::data::share::StringTemplate::format()]: Error. Parameter not found. Name=" + var.name->std_str());
    }

    const auto& param = paramIt->second;

    if(prevPos < var.posStart) {
      stream->writeSimple(m_text->getData() + prevPos, var.posStart - prevPos);
    }

    stream->writeSimple(param->getData(), param->getSize());

    prevPos = var.posEnd + 1;

  }

  if(prevPos < m_text->getSize()) {
    stream->writeSimple(m_text->getData() + prevPos, m_text->getSize() - prevPos);
  }

}

oatpp::String StringTemplate::format(const std::vector<oatpp::String>& params) const {
  stream::BufferOutputStream stream;
  format(&stream, params);
  return stream.toString();
}

oatpp::String StringTemplate::format(const std::unordered_map<oatpp::String, oatpp::String>& params) const {
  stream::BufferOutputStream stream;
  format(&stream, params);
  return stream.toString();
}

}}}
