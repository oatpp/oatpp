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

#ifndef oatpp_data_mapping_ObjectToTreeMapper_hpp
#define oatpp_data_mapping_ObjectToTreeMapper_hpp

#include "./Tree.hpp"

namespace oatpp { namespace data { namespace mapping {

class ObjectToTreeMapper : public base::Countable {
public:

  struct Config {
    bool includeNullFields = true;
    bool alwaysIncludeRequired = false;
    bool alwaysIncludeNullCollectionElements = false;
    std::vector<std::string> enabledInterpretations = {};
  };

public:

  struct MappingState {

    Config* config;
    Tree* tree;
    std::list<oatpp::String> errorStack;

    oatpp::String errorStacktrace() const;

  };

public:
  typedef void (*MapperMethod)(const ObjectToTreeMapper*, MappingState&, const oatpp::Void&);
public:

  template<class T>
  static void mapPrimitive(const ObjectToTreeMapper* mapper, MappingState& state, const oatpp::Void& polymorph){
    (void) mapper;
    if(polymorph){
      state.tree->setValue<typename T::ObjectType>(* static_cast<typename T::ObjectType*>(polymorph.get()));
    } else {
      state.tree->setNull();
    }
  }

  static void mapString(const ObjectToTreeMapper* mapper, MappingState& state, const oatpp::Void& polymorph);
  static void mapAny(const ObjectToTreeMapper* mapper, MappingState& state, const oatpp::Void& polymorph);
  static void mapEnum(const ObjectToTreeMapper* mapper, MappingState& state, const oatpp::Void& polymorph);

  static void mapCollection(const ObjectToTreeMapper* mapper, MappingState& state, const oatpp::Void& polymorph);
  static void mapMap(const ObjectToTreeMapper* mapper, MappingState& state, const oatpp::Void& polymorph);

  static void mapObject(const ObjectToTreeMapper* mapper, MappingState& state, const oatpp::Void& polymorph);

private:
  std::vector<MapperMethod> m_methods;
public:

  ObjectToTreeMapper();

  void setMapperMethod(const data::mapping::type::ClassId& classId, MapperMethod method);

  void map(MappingState& state, const oatpp::Void& polymorph) const;

};

}}}

#endif //oatpp_data_mapping_ObjectToTreeMapper_hpp
