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

#ifndef oatpp_parser_json_mapping_Serializer_hpp
#define oatpp_parser_json_mapping_Serializer_hpp

#include "oatpp/parser/json/Utils.hpp"
#include "oatpp/parser/json/Beautifier.hpp"
#include "oatpp/core/Types.hpp"
#include <vector>

namespace oatpp { namespace parser { namespace json { namespace mapping {

/**
 * Json Serializer.
 * Serializes oatpp DTO object to json. See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/).
 */
class Serializer {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::BaseObject::Property Property;
  typedef oatpp::data::mapping::type::BaseObject::Properties Properties;

  typedef oatpp::String String;
public:
  /**
   * Serializer config.
   */
  class Config : public oatpp::base::Countable {
  public:
    /**
     * Constructor.
     */
    Config()
    {}
  public:

    /**
     * Create shared config.
     * @return - `std::shared_ptr` to Config.
     */
    static std::shared_ptr<Config> createShared(){
      return std::make_shared<Config>();
    }

    /**
     * Include fields with value == nullptr into serialized json.
     * Field will still be included when field-info `required` is set to true and &id:alwaysIncludeRequired is set to true.
     */
    bool includeNullFields = true;

    /**
     * Always include required fields (set in in DTO_FIELD_INFO) even if they are `value == nullptr`
     */
    bool alwaysIncludeRequired = false;

    /**
     * Always include array or map elements, even if their value is `nullptr`.
     */
    bool alwaysIncludeNullCollectionElements = false;

    /**
     * If `true` - insert string `"<unknown-type>"` in json field value in case unknown field found.
     * Fail if `false`.
     * Known types for this serializer are:<br>
     * (String, Int8, Int16, Int32, Int64, Float32, Float64, Boolean, DTOs, List, Fields).
     */
    bool throwOnUnknownTypes = true;

    /**
     * Use JSON Beautifier.
     */
    bool useBeautifier = false;

    /**
     * Beautifier Indent.
     */
    oatpp::String beautifierIndent = "  ";

    /**
     * Beautifier new line.
     */
    oatpp::String beautifierNewLine = "\n";

    /**
     * Enable type interpretations.
     */
    std::vector<std::string> enabledInterpretations = {};

    /**
     * Escape flags.
     */
    v_uint32 escapeFlags = json::Utils::FLAG_ESCAPE_ALL;

  };
public:
  typedef void (*SerializerMethod)(Serializer*,
                                   data::stream::ConsistentOutputStream*,
                                   const oatpp::Void&);
private:

  template<class T>
  static void serializePrimitive(Serializer* serializer,
                                 data::stream::ConsistentOutputStream* stream,
                                 const oatpp::Void& polymorph){
    (void) serializer;

    if(polymorph){
      stream->writeAsString(* static_cast<typename T::ObjectType*>(polymorph.get()));
    } else {
      stream->writeSimple("null", 4);
    }
  }
  
  static void serializeString(oatpp::data::stream::ConsistentOutputStream* stream,
                              const char* data,
                              v_buff_size size,
                              v_uint32 escapeFlags);

  static void serializeString(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const oatpp::Void& polymorph);

  static void serializeAny(Serializer* serializer,
                           data::stream::ConsistentOutputStream* stream,
                           const oatpp::Void& polymorph);

  static void serializeEnum(Serializer* serializer,
                            data::stream::ConsistentOutputStream* stream,
                            const oatpp::Void& polymorph);

  static void serializeCollection(Serializer* serializer,
                                  data::stream::ConsistentOutputStream* stream,
                                  const oatpp::Void& polymorph);

  static void serializeMap(Serializer* serializer,
                           data::stream::ConsistentOutputStream* stream,
                           const oatpp::Void& polymorph);

  static void serializeObject(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const oatpp::Void& polymorph);

  void serialize(data::stream::ConsistentOutputStream* stream, const oatpp::Void& polymorph);

private:
  std::shared_ptr<Config> m_config;
  std::vector<SerializerMethod> m_methods;
public:

  /**
   * Constructor.
   * @param config - serializer config.
   */
  Serializer(const std::shared_ptr<Config>& config = std::make_shared<Config>());

  /**
   * Set serializer method for type.
   * @param classId - &id:oatpp::data::mapping::type::ClassId;.
   * @param method - `typedef void (*SerializerMethod)(Serializer*, data::stream::ConsistentOutputStream*, const oatpp::Void&)`.
   */
  void setSerializerMethod(const data::mapping::type::ClassId& classId, SerializerMethod method);

  /**
   * Serialize object to stream.
   * @param stream - &id:oatpp::data::stream::ConsistentOutputStream;.
   * @param polymorph - DTO as &id:oatpp::Void;.
   */
  void serializeToStream(data::stream::ConsistentOutputStream* stream, const oatpp::Void& polymorph);

  /**
   * Get serializer config.
   * @return
   */
  const std::shared_ptr<Config>& getConfig();

};

}}}}

#endif /* oatpp_parser_json_mapping_Serializer_hpp */
