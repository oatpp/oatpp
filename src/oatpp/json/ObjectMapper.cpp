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

ObjectMapper::ObjectMapper(const std::shared_ptr<Serializer::Config>& serializerConfig,
                           const std::shared_ptr<DeserializerConfig>& deserializerConfig)
  : data::mapping::ObjectMapper(getMapperInfo())
  , m_serializer(std::make_shared<Serializer>(serializerConfig))
  , m_deserializerConfig(deserializerConfig)
{}

ObjectMapper::ObjectMapper(const std::shared_ptr<Serializer>& serializer)
  : data::mapping::ObjectMapper(getMapperInfo())
  , m_serializer(serializer)
  , m_deserializerConfig(std::make_shared<DeserializerConfig>())
{}

std::shared_ptr<ObjectMapper> ObjectMapper::createShared(const std::shared_ptr<Serializer::Config>& serializerConfig,
                                                         const std::shared_ptr<DeserializerConfig>& deserializerConfig)
{
  return std::make_shared<ObjectMapper>(serializerConfig, deserializerConfig);
}

std::shared_ptr<ObjectMapper> ObjectMapper::createShared(const std::shared_ptr<Serializer>& serializer,
                                                         const std::shared_ptr<Deserializer>& deserializer)
{
  return std::make_shared<ObjectMapper>(serializer);
}

void ObjectMapper::write(data::stream::ConsistentOutputStream* stream,
                         const oatpp::Void& variant) const {
  m_serializer->serializeToStream(stream, variant);
}

oatpp::Void ObjectMapper::read(oatpp::utils::parser::Caret& caret, const oatpp::data::mapping::type::Type* const type) const {

  data::mapping::Tree tree;

  {
    Deserializer::MappingState state;
    state.caret = &caret;
    state.tree = &tree;
    state.config = m_deserializerConfig.get();

    Deserializer::deserialize(state);

    if (!state.errorStack.empty()) {
      //return nullptr;
      throw utils::parser::ParsingError(state.errorStacktrace(), 0, 0);
    }
  }

  {
    data::mapping::TreeToObjectMapper::MappingState state;
    state.tree = &tree;
    state.config = m_deserializerConfig.get();

    const auto & result = m_treeToObjectMapper.map(state, type);
    if(!state.errorStacktrace()->empty()) {
      throw utils::parser::ParsingError(state.errorStacktrace(), 0, 0);
    }

    return result;

  }

}

std::shared_ptr<Serializer> ObjectMapper::getSerializer() {
  return m_serializer;
}

std::shared_ptr<ObjectMapper::DeserializerConfig> ObjectMapper::getDeserializerConfig() {
  return m_deserializerConfig;
}

}}
