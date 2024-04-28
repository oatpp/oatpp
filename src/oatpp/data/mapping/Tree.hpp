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

#ifndef oatpp_data_mapping_Tree_hpp
#define oatpp_data_mapping_Tree_hpp

#include "oatpp/Types.hpp"

namespace oatpp { namespace data { namespace mapping {

class Tree {
public:

  enum class Type : v_int32 {

    UNDEFINED = -1,

    NULL_VALUE = 0,

    INTEGER = 1,
    FLOAT = 2,

    BOOL = 3,

    INT_8 = 4,
    UINT_8 = 5,
    INT_16 = 6,
    UINT_16 = 7,
    INT_32 = 8,
    UINT_32 = 9,
    INT_64 = 10,
    UINT_64 = 11,

    FLOAT_32 = 12,
    FLOAT_64 = 13,

    STRING = 14,

    VECTOR = 15,
    MAP = 16,
    PAIRS = 17

  };

  template<typename T>
  struct NodePrimitiveType {
  };

public:

  class Map {
  private:
    std::unordered_map<oatpp::String, Tree> m_map;
    std::vector<oatpp::String> m_order;
  public:

    Tree& operator [] (const oatpp::String& key);
    const Tree& operator [] (const oatpp::String& key) const;

    std::pair<oatpp::String, std::reference_wrapper<Tree>> operator [] (v_uint64 index);
    std::pair<oatpp::String, std::reference_wrapper<const Tree>> operator [] (v_uint64 index) const;

    v_uint64 size() const;

  };

public:

  class Attributes {
  private:
    std::unordered_map<oatpp::String, oatpp::String>* m_attributes;
  public:

    Attributes();
    Attributes(const Attributes& other);
    Attributes(Attributes&& other) noexcept;

    Attributes& operator = (const Attributes& other);
    Attributes& operator = (Attributes&& other) noexcept;

    ~Attributes();

    bool empty() const;

    v_uint64 size() const;

  };

private:
  typedef v_uint64 LARGEST_TYPE;
private:
  void deleteValueObject();
private:
  Type m_type;
  LARGEST_TYPE m_data;
  Attributes m_attributes;
public:

  Tree();
  Tree(const Tree& other);
  Tree(Tree&& other) noexcept;

  template<typename T, typename enabled = typename NodePrimitiveType<T>::value_type>
  explicit Tree (T value)
    : Tree()
  {
    setValue<T>(value);
  }

  explicit Tree(const oatpp::String& value);

  ~Tree();

  Tree& operator = (const Tree& other);
  Tree& operator = (Tree&& other) noexcept;

  template<typename T, typename enabled = typename NodePrimitiveType<T>::value_type>
  Tree& operator = (T value) {
    setValue<T>(value);
    return *this;
  }

  Tree& operator = (const oatpp::String& value);

  template <typename T, typename enabled = typename NodePrimitiveType<T>::value_type>
  operator T () const {

    switch (m_type) {

      case Type::UNDEFINED:
      case Type::NULL_VALUE: break;

      case Type::INTEGER: return static_cast<T>(getInteger());
      case Type::FLOAT: return static_cast<T>(getFloat());

      case Type::BOOL: return static_cast<T>(getValue<bool>());

      case Type::INT_8: return static_cast<T>(getValue<v_int8>());
      case Type::UINT_8: return static_cast<T>(getValue<v_uint8>());
      case Type::INT_16: return static_cast<T>(getValue<v_int16>());
      case Type::UINT_16: return static_cast<T>(getValue<v_uint16>());
      case Type::INT_32: return static_cast<T>(getValue<v_int32>());
      case Type::UINT_32: return static_cast<T>(getValue<v_uint32>());
      case Type::INT_64: return static_cast<T>(getValue<v_int64>());
      case Type::UINT_64: return static_cast<T>(getValue<v_uint64>());

      case Type::FLOAT_32: return static_cast<T>(getValue<v_float32>());
      case Type::FLOAT_64: return static_cast<T>(getValue<v_float64>());

      case Type::STRING:
      case Type::VECTOR:
      case Type::MAP:
      case Type::PAIRS:

      default:
        break;

    }

    throw std::runtime_error("[oatpp::data::Tree::operator T ()]: Value is NOT a Primitive type.");

  }

  operator oatpp::String ();

  Tree& operator [] (const oatpp::String& key);
  const Tree& operator [] (const oatpp::String& key) const;

  Tree& operator [] (v_uint64 index);
  const Tree& operator [] (v_uint64 index) const;

  Type getType() const;

  void setCopy(const Tree& other);
  void setMove(Tree&& other);

  template <typename T>
  void setValue(T value) {
    deleteValueObject();
    m_type = NodePrimitiveType<T>::type;
    m_data = 0;
    std::memcpy (&m_data, &value, sizeof(T));
  }

  template<typename T>
  T getValue() const {
    if(m_type != NodePrimitiveType<T>::type) {
      throw std::runtime_error(std::string("[oatpp::data::Tree::getValue()]: NOT a ") + NodePrimitiveType<T>::name);
    }
    T result;
    std::memcpy (&result, &m_data, sizeof(T));
    return result;
  }

  void setNull();
  void setUndefined();

  void setInteger(v_int64 value);
  void setFloat(v_float64 value);

  void setString(const oatpp::String& value);
  void setVector(const std::vector<Tree>& value);
  void setVector(v_uint64 size);
  void setMap(const Map& value);
  void setPairs(const std::vector<std::pair<oatpp::String, Tree>>& value);

  bool isNull() const;
  bool isUndefined() const;
  bool isPrimitive() const;

  v_int32 primitiveDataSize() const;
  bool isFloatPrimitive() const;
  bool isIntPrimitive() const;

  v_int64 getInteger() const;
  v_float64 getFloat() const;

  const oatpp::String& getString() const;

  const std::vector<Tree>& getVector() const;
  const Map& getMap() const;
  const std::vector<std::pair<oatpp::String, Tree>>& getPairs() const;

  std::vector<Tree>& getVector();
  Map& getMap();
  std::vector<std::pair<oatpp::String, Tree>>& getPairs();

  Attributes& attributes();
  const Attributes& attributes() const;

  oatpp::String debugPrint(v_uint32 indent0 = 0, v_uint32 indentDelta = 2, bool firstLineIndent = true) const;

};

//////////////////////////////////////////////////////
// Tree::NodePrimitiveType

template<>
struct Tree::NodePrimitiveType<bool> {
  static constexpr Type type = Type::BOOL;
  static constexpr const char* name = "BOOL";
  typedef bool value_type;
};

template<>
struct Tree::NodePrimitiveType<v_int8> {
  static constexpr Type type = Type::INT_8;
  static constexpr const char* name = "INT_8";
  typedef v_int8 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_uint8> {
  static constexpr Type type = Type::UINT_8;
  static constexpr const char* name = "UINT_8";
  typedef v_uint8 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_int16> {
  static constexpr Type type = Type::INT_16;
  static constexpr const char* name = "INT_16";
  typedef v_int16 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_uint16> {
  static constexpr Type type = Type::UINT_16;
  static constexpr const char* name = "UINT_16";
  typedef v_uint16 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_int32> {
  static constexpr Type type = Type::INT_32;
  static constexpr const char* name = "INT_32";
  typedef v_int32 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_uint32> {
  static constexpr Type type = Type::UINT_32;
  static constexpr const char* name = "UINT_32";
  typedef v_uint32 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_int64> {
  static constexpr Type type = Type::INT_64;
  static constexpr const char* name = "INT_64";
  typedef v_int64 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_uint64> {
  static constexpr Type type = Type::UINT_64;
  static constexpr const char* name = "UINT_64";
  typedef v_uint64 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_float32> {
  static constexpr Type type = Type::FLOAT_32;
  static constexpr const char* name = "FLOAT_32";
  typedef v_float32 value_type;
};

template<>
struct Tree::NodePrimitiveType<v_float64> {
  static constexpr Type type = Type::FLOAT_64;
  static constexpr const char* name = "FLOAT_64";
  typedef v_float64 value_type;
};

}}}

#endif //oatpp_data_mapping_Tree_hpp
