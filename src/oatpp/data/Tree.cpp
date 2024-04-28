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

#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace data {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tree::Map

Tree& Tree::Map::operator [] (const oatpp::String& key) {
  auto it = m_map.find(key);
  if(it == m_map.end()) {
    auto& result = m_map[key];
    m_order.push_back(key);
    return result;
  }
  return it->second;
}

const Tree& Tree::Map::operator [] (const oatpp::String& key) const {
  auto it = m_map.find(key);
  if(it == m_map.end()) {
    throw std::runtime_error("[oatpp::data::Tree::Map::operator[]]: const operator[] can't add items.");
  }
  return it->second;
}

std::pair<oatpp::String, std::reference_wrapper<Tree>> Tree::Map::operator [] (v_uint64 index) {
  auto& key = m_order.at(index);
  return {key, m_map[key]};
}

std::pair<oatpp::String, std::reference_wrapper<const Tree>> Tree::Map::operator [] (v_uint64 index) const {
  auto& key = m_order.at(index);
  return {key, m_map.at(key)};
}

v_uint64 Tree::Map::size() const {
  return m_map.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tree

Tree::Tree()
  : m_type(Type::UNDEFINED)
  , m_data(0)
{}

Tree::Tree(const Tree& other)
  : Tree()
{
  setCopy(other);
}

Tree::Tree(Tree&& other) noexcept
  : m_type(other.m_type)
  , m_data(other.m_data)
{
  other.m_type = Type::UNDEFINED;
  other.m_data = 0;
}

Tree::~Tree() {
  deleteValueObject();
}

Tree& Tree::operator = (const Tree& other) {
  setCopy(other);
  return *this;
}

Tree& Tree::operator = (Tree&& other) noexcept {
  setMove(std::forward<Tree>(other));
  return *this;
}

void Tree::deleteValueObject() {

  switch (m_type) {

    case Type::UNDEFINED:

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
      auto data = reinterpret_cast<std::vector<Tree> *>(m_data);
      delete data;
      break;
    }
    case Type::MAP: {
      auto data = reinterpret_cast<Map *>(m_data);
      delete data;
      break;
    }

    default:
      // DO-NOTHING
      break;

  }
}

Tree::operator oatpp::String () {
  return getString();
}

Tree& Tree::operator [] (const oatpp::String& key) {
  return getMap()[key];
}

const Tree& Tree::operator [] (const oatpp::String& key) const {
  return getMap()[key];
}

Tree& Tree::operator [] (v_uint64 index) {
  return getVector().at(index);
}

const Tree& Tree::operator [] (v_uint64 index) const {
  return getVector().at(index);
}

Tree::Type Tree::getType() const {
  return m_type;
}

void Tree::setCopy(const Tree& other) {

  deleteValueObject();
  m_type = other.m_type;

  switch (other.m_type) {

    case Type::UNDEFINED:
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
        throw std::runtime_error("[oatpp::data::Tree::setCopy()]: other.data is null, other.type is 'STRING'");
      }
      auto ptr = new oatpp::String(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }
    case Type::VECTOR: {
      auto otherData = reinterpret_cast<std::vector<Tree> *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::Tree::setCopy()]: other.data is null, other.type is 'VECTOR'");
      }
      auto ptr = new std::vector<Tree>(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }
    case Type::MAP: {
      auto otherData = reinterpret_cast<Map *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::Tree::setCopy()]: other.data is null, other.type is 'MAP'");
      }
      auto ptr = new Map(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }

    default:
      m_data = other.m_data;
      break;

  }

}

void Tree::setMove(Tree&& other) {
  deleteValueObject();
  m_type = other.m_type;
  m_data = other.m_data;
  other.m_type = Type::NULL_VALUE;
  other.m_data = 0;
}

void Tree::setNull() {
  deleteValueObject();
  m_type = Type::NULL_VALUE;
  m_data = 0;
}

void Tree::setUndefined() {
  deleteValueObject();
  m_type = Type::UNDEFINED;
  m_data = 0;
}

void Tree::setInteger(v_int64 value) {
  deleteValueObject();
  m_type = Type::INTEGER;
  std::memcpy (&m_data, &value, sizeof(v_int64));
}

void Tree::setFloat(v_float64 value) {
  deleteValueObject();
  m_type = Type::FLOAT;
  std::memcpy (&m_data, &value, sizeof(v_float64));
}

void Tree::setString(const oatpp::String& value) {
  deleteValueObject();
  m_type = Type::STRING;
  auto data = new oatpp::String(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setVector(const std::vector<Tree>& value) {
  deleteValueObject();
  m_type = Type::VECTOR;
  auto data = new std::vector<Tree>(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setVector(v_uint64 size) {
  deleteValueObject();
  m_type = Type::VECTOR;
  auto data = new std::vector<Tree>(size);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setMap(const Map& value) {
  deleteValueObject();
  m_type = Type::MAP;
  auto data = new Map(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

bool Tree::isNull() const {
  return m_type == Type::NULL_VALUE;
}

bool Tree::isUndefined() const {
  return m_type == Type::UNDEFINED;
}

bool Tree::isPrimitive() const {
  switch (m_type) {

    case Type::UNDEFINED:
    case Type::NULL_VALUE:
      return false;

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
      return true;

    case Type::STRING:
    case Type::VECTOR:
    case Type::MAP:
    default:
      return false;
  }

}

v_int32 Tree::primitiveDataSize() const {
  switch (m_type) {

    case Type::UNDEFINED:
    case Type::NULL_VALUE:
      return -1;

    case Type::INTEGER:
    case Type::FLOAT: return 8;

    case Type::BOOL:
    case Type::INT_8:
    case Type::UINT_8: return 1;

    case Type::INT_16:
    case Type::UINT_16: return 2;

    case Type::INT_32:
    case Type::UINT_32: return 4;

    case Type::INT_64:
    case Type::UINT_64: return 8;

    case Type::FLOAT_32: return 4;
    case Type::FLOAT_64: return 8;

    case Type::STRING:
    case Type::VECTOR:
    case Type::MAP:
    default:
      return -1;
  }
}

bool Tree::isFloatPrimitive() const {
  switch (m_type) {

    case Type::UNDEFINED:
    case Type::NULL_VALUE:
    case Type::INTEGER: return false;

    case Type::FLOAT: return true;

    case Type::BOOL:
    case Type::INT_8:
    case Type::UINT_8:
    case Type::INT_16:
    case Type::UINT_16:
    case Type::INT_32:
    case Type::UINT_32:
    case Type::INT_64:
    case Type::UINT_64: return false;

    case Type::FLOAT_32:
    case Type::FLOAT_64: return true;

    case Type::STRING:
    case Type::VECTOR:
    case Type::MAP:
    default:
      return false;
  }
}

bool Tree::isIntPrimitive() const {
  switch (m_type) {

    case Type::UNDEFINED:
    case Type::NULL_VALUE:
      return false;

    case Type::INTEGER: return true;
    case Type::FLOAT: return false;

    case Type::BOOL:
    case Type::INT_8:
    case Type::UINT_8:
    case Type::INT_16:
    case Type::UINT_16:
    case Type::INT_32:
    case Type::UINT_32:
    case Type::INT_64:
    case Type::UINT_64: return true;

    case Type::FLOAT_32:
    case Type::FLOAT_64:

    case Type::STRING:
    case Type::VECTOR:
    case Type::MAP:
    default:
      return false;
  }
}

v_int64 Tree::getInteger() const {
  if(m_type != Type::INTEGER) {
    throw std::runtime_error("[oatpp::data::Tree::getInteger()]: NOT an arbitrary INTEGER.");
  }
  v_int64 result;
  std::memcpy (&result, &m_data, sizeof(v_float64));
  return result;
}

v_float64 Tree::getFloat() const {
  if(m_type != Type::FLOAT) {
    throw std::runtime_error("[oatpp::data::Tree::getFloat()]: NOT an arbitrary FLOAT.");
  }
  v_float64 result;
  std::memcpy (&result, &m_data, sizeof(v_float64));
  return result;
}

const oatpp::String& Tree::getString() const {
  if(m_type != Type::STRING) {
    throw std::runtime_error("[oatpp::data::Tree::getString()]: NOT a STRING.");
  }
  auto data = reinterpret_cast<const oatpp::String*>(m_data);
  return *data;
}

const std::vector<Tree>& Tree::getVector() const {
  if(m_type != Type::VECTOR) {
    throw std::runtime_error("[oatpp::data::Tree::getVector()]: NOT a VECTOR.");
  }
  auto data = reinterpret_cast<const std::vector<Tree>*>(m_data);
  return *data;
}

const Tree::Map& Tree::getMap() const {
  if(m_type != Type::MAP) {
    throw std::runtime_error("[oatpp::data::Tree::getMap()]: NOT a MAP.");
  }
  auto data = reinterpret_cast<const Map*>(m_data);
  return *data;
}

std::vector<Tree>& Tree::getVector() {
  if(m_type == Type::UNDEFINED) {
    setVector({});
  }
  if(m_type != Type::VECTOR) {
    throw std::runtime_error("[oatpp::data::Tree::getVector()]: NOT a VECTOR.");
  }
  auto data = reinterpret_cast<std::vector<Tree>*>(m_data);
  return *data;
}

Tree::Map& Tree::getMap() {
  if(m_type == Type::UNDEFINED) {
    setMap({});
  }
  if(m_type != Type::MAP) {
    throw std::runtime_error("[oatpp::data::Tree::getMap()]: NOT a MAP.");
  }
  auto data = reinterpret_cast<Map*>(m_data);
  return *data;
}

oatpp::String Tree::debugPrint(v_uint32 indent0, v_uint32 indentDelta, bool firstLineIndent) const {

  stream::BufferOutputStream ss;
  for(v_uint32 i = 0; i < indent0; i ++) {
    ss << " ";
  }
  oatpp::String indentStr0 = ss.toString();

  ss.setCurrentPosition(0);
  for(v_uint32 i = 0; i < indentDelta; i ++) {
    ss << " ";
  }
  oatpp::String indentDeltaStr = ss.toString();

  ss.setCurrentPosition(0);
  if(firstLineIndent) {
    ss << indentStr0;
  }


  switch (m_type) {

    case Type::UNDEFINED: {
      ss << "undefined";
      break;
    }
    case Type::NULL_VALUE: {
      ss << "null";
      break;
    }

    case Type::INTEGER: {
      ss << getInteger() << " (integer)";
      break;
    }
    case Type::FLOAT: {
      ss << getFloat() << " (float)";
      break;
    }

    case Type::BOOL: {
      ss << getValue<bool>() << " (bool)";
      break;
    }
    case Type::INT_8: {
      ss << getValue<v_int8>() << " (int_8)";
      break;
    }

    case Type::UINT_8: {
      ss << getValue<v_uint8>() << " (uint_8)";
      break;
    }
    case Type::INT_16: {
      ss << getValue<v_int16>() << " (int_16)";
      break;
    }
    case Type::UINT_16: {
      ss << getValue<v_uint16>() << " (uint_16)";
      break;
    }
    case Type::INT_32: {
      ss << getValue<v_int32 >() << " (int_32)";
      break;
    }
    case Type::UINT_32: {
      ss << getValue<v_uint32>() << " (uint_32)";
      break;
    }
    case Type::INT_64: {
      ss << getValue<v_int64>() << " (int_64)";
      break;
    }
    case Type::UINT_64: {
      ss << getValue<v_uint64>() << " (uint_64)";
      break;
    }
    case Type::FLOAT_32: {
      ss << getValue<v_float32>() << " (float_32)";
      break;
    }
    case Type::FLOAT_64: {
      ss << getValue<v_float64>() << " (float_64)";
      break;
    }
    case Type::STRING: {
      ss << "'" << getString() << "'";
      break;
    }

    case Type::VECTOR: {
      ss << "[\n";
      auto& vector = getVector();
      for(auto& v : vector) {
        ss << v.debugPrint(indent0 + indentDelta, indentDelta) << "\n";
      }
      ss << indentStr0 << "]";
      break;
    }
    case Type::MAP: {
      ss << "{\n";
      auto& map = getMap();
      for(v_uint32 i = 0; i < map.size(); i ++) {
        const auto& node = map[i];
        ss << indentStr0 << indentDeltaStr << node.first << ": " << node.second.get().debugPrint(indent0 + indentDelta, indentDelta, false) << "\n";
      }
      ss << indentStr0 << "}";
      break;
    }

    default:
      break;
  }

  return ss.toString();

}

}}
