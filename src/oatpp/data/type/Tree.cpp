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

#include "./Tree.hpp"
#include "oatpp/data/mapping/Tree.hpp"

namespace oatpp { namespace data { namespace type {

namespace __class {

const ClassId Tree::CLASS_ID("Tree");

Type* Tree::getType() {
  static Type type(CLASS_ID);
  return &type;
}

}

Tree::Tree()
  : ObjectWrapper<mapping::Tree, __class::Tree>()
{}

Tree::Tree(std::nullptr_t)
  : ObjectWrapper<mapping::Tree, __class::Tree>()
{}

/**
 * Copy constructor.
 * @param other - other Any.
 */
Tree::Tree(const Tree& other)
  : ObjectWrapper<mapping::Tree, __class::Tree>(other)
{}

/**
 * Move constructor.
 * @param other
 */
Tree::Tree(Tree&& other)
  : ObjectWrapper<mapping::Tree, __class::Tree>(std::forward<ObjectWrapper<mapping::Tree, __class::Tree>>(other))
{}

Tree::Tree(const mapping::Tree& other)
  : ObjectWrapper<mapping::Tree, __class::Tree>(std::make_shared<mapping::Tree>(other))
{}

Tree::Tree(mapping::Tree&& other)
  : ObjectWrapper<mapping::Tree, __class::Tree>(std::make_shared<mapping::Tree>(std::forward<mapping::Tree>(other)))
{}

Tree::Tree(const std::shared_ptr<mapping::Tree>& node, const Type* const type)
  : ObjectWrapper<mapping::Tree, __class::Tree>(node, type)
{}

Tree& Tree::operator = (std::nullptr_t) {
  m_ptr.reset();
  return *this;
}

Tree& Tree::operator = (const Tree& other) {
  m_ptr = other.m_ptr;
  return *this;
}

Tree& Tree::operator = (Tree&& other) {
  m_ptr = std::move(other.m_ptr);
  return *this;
}

Tree& Tree::operator = (const mapping::Tree& other) {
  if(m_ptr) {
    *m_ptr = other;
  } else {
    m_ptr = std::make_shared<mapping::Tree>(other);
  }
  return *this;
}

Tree& Tree::operator = (mapping::Tree&& other) {
  if(m_ptr) {
    *m_ptr = std::forward<mapping::Tree>(std::forward<mapping::Tree>(other));
  } else {
    m_ptr = std::make_shared<mapping::Tree>(std::forward<mapping::Tree>(other));
  }
  return *this;
}

bool Tree::operator == (std::nullptr_t) const {
  return m_ptr.get() == nullptr;
}

bool Tree::operator != (std::nullptr_t) const {
  return m_ptr.get() != nullptr;
}

bool Tree::operator == (const Tree& other) const {
  return m_ptr.get() == other.m_ptr.get();
}

bool Tree::operator != (const Tree& other) const {
  return !operator == (other);
}

mapping::Tree* Tree::operator->() {
  if(!m_ptr) {
    m_ptr = std::make_shared<mapping::Tree>();
  }
  return m_ptr.get();
}

mapping::Tree* Tree::operator->() const {
  if(!m_ptr) {
    throw std::runtime_error("[oatpp::data::type::Tree::operator ->()]: null-pointer exception");
  }
  return m_ptr.get();
}

mapping::Tree& Tree::operator*() {
  if(!m_ptr) {
    m_ptr = std::make_shared<mapping::Tree>();
  }
  return *m_ptr;
}

const mapping::Tree& Tree::operator*() const {
  if(!m_ptr) {
    throw std::runtime_error("[oatpp::data::type::Tree::operator *()]: null-pointer exception");
  }
  return *m_ptr;
}

mapping::Tree& Tree::operator [] (const String& key) {
  if(!m_ptr) {
    m_ptr = std::make_shared<mapping::Tree>();
  }
  return (*m_ptr)[key];
}

const mapping::Tree& Tree::operator [] (const String& key) const {
  if(!m_ptr) {
    throw std::runtime_error("[oatpp::data::type::Tree::operator []]: null-pointer exception");
  }
  return (*m_ptr)[key];
}

mapping::Tree& Tree::operator [] (v_uint64 index) {
  if(!m_ptr) {
    m_ptr = std::make_shared<mapping::Tree>();
  }
  return (*m_ptr)[index];
}

const mapping::Tree& Tree::operator [] (v_uint64 index) const {
  if(!m_ptr) {
    throw std::runtime_error("[oatpp::data::type::Tree::operator []]: null-pointer exception");
  }
  return (*m_ptr)[index];
}

}}}
