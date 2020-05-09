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

#ifndef oatpp_parser_json_mapping_ObjectMapper_hpp
#define oatpp_parser_json_mapping_ObjectMapper_hpp

#include "./Serializer.hpp"
#include "./Deserializer.hpp"

#include "oatpp/core/data/mapping/ObjectMapper.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {

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
private:
  std::shared_ptr<Serializer> m_serializer;
  std::shared_ptr<Deserializer> m_deserializer;
public:
  /**
   * Constructor.
   * @param serializerConfig - &id:oatpp::parser::json::mapping::Serializer::Config;.
   * @param deserializerConfig - &id:oatpp::parser::json::mapping::Deserializer::Config;.
   */
  ObjectMapper(const std::shared_ptr<Serializer::Config>& serializerConfig,
               const std::shared_ptr<Deserializer::Config>& deserializerConfig);

  /**
   * Constructor.
   * @param serializer
   * @param deserializer
   */
  ObjectMapper(const std::shared_ptr<Serializer>& serializer = std::make_shared<Serializer>(),
               const std::shared_ptr<Deserializer>& deserializer = std::make_shared<Deserializer>());
public:

  /**
   * Create shared ObjectMapper.
   * @param serializerConfig - &id:oatpp::parser::json::mapping::Serializer::Config;.
   * @param deserializerConfig - &id:oatpp::parser::json::mapping::Deserializer::Config;.
   * @return - `std::shared_ptr` to ObjectMapper.
   */
  static std::shared_ptr<ObjectMapper>
  createShared(const std::shared_ptr<Serializer::Config>& serializerConfig,
               const std::shared_ptr<Deserializer::Config>& deserializerConfig);

  /**
   * Create shared ObjectMapper.
   * @param serializer
   * @param deserializer
   * @return
   */
  static std::shared_ptr<ObjectMapper>
  createShared(const std::shared_ptr<Serializer>& serializer = std::make_shared<Serializer>(),
               const std::shared_ptr<Deserializer>& deserializer = std::make_shared<Deserializer>());

  /**
   * Implementation of &id:oatpp::data::mapping::ObjectMapper::write;.
   * @param stream - stream to write serializerd data to &id:oatpp::data::stream::ConsistentOutputStream;.
   * @param variant - object to serialize &id:oatpp::Void;.
   */
  void write(data::stream::ConsistentOutputStream* stream, const oatpp::Void& variant) const override;

  /**
   * Implementation of &id:oatpp::data::mapping::ObjectMapper::read;.
   * @param caret - &id:oatpp::parser::Caret;.
   * @param type - type of resultant object &id:oatpp::data::mapping::type::Type;.
   * @return - &id:oatpp::Void; holding resultant object.
   */
  oatpp::Void read(oatpp::parser::Caret& caret, const oatpp::data::mapping::type::Type* const type) const override;


  /**
   * Get serializer.
   * @return
   */
  std::shared_ptr<Serializer> getSerializer();

  /**
   * Get deserializer.
   * @return
   */
  std::shared_ptr<Deserializer> getDeserializer();
  
};
  
}}}}

#endif /* oatpp_parser_json_mapping_ObjectMapper_hpp */
