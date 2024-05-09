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

#include "ObjectRemapper.hpp"

namespace oatpp { namespace data { namespace mapping {

oatpp::Void ObjectRemapper::remap(const oatpp::Void& polymorph, const oatpp::Type* toType, ErrorStack& errorStack) const {

  Tree tree;

  {
    ObjectToTreeMapper::State state;
    state.tree = &tree;
    state.config = &m_objectToTreeConfig;
    m_objectToTreeMapper.map(state, polymorph);
    if(!state.errorStack.empty()) {
      errorStack = std::move(state.errorStack);
      return nullptr;
    }
  }

  /* if expected type is Tree (root element is Tree) - then we can just move deserialized tree */
  if(toType == data::type::Tree::Class::getType()) {
    return oatpp::Tree(std::move(tree));
  }

  {
    TreeToObjectMapper::State state;
    state.tree = &tree;
    state.config = &m_treeToObjectConfig;
    const auto & result = m_treeToObjectMapper.map(state, toType);
    if(!state.errorStack.empty()) {
      errorStack = std::move(state.errorStack);
      return nullptr;
    }
    return result;
  }

}

ObjectToTreeMapper::Config& ObjectRemapper::objectToTreeConfig() {
  return m_objectToTreeConfig;
}

TreeToObjectMapper::Config& ObjectRemapper::treeToObjectConfig() {
  return m_treeToObjectConfig;
}

ObjectToTreeMapper& ObjectRemapper::objectToTreeMapper() {
  return m_objectToTreeMapper;
}

TreeToObjectMapper& ObjectRemapper::treeToObjectMapper() {
  return m_treeToObjectMapper;
}

const ObjectToTreeMapper::Config& ObjectRemapper::objectToTreeConfig() const {
  return m_objectToTreeConfig;
}

const TreeToObjectMapper::Config& ObjectRemapper::treeToObjectConfig() const {
  return m_treeToObjectConfig;
}

const ObjectToTreeMapper& ObjectRemapper::objectToTreeMapper() const {
  return m_objectToTreeMapper;
}

const TreeToObjectMapper& ObjectRemapper::treeToObjectMapper() const {
  return m_treeToObjectMapper;
}

}}}
