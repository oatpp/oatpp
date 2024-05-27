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

namespace oatpp { namespace data { namespace mapping {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tree::Attributes

Tree::Attributes::Attributes()
  : m_attributes(nullptr)
{}

Tree::Attributes::Attributes(const Attributes& other)
  : Attributes()
{
  operator=(other);
}

Tree::Attributes::Attributes(Attributes&& other) noexcept
  : m_attributes(other.m_attributes)
{
  other.m_attributes = nullptr;
}

Tree::Attributes& Tree::Attributes::operator = (const Attributes& other) {

  if(other.m_attributes) {

    if(m_attributes) {
      *m_attributes = *other.m_attributes;
    } else {
      m_attributes = new Attrs(*other.m_attributes);
    }

    for(auto & po : m_attributes->order){
      po.second = &m_attributes->map.at(po.first.lock());
    }

  } else {
    delete m_attributes;
    m_attributes = nullptr;
  }

  return *this;
}

Tree::Attributes& Tree::Attributes::operator = (Attributes&& other) noexcept {

  delete m_attributes;

  m_attributes = other.m_attributes;
  other.m_attributes = nullptr;

  return *this;
}

Tree::Attributes::~Attributes() {
  delete m_attributes;
  m_attributes = nullptr;
}

void Tree::Attributes::initAttributes() {
  if(m_attributes == nullptr)  {
    m_attributes = new Attrs();
  }
}

type::String& Tree::Attributes::operator [] (const type::String& key) {
  initAttributes();
  auto it = m_attributes->map.find(key);
  if(it == m_attributes->map.end()) {
    auto& result = m_attributes->map[key];
    m_attributes->order.emplace_back(key.getPtr(), &result);
    return result;
  }
  return it->second;
}

const type::String& Tree::Attributes::operator [] (const type::String& key) const {
  if(m_attributes != nullptr) {
    auto it = m_attributes->map.find(key);
    if (it != m_attributes->map.end()) {
      return it->second;
    }
  }
  throw std::runtime_error("[oatpp::data::mapping::Tree::Attributes::operator []]: const operator[] can't add items.");
}

std::pair<type::String, std::reference_wrapper<type::String>> Tree::Attributes::operator [] (v_uint64 index) {
  if(m_attributes != nullptr) {
    auto &item = m_attributes->order.at(index);
    return {item.first.lock(), *item.second};
  }
  throw std::runtime_error("[oatpp::data::mapping::Tree::Attributes::operator []]: const operator[] can't get item - empty attributes.");
}

std::pair<type::String, std::reference_wrapper<const type::String>> Tree::Attributes::operator [] (v_uint64 index) const {
  if(m_attributes != nullptr) {
    auto &item = m_attributes->order.at(index);
    return {item.first.lock(), *item.second};
  }
  throw std::runtime_error("[oatpp::data::mapping::Tree::Attributes::operator []]: const operator[] can't get item - empty attributes.");
}

type::String Tree::Attributes::get(const type::String& key) const {
  if(m_attributes == nullptr) return nullptr;
  auto it = m_attributes->map.find(key);
  if(it != m_attributes->map.end()) {
    return it->second;
  }
  return nullptr;
}

bool Tree::Attributes::empty() const {
  return m_attributes == nullptr || m_attributes->map.empty();
}

v_uint64 Tree::Attributes::size() const {
  if(m_attributes) {
    return m_attributes->map.size();
  }
  return 0;
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
  , m_attributes(std::move(other.m_attributes))
{
  other.m_type = Type::UNDEFINED;
  other.m_data = 0;
}

Tree::Tree(const type::String& value)
  : Tree()
{
  setString(value);
}

Tree::~Tree() {
  deleteValueObject();
}

Tree& Tree::operator = (const Tree& other) {
  setCopy(other);
  return *this;
}

Tree& Tree::operator = (Tree&& other) noexcept {
  setMove(std::move(other));
  return *this;
}

Tree& Tree::operator = (const type::String& value) {
  setString(value);
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
      auto data = reinterpret_cast<type::String *>(m_data);
      delete data;
      break;
    }
    case Type::VECTOR: {
      auto data = reinterpret_cast<std::vector<Tree> *>(m_data);
      delete data;
      break;
    }
    case Type::MAP: {
      auto data = reinterpret_cast<TreeMap *>(m_data);
      delete data;
      break;
    }
    case Type::PAIRS: {
      auto data = reinterpret_cast<std::vector<std::pair<type::String, Tree>> *>(m_data);
      delete data;
      break;
    }

    default:
      // DO-NOTHING
      break;

  }
}

Tree::operator type::String () {
  return getString();
}

Tree& Tree::operator [] (const type::String& key) {
  return getMap()[key];
}

const Tree& Tree::operator [] (const type::String& key) const {
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
  m_attributes = other.m_attributes;

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
      auto otherData = reinterpret_cast<type::String *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::mapping::Tree::setCopy()]: other.data is null, other.type is 'STRING'");
      }
      auto ptr = new type::String(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }
    case Type::VECTOR: {
      auto otherData = reinterpret_cast<std::vector<Tree> *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::mapping::Tree::setCopy()]: other.data is null, other.type is 'VECTOR'");
      }
      auto ptr = new std::vector<Tree>(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }
    case Type::MAP: {
      auto otherData = reinterpret_cast<TreeMap *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::mapping::Tree::setCopy()]: other.data is null, other.type is 'MAP'");
      }
      auto ptr = new TreeMap(*otherData);
      m_data = reinterpret_cast<LARGEST_TYPE>(ptr);
      break;
    }
    case Type::PAIRS: {
      auto otherData = reinterpret_cast<std::vector<std::pair<type::String, Tree>> *>(other.m_data);
      if(otherData == nullptr) {
        throw std::runtime_error("[oatpp::data::mapping::Tree::setCopy()]: other.data is null, other.type is 'PAIRS'");
      }
      auto ptr = new std::vector<std::pair<type::String, Tree>>(*otherData);
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
  m_attributes = std::move(other.m_attributes);

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

void Tree::setString(const type::String& value) {
  deleteValueObject();
  m_type = Type::STRING;
  auto data = new type::String(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setString(type::String&& value) {
  deleteValueObject();
  m_type = Type::STRING;
  auto data = new type::String(std::move(value));
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setVector(const std::vector<Tree>& value) {
  deleteValueObject();
  m_type = Type::VECTOR;
  auto data = new std::vector<Tree>(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setVector(std::vector<Tree>&& value) {
  deleteValueObject();
  m_type = Type::VECTOR;
  auto data = new std::vector<Tree>(std::move(value));
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setVector(v_uint64 size) {
  deleteValueObject();
  m_type = Type::VECTOR;
  auto data = new std::vector<Tree>(size);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setMap(const TreeMap& value) {
  deleteValueObject();
  m_type = Type::MAP;
  auto data = new TreeMap(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setMap(TreeMap&& value) {
  deleteValueObject();
  m_type = Type::MAP;
  auto data = new TreeMap(std::move(value));
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setPairs(const std::vector<std::pair<type::String, Tree>>& value) {
  deleteValueObject();
  m_type = Type::PAIRS;
  auto data = new std::vector<std::pair<type::String, Tree>>(value);
  m_data = reinterpret_cast<LARGEST_TYPE>(data);
}

void Tree::setPairs(const std::vector<std::pair<type::String, Tree>>&& value) {
  deleteValueObject();
  m_type = Type::PAIRS;
  auto data = new std::vector<std::pair<type::String, Tree>>(std::move(value));
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
    case Type::PAIRS:
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
    case Type::PAIRS:
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
    case Type::PAIRS:
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
    case Type::PAIRS:
    default:
      return false;
  }
}

bool Tree::isString() const {
  return m_type == Type::STRING;
}

bool Tree::isVector() const {
  return m_type == Type::VECTOR;
}

bool Tree::isMap() const {
  return m_type == Type::MAP;
}

bool Tree::isPairs() const {
  return m_type == Type::PAIRS;
}

v_int64 Tree::getInteger() const {
  if(m_type != Type::INTEGER) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getInteger()]: NOT an arbitrary INTEGER.");
  }
  v_int64 result;
  std::memcpy (&result, &m_data, sizeof(v_float64));
  return result;
}

v_float64 Tree::getFloat() const {
  if(m_type != Type::FLOAT) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getFloat()]: NOT an arbitrary FLOAT.");
  }
  v_float64 result;
  std::memcpy (&result, &m_data, sizeof(v_float64));
  return result;
}

const type::String& Tree::getString() const {
  if(m_type != Type::STRING) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getString()]: NOT a STRING.");
  }
  auto data = reinterpret_cast<const type::String*>(m_data);
  return *data;
}

const std::vector<Tree>& Tree::getVector() const {
  if(m_type != Type::VECTOR) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getVector()]: NOT a VECTOR.");
  }
  auto data = reinterpret_cast<const std::vector<Tree>*>(m_data);
  return *data;
}

const TreeMap& Tree::getMap() const {
  if(m_type != Type::MAP) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getMap()]: NOT a MAP.");
  }
  auto data = reinterpret_cast<const TreeMap*>(m_data);
  return *data;
}

const std::vector<std::pair<type::String, Tree>>& Tree::getPairs() const {
  if(m_type != Type::PAIRS) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getPairs()]: NOT a PAIRS.");
  }
  auto data = reinterpret_cast<const std::vector<std::pair<type::String, Tree>>*>(m_data);
  return *data;
}

std::vector<Tree>& Tree::getVector() {
  if(m_type == Type::UNDEFINED) {
    setVector({});
  }
  if(m_type != Type::VECTOR) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getVector()]: NOT a VECTOR.");
  }
  auto data = reinterpret_cast<std::vector<Tree>*>(m_data);
  return *data;
}

TreeMap& Tree::getMap() {
  if(m_type == Type::UNDEFINED) {
    setMap({});
  }
  if(m_type != Type::MAP) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getMap()]: NOT a MAP.");
  }
  auto data = reinterpret_cast<TreeMap*>(m_data);
  return *data;
}

std::vector<std::pair<type::String, Tree>>& Tree::getPairs() {
  if(m_type == Type::UNDEFINED) {
    setPairs({});
  }
  if(m_type != Type::PAIRS) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::getMap()]: NOT a PAIRS.");
  }
  auto data = reinterpret_cast<std::vector<std::pair<type::String, Tree>>*>(m_data);
  return *data;
}

Tree::Attributes& Tree::attributes() {
  return m_attributes;
}

const Tree::Attributes& Tree::attributes() const {
  return m_attributes;
}

type::String Tree::debugPrint(v_uint32 indent0, v_uint32 indentDelta, bool firstLineIndent) const {

  stream::BufferOutputStream ss;
  for(v_uint32 i = 0; i < indent0; i ++) {
    ss << " ";
  }
  type::String indentStr0 = ss.toString();

  ss.setCurrentPosition(0);
  for(v_uint32 i = 0; i < indentDelta; i ++) {
    ss << " ";
  }
  type::String indentDeltaStr = ss.toString();

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
      ss << getPrimitive<bool>() << " (bool)";
      break;
    }
    case Type::INT_8: {
      ss << getPrimitive<v_int8>() << " (int_8)";
      break;
    }

    case Type::UINT_8: {
      ss << getPrimitive<v_uint8>() << " (uint_8)";
      break;
    }
    case Type::INT_16: {
      ss << getPrimitive<v_int16>() << " (int_16)";
      break;
    }
    case Type::UINT_16: {
      ss << getPrimitive<v_uint16>() << " (uint_16)";
      break;
    }
    case Type::INT_32: {
      ss << getPrimitive<v_int32 >() << " (int_32)";
      break;
    }
    case Type::UINT_32: {
      ss << getPrimitive<v_uint32>() << " (uint_32)";
      break;
    }
    case Type::INT_64: {
      ss << getPrimitive<v_int64>() << " (int_64)";
      break;
    }
    case Type::UINT_64: {
      ss << getPrimitive<v_uint64>() << " (uint_64)";
      break;
    }
    case Type::FLOAT_32: {
      ss << getPrimitive<v_float32>() << " (float_32)";
      break;
    }
    case Type::FLOAT_64: {
      ss << getPrimitive<v_float64>() << " (float_64)";
      break;
    }
    case Type::STRING: {
      ss << "'" << getString() << "'";
      break;
    }

    case Type::VECTOR: {
      ss << "VECTOR [\n";
      auto& vector = getVector();
      for(auto& v : vector) {
        ss << v.debugPrint(indent0 + indentDelta, indentDelta) << "\n";
      }
      ss << indentStr0 << "]";
      break;
    }
    case Type::MAP: {
      ss << "MAP {\n";
      auto& map = getMap();
      for(v_uint32 i = 0; i < map.size(); i ++) {
        const auto& node = map[i];
        ss << indentStr0 << indentDeltaStr << node.first << ": " << node.second.get().debugPrint(indent0 + indentDelta, indentDelta, false) << "\n";
      }
      ss << indentStr0 << "}";
      break;
    }
    case Type::PAIRS: {
      ss << "PAIRS {\n";
      auto& pairs = getPairs();
      for(auto& node : pairs) {
        ss << indentStr0 << indentDeltaStr << node.first << ": " << node.second.debugPrint(indent0 + indentDelta, indentDelta, false) << "\n";
      }
      ss << indentStr0 << "}";
      break;
    }

    default:
      break;
  }

  return ss.toString();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TreeMap

TreeMap::TreeMap(const TreeMap& other) {
  operator=(other);
}

TreeMap::TreeMap(TreeMap&& other) noexcept {
  operator=(std::move(other));
}

TreeMap& TreeMap::operator = (const TreeMap& other) {
  m_map = other.m_map;
  m_order = other.m_order;
  for(auto & po : m_order){
    po.second = &m_map.at(po.first.lock());
  }
  return *this;
}

TreeMap& TreeMap::operator = (TreeMap&& other) noexcept {
  m_map = std::move(other.m_map);
  m_order = std::move(other.m_order);
  for(auto& p : m_order) {
    p.second = &m_map.at(p.first.lock());
  }
  return *this;
}

Tree& TreeMap::operator [] (const type::String& key) {
  auto it = m_map.find(key);
  if(it == m_map.end()) {
    auto& result = m_map[key];
    m_order.emplace_back(key.getPtr(), &result);
    return result;
  }
  return it->second;
}

const Tree& TreeMap::operator [] (const type::String& key) const {
  auto it = m_map.find(key);
  if(it == m_map.end()) {
    throw std::runtime_error("[oatpp::data::mapping::Tree::TreeMap::operator[]]: const operator[] can't add items.");
  }
  return it->second;
}

std::pair<type::String, std::reference_wrapper<Tree>> TreeMap::operator [] (v_uint64 index) {
  auto& item = m_order.at(index);
  return {item.first.lock(), *item.second};
}

std::pair<type::String, std::reference_wrapper<const Tree>> TreeMap::operator [] (v_uint64 index) const {
  auto& item = m_order.at(index);
  return {item.first.lock(), *item.second};
}

v_uint64 TreeMap::size() const {
  return m_map.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TreeChildrenOperator

TreeChildrenOperator::TreeChildrenOperator(Tree& tree)
  : TreeChildrenOperator(const_cast<const Tree&>(tree))
{
  m_const = false;
}

TreeChildrenOperator::TreeChildrenOperator(const Tree& tree)
  : m_vector(nullptr)
  , m_map(nullptr)
  , m_pairs(nullptr)
{
  m_const = true;
  if(tree.getType() == Tree::Type::VECTOR) {
    m_type = VECTOR;
    m_vector = std::addressof(tree.getVector());
  } else if(tree.getType() == Tree::Type::MAP) {
    m_type = MAP;
    m_map = std::addressof(tree.getMap());
  } else if(tree.getType() == Tree::Type::PAIRS) {
    m_type = PAIRS;
    m_pairs = std::addressof(tree.getPairs());
  } else {
    throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::TreeChildrenOperator()]: Node type is NOT suppoerted");
  }
}

std::pair<type::String, Tree*> TreeChildrenOperator::getPair(v_uint64 index) {
  if(m_const) {
    throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::getPair()]: Can't operate on CONST tree node");
  }
  switch (m_type) {
    case VECTOR: break;
    case MAP: {
      const auto& p = (*const_cast<TreeMap*>(m_map))[index];
      return {p.first, std::addressof(p.second.get())};
    }
    case PAIRS: {
      auto& p = const_cast<std::vector<std::pair<type::String, Tree>>*>(m_pairs)->at(index);
      return {p.first, &p.second};
    }
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::getPair()]: Node type doesn't support pairs");
}

std::pair<type::String, const Tree*> TreeChildrenOperator::getPair(v_uint64 index) const {
  switch (m_type) {
    case VECTOR: break;
    case MAP: {
      const auto& p = (*m_map)[index];
      return {p.first, std::addressof(p.second.get())};
    }
    case PAIRS: {
      auto& p = (*m_pairs)[index];
      return {p.first, &p.second};
    }
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::getPair()]: Node type doesn't support pairs");
}

Tree* TreeChildrenOperator::getItem(v_uint64 index) {
  if(m_const) {
    throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::getItem()]: Can't operate on CONST tree node");
  }
  switch (m_type) {
    case VECTOR: return std::addressof(const_cast<std::vector<Tree>*>(m_vector)->at(index));
    case MAP: return std::addressof((*const_cast<TreeMap*>(m_map))[index].second.get());
    case PAIRS: return &const_cast<std::vector<std::pair<type::String, Tree>>*>(m_pairs)->at(index).second;
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::getItem()]: Invalid iterator type");
}

const Tree* TreeChildrenOperator::getItem(v_uint64 index) const {
  switch (m_type) {
    case VECTOR: return std::addressof(m_vector->at(index));
    case MAP: return std::addressof((*m_map)[index].second.get());
    case PAIRS: return &m_pairs->at(index).second;
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::getItem()]: Invalid operator type");
}

Tree* TreeChildrenOperator::putPair(const type::String& key, const Tree& tree) {
  if(m_const) {
    throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putPair()]: Can't operate on CONST tree node");
  }
  switch (m_type) {
    case VECTOR: break;
    case MAP: {
      auto& node = (*const_cast<TreeMap*>(m_map))[key];
      node = tree;
      return std::addressof(node);
    }
    case PAIRS: {
      auto& pairs = *const_cast<std::vector<std::pair<type::String, Tree>>*>(m_pairs);
      pairs.emplace_back(key, tree);
      auto& p = pairs.at(pairs.size() - 1);
      return std::addressof(p.second);
    }
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putPair()]: Node type doesn't support pairs");
}

Tree* TreeChildrenOperator::putPair(const type::String& key, Tree&& tree) {
  if(m_const) {
    throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putPair()]: Can't operate on CONST tree node");
  }
  switch (m_type) {
    case VECTOR: break;
    case MAP: {
      auto& node = (*const_cast<TreeMap*>(m_map))[key];
      node = std::move(tree);
      return std::addressof(node);
    }
    case PAIRS: {
      auto& pairs = *const_cast<std::vector<std::pair<type::String, Tree>>*>(m_pairs);
      pairs.emplace_back(key, std::move(tree));
      auto& p = pairs.at(pairs.size() - 1);
      return std::addressof(p.second);
    }
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putPair()]: Node type doesn't support pairs");
}

Tree* TreeChildrenOperator::putItem(const Tree& tree) {
  if(m_const) {
    throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putItem()]: Can't operate on CONST tree node");
  }
  switch (m_type) {
    case VECTOR: {
      auto& vector = *const_cast<std::vector<Tree>*>(m_vector);
      vector.push_back(tree);
      return std::addressof(vector.at(vector.size() - 1));
    }
    case MAP:
    case PAIRS:
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putItem()]: Invalid iterator type");
}

Tree* TreeChildrenOperator::putItem(Tree&& tree) {
  if(m_const) {
    throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putItem()]: Can't operate on CONST tree node");
  }
  switch (m_type) {
    case VECTOR: {
      auto& vector = *const_cast<std::vector<Tree>*>(m_vector);
      vector.emplace_back(std::move(tree));
      return std::addressof(vector.at(vector.size() - 1));
    }
    case MAP:
    case PAIRS:
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::putItem()]: Invalid iterator type");
}

v_uint64 TreeChildrenOperator::size() const {
  switch (m_type) {
    case VECTOR: return m_vector->size();
    case MAP: return m_map->size();
    case PAIRS: return m_pairs->size();
    default:
      break;
  }
  throw std::runtime_error("[oatpp::data::mapping::TreeChildrenOperator::size()]: Invalid operator type");
}

}}}
