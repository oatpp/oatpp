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

#ifndef oatpp_parser_json_mapping_Deserializer_hpp
#define oatpp_parser_json_mapping_Deserializer_hpp

#include "oatpp/parser/json/Utils.hpp"
#include "oatpp/core/parser/Caret.hpp"
#include "oatpp/core/Types.hpp"

#include <vector>

namespace oatpp { namespace parser { namespace json { namespace mapping {

/**
 * Json Deserializer.
 * Deserialize oatpp DTO object from json. See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/).
 */
class Deserializer {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::BaseObject::Property Property;
  typedef oatpp::data::mapping::type::BaseObject::Properties Properties;

  typedef oatpp::String String;

public:

  /**
 * "'{' - expected"
 */
  static constexpr v_int32 ERROR_CODE_OBJECT_SCOPE_OPEN = 1;

  /**
   * "'}' - expected"
   */
  static constexpr v_int32 ERROR_CODE_OBJECT_SCOPE_CLOSE = 2;

  /**
   * "Unknown field"
   */
  static constexpr v_int32 ERROR_CODE_OBJECT_SCOPE_UNKNOWN_FIELD = 3;

  /**
   * "':' - expected"
   */
  static constexpr v_int32 ERROR_CODE_OBJECT_SCOPE_COLON_MISSING = 4;

  /**
   * "'[' - expected"
   */
  static constexpr v_int32 ERROR_CODE_ARRAY_SCOPE_OPEN = 5;

  /**
   * "']' - expected"
   */
  static constexpr v_int32 ERROR_CODE_ARRAY_SCOPE_CLOSE = 6;

  /**
   * "'true' or 'false' - expected"
   */
  static constexpr v_int32 ERROR_CODE_VALUE_BOOLEAN = 7;

public:

  /**
   * Deserializer config.
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
     * Create shared Config.
     * @return - `std::shared_ptr` to Config.
     */
    static std::shared_ptr<Config> createShared(){
      return std::make_shared<Config>();
    }

    /**
     * Do not fail if unknown field is found in json.
     * "unknown field" is the one which is not present in DTO object class.
     */
    bool allowUnknownFields = true;

    /**
     * Enable type interpretations.
     */
    std::vector<std::string> enabledInterpretations = {};

  };

public:
  typedef oatpp::Void (*DeserializerMethod)(Deserializer*, parser::Caret&, const Type* const);
private:
  static void skipScope(oatpp::parser::Caret& caret, v_char8 charOpen, v_char8 charClose);
  static void skipString(oatpp::parser::Caret& caret);
  static void skipToken(oatpp::parser::Caret& caret);
  static void skipValue(oatpp::parser::Caret& caret);
private:
  static const Type* guessNumberType(oatpp::parser::Caret& caret);
  static const Type* guessType(oatpp::parser::Caret& caret);
private:

  template<class T>
  static oatpp::Void deserializeInt(Deserializer* deserializer, parser::Caret& caret, const Type* const type){

    (void) deserializer;
    (void) type;

    if(caret.isAtText("null", true)){
      return oatpp::Void(T::Class::getType());
    } else {
      //TODO: shall we handle overflow cases like
      // oatpp::String json = "128";
      // auto value = jsonObjectMapper->readFromString<oatpp::Int8>(json); // UInt8 will overflow to -128
      return T(static_cast<typename T::UnderlyingType>(caret.parseInt()));
    }

  }

  template<class T>
  static oatpp::Void deserializeUInt(Deserializer* deserializer, parser::Caret& caret, const Type* const type){

    (void) deserializer;
    (void) type;

    if(caret.isAtText("null", true)){
      return oatpp::Void(T::Class::getType());
    } else {
      //TODO: shall we handle overflow cases like
      // oatpp::String json = "256";
      // auto value = jsonObjectMapper->readFromString<oatpp::UInt8>(json); // UInt8 will overflow to 0
      return T(static_cast<typename T::UnderlyingType>(caret.parseUnsignedInt()));
    }

  }

  static oatpp::Void deserializeFloat32(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static oatpp::Void deserializeFloat64(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static oatpp::Void deserializeBoolean(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static oatpp::Void deserializeString(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static oatpp::Void deserializeAny(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static oatpp::Void deserializeEnum(Deserializer* deserializer, parser::Caret& caret, const Type* const type);

  static oatpp::Void deserializeCollection(Deserializer* deserializer, parser::Caret& caret, const Type* type);
  static oatpp::Void deserializeMap(Deserializer* deserializer, parser::Caret& caret, const Type* const type);

  static oatpp::Void deserializeObject(Deserializer* deserializer, parser::Caret& caret, const Type* const type);

private:
  std::shared_ptr<Config> m_config;
  std::vector<DeserializerMethod> m_methods;
public:

  /**
   * Constructor.
   * @param config
   */
  Deserializer(const std::shared_ptr<Config>& config = std::make_shared<Config>());

  /**
   * Set deserializer method for type.
   * @param classId - &id:oatpp::data::mapping::type::ClassId;.
   * @param method - `typedef oatpp::Void (*DeserializerMethod)(Deserializer*, parser::Caret&, const Type* const)`.
   */
  void setDeserializerMethod(const data::mapping::type::ClassId& classId, DeserializerMethod method);

  /**
   * Deserialize text.
   * @param caret - &id:oatpp::parser::Caret;.
   * @param type - &id:oatpp::data::mapping::type::Type;
   * @return - `oatpp::Void` over deserialized object.
   */
  oatpp::Void deserialize(parser::Caret& caret, const Type* const type);

  /**
   * Get deserializer config.
   * @return
   */
  const std::shared_ptr<Config>& getConfig();

};

}}}}

#endif /* oatpp_parser_json_mapping_Deserializer_hpp */
