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

#include "Tree.hpp"

namespace oatpp { namespace data {

Tree::Node::Node()
  : m_type(Type::NULL_VALUE)
  , m_data(0)
{}

Tree::Node::Node(const Node& other)
  : Node()
{
  setCopy(other);
}

Tree::Node::Node(Node&& other) noexcept
  : m_type(other.m_type)
  , m_data(other.m_data)
{
  other.m_type = Type::NULL_VALUE;
  other.m_data = 0;
}

Tree::Node::~Node() {
  deleteValueObject();
}

Tree::Node& Tree::Node::operator = (const Node& other) {
  setCopy(other);
  return *this;
}

Tree::Node& Tree::Node::operator = (Node&& other) noexcept {
  setMove(std::forward<Node>(other));
  return *this;
}

void Tree::Node::deleteValueObject() {

  switch (m_type) {

    case Type::NULL_VALUE:

    case Type::INTEGER:
    case Type::FLOAT:

    case Type::BOOL:

    case Type::INT_8:
    case Type::UINT_8:
    case Type::INT_16:
    case Type::UINT_16:
    case Type::INT_32:
    case Type::UINT_32:
    case Type::INT_64:
    case Type::UINT_64:

    case Type::FLOAT_32:
    case Type::FLOAT_64:
      break;

    case Type::STRING: {
      auto data = reinterpret_cast<oatpp::String *>(m_data);
      delete data;
      break;
    }
    case Type::VECTOR: {
      auto data = reinterpret_cast<std::vector<Node> *>(m_data);
      delete data;
      break;
    }
    case Type::MAP: {
      auto data = reinterpret_cast<std::vector<std::pair<oatpp::String, Node>> *>(m_data);
      delete data;
      break;
    }

    default:
      // DO-NOTHING
      break;

  }
}

Tree::Node::Type Tree::Node::getType() const {
  return m_type;
}

void Tree::Node::setCopy(const Node& other) {

  deleteValueObject();
  m_type = other.m_type;

  switch (other.m_type) {

    case Type::NULL_VALUE:
      break;

    case Type::INTEGER:
    case Type::FLOAT:

    case Type::BOOL:
    case Type::INT_8:
    case Type::UINT_8:
    case Type::INT_16:
    case Type::UINT_16:
    case Type::INT_32:
    case Type::UINT_32:
    case Type::INT_64:
    case Type::UINT_64:
    case Type::FLOAT_32:
    case Type::FLOAT_64:
    {
      m_data = other.m_data;
      break;
    }

    case Type::STRING: {
      auto otherData = reinterpret_cast<oatpp::String *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::Tree::Node::setCopy()]: other.data is null, other.type is 'STRING'");
      }
      auto ptr = new oatpp::String(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }
    case Type::VECTOR: {
      auto otherData = reinterpret_cast<std::vector<Node> *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::Tree::Node::setCopy()]: other.data is null, other.type is 'VECTOR'");
      }
      auto ptr = new std::vector<Node>(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }
    case Type::MAP: {
      auto otherData = reinterpret_cast<std::vector<std::pair<oatpp::String, Node>> *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::Tree::Node::setCopy()]: other.data is null, other.type is 'MAP'");
      }
      auto ptr = new std::vector<std::pair<oatpp::String, Node>>(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }

    default:
      m_data = other.m_data;
      break;

  }

}

void Tree::Node::setMove(Node&& other) {
  deleteValueObject();
  m_type = other.m_type;
  m_data = other.m_data;
  other.m_type = Type::NULL_VALUE;
  other.m_data = 0;
}

void Tree::Node::setNull() {
  deleteValueObject();
  m_type = Type::NULL_VALUE;
  m_data = 0;
}

void Tree::Node::setInteger(v_int64 value) {
  deleteValueObject();
  m_type = Type::INTEGER;
  std::memcpy (&m_data, &value, sizeof(v_int64));
}

void Tree::Node::setFloat(v_float64 value) {
  deleteValueObject();
  m_type = Type::FLOAT;
  std::memcpy (&m_data, &value, sizeof(v_float64));
}

void Tree::Node::setString(const oatpp::String& value) {
  deleteValueObject();
  m_type = Type::STRING;
  auto data = new oatpp::String(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::Node::setVector(const std::vector<Node>& value) {
  deleteValueObject();
  m_type = Type::VECTOR;
  auto data = new std::vector<Node>(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::Node::setMap(const std::vector<std::pair<oatpp::String, Node>>& value) {
  deleteValueObject();
  m_type = Type::MAP;
  auto data = new std::vector<std::pair<oatpp::String, Node>>(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

bool Tree::Node::isNull() const {
  return m_type == Type::NULL_VALUE;
}

v_int64 Tree::Node::getInteger() const {
  if(m_type != Type::INTEGER) {
    throw std::runtime_error("[oatpp::data::Tree::Node::getInteger()]: NOT an arbitrary INTEGER.");
  }
  v_int64 result;
  std::memcpy (&result, &m_data, sizeof(v_float64));
  return result;
}

v_float64 Tree::Node::getFloat() const {
  if(m_type != Type::FLOAT) {
    throw std::runtime_error("[oatpp::data::Tree::Node::getInteger()]: NOT an arbitrary FLOAT.");
  }
  v_float64 result;
  std::memcpy (&result, &m_data, sizeof(v_float64));
  return result;
}

const oatpp::String& Tree::Node::getString() const {
  if(m_type != Type::STRING) {
    throw std::runtime_error("[oatpp::data::Tree::Node::getInteger()]: NOT a STRING.");
  }
  auto data = reinterpret_cast<const oatpp::String*>(m_data);
  return *data;
}

const std::vector<Tree::Node>& Tree::Node::getVector() const {
  if(m_type != Type::VECTOR) {
    throw std::runtime_error("[oatpp::data::Tree::Node::getInteger()]: NOT a VECTOR.");
  }
  auto data = reinterpret_cast<const std::vector<Tree::Node>*>(m_data);
  return *data;
}

const std::vector<std::pair<oatpp::String, Tree::Node>>& Tree::Node::getMap() const {
  if(m_type != Type::MAP) {
    throw std::runtime_error("[oatpp::data::Tree::Node::getInteger()]: NOT a MAP.");
  }
  auto data = reinterpret_cast<const std::vector<std::pair<oatpp::String, Tree::Node>>*>(m_data);
  return *data;
}

std::vector<Tree::Node>& Tree::Node::getVector() {
  if(m_type != Type::VECTOR) {
    throw std::runtime_error("[oatpp::data::Tree::Node::getInteger()]: NOT a VECTOR.");
  }
  auto data = reinterpret_cast<std::vector<Tree::Node>*>(m_data);
  return *data;
}

std::vector<std::pair<oatpp::String, Tree::Node>>& Tree::Node::getMap() {
  if(m_type != Type::MAP) {
    throw std::runtime_error("[oatpp::data::Tree::Node::getInteger()]: NOT a MAP.");
  }
  auto data = reinterpret_cast<std::vector<std::pair<oatpp::String, Tree::Node>>*>(m_data);
  return *data;
}

}}
