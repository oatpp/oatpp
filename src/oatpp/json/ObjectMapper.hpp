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

#ifndef oatpp_json_ObjectMapper_hpp
#define oatpp_json_ObjectMapper_hpp

#include "./Serializer.hpp"
#include "./Deserializer.hpp"

#include "oatpp/data/mapping/ObjectToTreeMapper.hpp"
#include "oatpp/data/mapping/TreeToObjectMapper.hpp"
#include "oatpp/data/mapping/ObjectMapper.hpp"

namespace oatpp { namespace json {

/**
 * Json ObjectMapper. Serialized/Deserializes oatpp DTO objects to/from JSON.
 * See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/). <br>
 * Extends &id:oatpp::base::Countable;, &id:oatpp::data::mapping::ObjectMapper;.
 */
class ObjectMapper : public oatpp::base::Countable, public oatpp::data::mapping::ObjectMapper {
private:
  static Info& getMapperInfo() {
    static Info info("application/json");
    return info;
  }

public:

  class DeserializerConfig : public data::mapping::TreeToObjectMapper::Config, public Deserializer::Config {
  public:

  };

public:

  class SerializerConfig : public data::mapping::ObjectToTreeMapper::Config, public Serializer::Config {
  public:

  };

private:
  std::shared_ptr<SerializerConfig> m_serializerConfig;
  std::shared_ptr<DeserializerConfig> m_deserializerConfig;
private:
  data::mapping::ObjectToTreeMapper m_objectToTreeMapper;
  data::mapping::TreeToObjectMapper m_treeToObjectMapper;
public:

  ObjectMapper(const std::shared_ptr<SerializerConfig>& serializerConfig = std::make_shared<SerializerConfig>(),
               const std::shared_ptr<DeserializerConfig>& deserializerConfig = std::make_shared<DeserializerConfig>());

public:

  static std::shared_ptr<ObjectMapper>
  createShared(const std::shared_ptr<SerializerConfig>& serializerConfig = std::make_shared<SerializerConfig>(),
               const std::shared_ptr<DeserializerConfig>& deserializerConfig = std::make_shared<DeserializerConfig>());

  /**
   * Implementation of &id:oatpp::data::mapping::ObjectMapper::write;.
   * @param stream - stream to write serializerd data to &id:oatpp::data::stream::ConsistentOutputStream;.
   * @param variant - object to serialize &id:oatpp::Void;.
   */
  void write(data::stream::ConsistentOutputStream* stream, const oatpp::Void& variant) const override;

  /**
   * Implementation of &id:oatpp::data::mapping::ObjectMapper::read;.
   * @param caret - &id:oatpp::utils::parser::Caret;.
   * @param type - type of resultant object &id:oatpp::data::mapping::type::Type;.
   * @return - &id:oatpp::Void; holding resultant object.
   */
  oatpp::Void read(oatpp::utils::parser::Caret& caret, const oatpp::data::mapping::type::Type* const type) const override;


  std::shared_ptr<ObjectMapper::SerializerConfig> getSerializerConfig();

  std::shared_ptr<DeserializerConfig> getDeserializerConfig();
  
};
  
}}

#endif /* oatpp_json_ObjectMapper_hpp */
