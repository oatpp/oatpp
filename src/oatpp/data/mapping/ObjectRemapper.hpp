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

#ifndef oatpp_data_mapping_ObjectRemapper_hpp
#define oatpp_data_mapping_ObjectRemapper_hpp

#include "./ObjectToTreeMapper.hpp"
#include "./TreeToObjectMapper.hpp"

#include "oatpp/Types.hpp"

namespace oatpp { namespace data { namespace mapping {

class ObjectRemapper {
protected:
  ObjectToTreeMapper::Config m_objectToTreeConfig;
  TreeToObjectMapper::Config m_treeToObjectConfig;
  ObjectToTreeMapper m_objectToTreeMapper;
  TreeToObjectMapper m_treeToObjectMapper;
public:

  ObjectRemapper() = default;
  virtual ~ObjectRemapper() = default;

  oatpp::Void remap(const Tree& tree, const oatpp::Type* toType, ErrorStack& errorStack) const;

  template<class Wrapper>
  Wrapper remap(const Tree& tree, ErrorStack& errorStack) const {
    auto toType = Wrapper::Class::getType();
    return remap(tree, toType, errorStack).template cast<Wrapper>();
  }

  template<class Wrapper>
  Wrapper remap(const Tree& tree) const {
    auto toType = Wrapper::Class::getType();
    ErrorStack errorStack;
    const auto& result = remap(tree, toType, errorStack).template cast<Wrapper>();
    if(!errorStack.empty()) {
      throw MappingError(std::move(errorStack));
    }
    return result;
  }

  oatpp::Void remap(const oatpp::Void& polymorph, const oatpp::Type* toType, ErrorStack& errorStack) const;

  template<class Wrapper>
  Wrapper remap(const oatpp::Void& polymorph, ErrorStack& errorStack) const {
    auto toType = Wrapper::Class::getType();
    return remap(polymorph, toType, errorStack).template cast<Wrapper>();
  }

  template<class Wrapper>
  Wrapper remap(const oatpp::Void& polymorph) const {
    auto toType = Wrapper::Class::getType();
    ErrorStack errorStack;
    const auto& result = remap(polymorph, toType, errorStack).template cast<Wrapper>();
    if(!errorStack.empty()) {
      throw MappingError(std::move(errorStack));
    }
    return result;
  }

  ObjectToTreeMapper::Config& objectToTreeConfig();
  TreeToObjectMapper::Config& treeToObjectConfig();

  ObjectToTreeMapper& objectToTreeMapper();
  TreeToObjectMapper& treeToObjectMapper();

  const ObjectToTreeMapper::Config& objectToTreeConfig() const;
  const TreeToObjectMapper::Config& treeToObjectConfig() const;

  const ObjectToTreeMapper& objectToTreeMapper() const;
  const TreeToObjectMapper& treeToObjectMapper() const;

};

}}}

#endif // oatpp_data_mapping_ObjectRemapper_hpp
