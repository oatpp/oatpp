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

namespace oatpp { namespace json {

ObjectMapper::ObjectMapper(const SerializerConfig& serializerConfig, const DeserializerConfig& deserializerConfig)
  : data::mapping::ObjectMapper(getMapperInfo())
  , m_serializerConfig(serializerConfig)
  , m_deserializerConfig(deserializerConfig)
{}

void ObjectMapper::write(data::stream::ConsistentOutputStream* stream, const oatpp::Void& variant, data::mapping::ErrorStack& errorStack) const {

  data::mapping::Tree tree;
  {
    data::mapping::ObjectToTreeMapper::MappingState state;
    state.config = &m_serializerConfig.mapper;
    state.tree = &tree;
    m_objectToTreeMapper.map(state, variant);
    if(!state.errorStack.empty()) {
      errorStack = std::move(state.errorStack);
      return;
    }
  }

  {
    Serializer::MappingState state;
    state.config = &m_serializerConfig.json;
    state.tree = &tree;
    Serializer::serializeToStream(stream, state);
    if(!state.errorStack.empty()) {
      errorStack = std::move(state.errorStack);
      return;
    }
  }

}

oatpp::Void ObjectMapper::read(utils::parser::Caret& caret, const data::type::Type* type, data::mapping::ErrorStack& errorStack) const {

  data::mapping::Tree tree;

  {
    Deserializer::MappingState state;
    state.caret = &caret;
    state.tree = &tree;
    state.config = &m_deserializerConfig.json;
    Deserializer::deserialize(state);
    if(!state.errorStack.empty()) {
      errorStack = std::move(state.errorStack);
      return nullptr;
    }
  }

  {
    data::mapping::TreeToObjectMapper::MappingState state;
    state.tree = &tree;
    state.config = &m_deserializerConfig.mapper;
    const auto & result = m_treeToObjectMapper.map(state, type);
    if(!state.errorStack.empty()) {
      errorStack = std::move(state.errorStack);
      return nullptr;
    }
    return result;
  }

}

const ObjectMapper::SerializerConfig& ObjectMapper::serializerConfig() const {
  return m_serializerConfig;
}

const ObjectMapper::DeserializerConfig& ObjectMapper::deserializerConfig() const {
  return m_deserializerConfig;
}

ObjectMapper::SerializerConfig& ObjectMapper::serializerConfig() {
  return m_serializerConfig;
}

ObjectMapper::DeserializerConfig& ObjectMapper::deserializerConfig() {
  return m_deserializerConfig;
}

}}
