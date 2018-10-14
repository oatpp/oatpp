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

#ifndef oatpp_parser_json_mapping_ObjectMapper_hpp
#define oatpp_parser_json_mapping_ObjectMapper_hpp

#include "./Serializer.hpp"
#include "./Deserializer.hpp"

#include "oatpp/core/data/mapping/ObjectMapper.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {
  
class ObjectMapper : public oatpp::base::Controllable, public oatpp::data::mapping::ObjectMapper {
private:
  static Info& getMapperInfo() {
    static Info info("application/json");
    return info;
  }
public:
  ObjectMapper(const std::shared_ptr<Serializer::Config>& pSerializerConfig,
               const std::shared_ptr<Deserializer::Config>& pDeserializerConfig)
    : oatpp::data::mapping::ObjectMapper(getMapperInfo())
    , serializerConfig(pSerializerConfig)
    , deserializerConfig(pDeserializerConfig)
  {}
public:
  
  static std::shared_ptr<ObjectMapper>
  createShared(const std::shared_ptr<Serializer::Config>& serializerConfig = Serializer::Config::createShared(),
         const std::shared_ptr<Deserializer::Config>& deserializerConfig = Deserializer::Config::createShared()){
    return std::make_shared<ObjectMapper>(serializerConfig, deserializerConfig);
  }
  
  void write(const std::shared_ptr<oatpp::data::stream::OutputStream>& stream,
             const oatpp::data::mapping::type::AbstractObjectWrapper& variant) const override {
    Serializer::serialize(stream, variant, serializerConfig);
  }
  
  oatpp::data::mapping::type::AbstractObjectWrapper
  read(oatpp::parser::ParsingCaret& caret,
       const oatpp::data::mapping::type::Type* const type) const override {
    return Deserializer::deserialize(caret, deserializerConfig, type);
  }
  
  std::shared_ptr<Serializer::Config> serializerConfig;
  std::shared_ptr<Deserializer::Config> deserializerConfig;
  
};
  
}}}}

#endif /* oatpp_parser_json_mapping_ObjectMapper_hpp */
