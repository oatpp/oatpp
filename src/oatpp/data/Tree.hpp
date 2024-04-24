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

#ifndef oatpp_data_Tree_hpp
#define oatpp_data_Tree_hpp

#include "oatpp/Types.hpp"

namespace oatpp { namespace data {

class Tree {
public:

  class Node {
  public:

    enum class Type : v_int32 {

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
      MAP = 16

    };

    template<typename T>
    struct NodePrimitiveType {
    };

  private:
    typedef v_uint64 LARGEST_TYPE;
  private:
    void deleteValueObject();
  private:
    Type m_type;
    LARGEST_TYPE m_data;
  public:

    Node();
    Node(const Node& other);
    Node(Node&& other) noexcept;

    ~Node();

    Node& operator = (const Node& other);
    Node& operator = (Node&& other) noexcept;

    Type getType() const;

    void setCopy(const Node& other);
    void setMove(Node&& other);

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
        throw std::runtime_error(std::string("[oatpp::data::Tree::Node::getValue()]: NOT a ") + NodePrimitiveType<T>::name);
      }
      T result;
      std::memcpy (&result, &m_data, sizeof(T));
      return result;
    }

    void setNull();

    void setInteger(v_int64 value);
    void setFloat(v_float64 value);

    void setString(const oatpp::String& value);
    void setVector(const std::vector<Node>& value);
    void setMap(const std::vector<std::pair<oatpp::String, Node>>& value);

    bool isNull() const;

    v_int64 getInteger() const;
    v_float64 getFloat() const;

    const oatpp::String& getString() const;

    const std::vector<Node>& getVector() const;
    const std::vector<std::pair<oatpp::String, Node>>& getMap() const;

    std::vector<Node>& getVector();
    std::vector<std::pair<oatpp::String, Node>>& getMap();

  };

public:

};

template<>
struct Tree::Node::NodePrimitiveType<bool> {
  static constexpr Type type = Type::BOOL;
  static constexpr const char* name = "BOOL";
};

template<>
struct Tree::Node::NodePrimitiveType<v_int8> {
  static constexpr Type type = Type::INT_8;
  static constexpr const char* name = "INT_8";
};

template<>
struct Tree::Node::NodePrimitiveType<v_uint8> {
  static constexpr Type type = Type::UINT_8;
  static constexpr const char* name = "UINT_8";
};

template<>
struct Tree::Node::NodePrimitiveType<v_int16> {
  static constexpr Type type = Type::INT_16;
  static constexpr const char* name = "INT_16";
};

template<>
struct Tree::Node::NodePrimitiveType<v_uint16> {
  static constexpr Type type = Type::UINT_16;
  static constexpr const char* name = "UINT_16";
};

template<>
struct Tree::Node::NodePrimitiveType<v_int32> {
  static constexpr Type type = Type::INT_32;
  static constexpr const char* name = "INT_32";
};

template<>
struct Tree::Node::NodePrimitiveType<v_uint32> {
  static constexpr Type type = Type::UINT_32;
  static constexpr const char* name = "UINT_32";
};

template<>
struct Tree::Node::NodePrimitiveType<v_int64> {
  static constexpr Type type = Type::INT_64;
  static constexpr const char* name = "INT_64";
};

template<>
struct Tree::Node::NodePrimitiveType<v_uint64> {
  static constexpr Type type = Type::UINT_64;
  static constexpr const char* name = "UINT_64";
};

template<>
struct Tree::Node::NodePrimitiveType<v_float32> {
  static constexpr Type type = Type::FLOAT_32;
  static constexpr const char* name = "FLOAT_32";
};

template<>
struct Tree::Node::NodePrimitiveType<v_float64> {
  static constexpr Type type = Type::FLOAT_64;
  static constexpr const char* name = "FLOAT_64";
};

}}

#endif //oatpp_data_Tree_hpp
