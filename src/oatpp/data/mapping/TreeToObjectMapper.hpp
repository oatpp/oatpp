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

#ifndef oatpp_data_mapping_TreeToObjectMapper_hpp
#define oatpp_data_mapping_TreeToObjectMapper_hpp

#include "./Tree.hpp"
#include "./ObjectMapper.hpp"

#include "oatpp/utils/Conversion.hpp"

namespace oatpp { namespace data { namespace mapping {

class TreeToObjectMapper : public base::Countable {
public:

  struct Config {
    bool allowUnknownFields = true;
    bool allowLexicalCasting = false;
    bool useUnqualifiedFieldNames = false;
    bool useUnqualifiedEnumNames = false;
    std::vector<std::string> enabledInterpretations = {};

    /**
     * Pointer to anything extra that might be useful in mapper-method.
     */
    void* extra = nullptr;
  };

public:

  enum class GuessedPrimitiveType : v_uint32 {
    NOT_PRIMITIVE = 0,
    BOOL_TRUE = 1,
    BOOL_FALSE = 2,
    INT = 3,
    FLOAT = 4
  };

public:

  struct State {

    const Config* config;
    const Tree* tree;
    ErrorStack errorStack;

  };

public:
  typedef oatpp::Void (*MapperMethod)(const TreeToObjectMapper*, State&, const Type* const);
public:

  static GuessedPrimitiveType guessedPrimitiveType(const oatpp::String& text);

  template<class T>
  static oatpp::Void mapPrimitive(const TreeToObjectMapper* mapper, State& state, const Type* const type){
    (void) mapper;
    (void) type;
    if(state.tree->isPrimitive()) {
      return T(state.tree->operator typename T::UnderlyingType());
    }
    if(state.tree->isNull()) {
      return oatpp::Void(T::Class::getType());
    }
    if(state.config->allowLexicalCasting && state.tree->isString()) {
      const auto& text = state.tree->getString();
      auto pt = guessedPrimitiveType(text);
      switch (pt) {
        case GuessedPrimitiveType::BOOL_TRUE:
          return T(static_cast<typename T::UnderlyingType>(true));
        case GuessedPrimitiveType::BOOL_FALSE:
          return T(static_cast<typename T::UnderlyingType>(false));
        case GuessedPrimitiveType::INT: {
          bool success;
          auto value = static_cast<typename T::UnderlyingType>(utils::Conversion::strToInt64(text, success));
          if(!success) break;
          return T(value);
        }
        case GuessedPrimitiveType::FLOAT: {
          bool success;
          auto value = static_cast<typename T::UnderlyingType>(utils::Conversion::strToFloat64(text, success));
          if(!success) break;
          return T(value);
        }

        case GuessedPrimitiveType::NOT_PRIMITIVE:
        default:
          break;
      }
    }
    state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapPrimitive()]: Value is NOT a Primitive type");
    return nullptr;
  }

  static const Type* guessType(const Tree& node);

  static oatpp::Void mapString(const TreeToObjectMapper* mapper, State& state, const Type* type);
  static oatpp::Void mapTree(const TreeToObjectMapper* mapper, State& state, const Type* type);
  static oatpp::Void mapAny(const TreeToObjectMapper* mapper, State& state, const Type* type);
  static oatpp::Void mapEnum(const TreeToObjectMapper* mapper, State& state, const Type* type);

  static oatpp::Void mapCollection(const TreeToObjectMapper* mapper, State& state, const Type* type);
  static oatpp::Void mapMap(const TreeToObjectMapper* mapper, State& state, const Type* type);

  static oatpp::Void mapObject(const TreeToObjectMapper* mapper, State& state, const Type* type);

private:
  std::vector<MapperMethod> m_methods;
public:

  TreeToObjectMapper();

  void setMapperMethod(const data::type::ClassId& classId, MapperMethod method);

  oatpp::Void map(State& state, const Type* type) const;

};

}}}

#endif //oatpp_data_mapping_TreeToObjectMapper_hpp
