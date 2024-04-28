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

#ifndef oatpp_data_TreeToObjectMapper_hpp
#define oatpp_data_TreeToObjectMapper_hpp

#include "./Tree.hpp"

namespace oatpp { namespace data {

class TreeToObjectMapper : public base::Countable {
public:

  struct Config {
    bool allowUnknownFields = true;
    std::vector<std::string> enabledInterpretations = {};
  };

public:

  struct MappingState {

    Config* config;
    const Tree* tree;
    std::list<oatpp::String> errorStack;

    oatpp::String errorStacktrace() const;

  };

public:
  typedef oatpp::Void (*MapperMethod)(TreeToObjectMapper*, MappingState&, const Type* const);
public:

  template<class T>
  static oatpp::Void mapPrimitive(TreeToObjectMapper* mapper, MappingState& state, const Type* const type){
    (void) mapper;
    (void) type;
    if(state.tree->isNull()) {
      return oatpp::Void(T::Class::getType());
    }
    if(!state.tree->isPrimitive()) {
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapPrimitive()]: Value is NOT a Primitive type");
      return nullptr;
    }
    return T(state.tree->operator typename T::UnderlyingType());
  }

  static const Type* guessType(const Tree& node);

  static oatpp::Void mapString(TreeToObjectMapper* mapper, MappingState& state, const Type* const type);
  static oatpp::Void mapAny(TreeToObjectMapper* mapper, MappingState& state, const Type* const type);
  static oatpp::Void mapEnum(TreeToObjectMapper* mapper, MappingState& state, const Type* const type);

  static oatpp::Void mapCollection(TreeToObjectMapper* mapper, MappingState& state, const Type* type);
  static oatpp::Void mapMap(TreeToObjectMapper* mapper, MappingState& state, const Type* const type);

  static oatpp::Void mapObject(TreeToObjectMapper* mapper, MappingState& state, const Type* const type);

private:
  std::vector<MapperMethod> m_methods;
public:

  TreeToObjectMapper();

  void setMapperMethod(const data::mapping::type::ClassId& classId, MapperMethod method);

  oatpp::Void map(MappingState& state, const Type* const type);

};

}}

#endif //oatpp_data_TreeToObjectMapper_hpp
