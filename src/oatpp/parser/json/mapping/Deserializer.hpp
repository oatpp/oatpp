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

#include "oatpp/core/data/mapping/type/ListMap.hpp"
#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"

#include "oatpp/core/parser/Caret.hpp"

#include "oatpp/core/collection/LinkedList.hpp"
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
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;

  typedef oatpp::data::mapping::type::Object Object;
  typedef oatpp::String String;

  template<class T>
  using PolymorphicWrapper = data::mapping::type::PolymorphicWrapper<T>;

  typedef oatpp::data::mapping::type::AbstractObjectWrapper AbstractObjectWrapper;
  typedef oatpp::data::mapping::type::List<AbstractObjectWrapper> AbstractList;
  typedef oatpp::data::mapping::type::ListMap<String, AbstractObjectWrapper> AbstractFieldsMap;
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

  };

public:
  typedef AbstractObjectWrapper (*DeserializerMethod)(Deserializer*, parser::Caret&, const Type* const);
private:
  static void skipScope(oatpp::parser::Caret& caret, v_char8 charOpen, v_char8 charClose);
  static void skipString(oatpp::parser::Caret& caret);
  static void skipToken(oatpp::parser::Caret& caret);
  static void skipValue(oatpp::parser::Caret& caret);
private:

  template<class T>
  static AbstractObjectWrapper deserializeInt(Deserializer* deserializer, parser::Caret& caret, const Type* const type){

    (void) deserializer;
    (void) type;

    if(caret.isAtText("null", true)){
      return AbstractObjectWrapper(T::Class::getType());
    } else {
      return AbstractObjectWrapper(T::ObjectType::createAbstract((typename T::ObjectType::ValueType) caret.parseInt()), T::ObjectWrapper::Class::getType());
    }

  }

  template<class T>
  static AbstractObjectWrapper deserializeUInt(Deserializer* deserializer, parser::Caret& caret, const Type* const type){

    (void) deserializer;
    (void) type;

    if(caret.isAtText("null", true)){
      return AbstractObjectWrapper(T::Class::getType());
    } else {
      return AbstractObjectWrapper(T::ObjectType::createAbstract((typename T::ObjectType::ValueType) caret.parseUnsignedInt()), T::ObjectWrapper::Class::getType());
    }

  }

  static AbstractObjectWrapper deserializeFloat32(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static AbstractObjectWrapper deserializeFloat64(Deserializer* deserializer, parser::Caret& caret, const Type* const type);

  static AbstractObjectWrapper deserializeBoolean(Deserializer* deserializer, parser::Caret& caret, const Type* const type);

  static AbstractObjectWrapper deserializeString(Deserializer* deserializer, parser::Caret& caret, const Type* const type);

  static AbstractObjectWrapper deserializeList(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static AbstractObjectWrapper deserializeFieldsMap(Deserializer* deserializer, parser::Caret& caret, const Type* const type);
  static AbstractObjectWrapper deserializeObject(Deserializer* deserializer, parser::Caret& caret, const Type* const type);

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
   * @param method - `typedef AbstractObjectWrapper (*DeserializerMethod)(Deserializer*, parser::Caret&, const Type* const)`.
   */
  void setDeserializerMethod(const data::mapping::type::ClassId& classId, DeserializerMethod method);

  /**
   * Deserialize text.
   * @param caret - &id:oatpp::parser::Caret;.
   * @param type - &id:oatpp::data::mapping::type::Type;
   * @return - `AbstractObjectWrapper` over deserialized object.
   */
  AbstractObjectWrapper deserialize(parser::Caret& caret, const Type* const type);

  /**
   * Get deserializer config.
   * @return
   */
  const std::shared_ptr<Config>& getConfig();

};

}}}}

#endif /* oatpp_parser_json_mapping_Deserializer_hpp */
