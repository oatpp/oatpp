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

#ifndef oatpp_data_type_Tree_hpp
#define oatpp_data_type_Tree_hpp

#include "./Type.hpp"
#include "./Primitive.hpp"

namespace oatpp { namespace data { namespace mapping {

class Tree; // FWD

}}}

namespace oatpp { namespace data { namespace type {

namespace __class {

/**
 * Tree class.
 */
class Tree {
public:

  /**
   * Class Id.
   */
  static const ClassId CLASS_ID;

  static Type *getType();

};

}

class Tree : public ObjectWrapper<mapping::Tree, __class::Tree> {
public:

  /**
   * Default constructor.
   */
  Tree();

  /**
   * Nullptr constructor.
   */
  Tree(std::nullptr_t);

  /**
   * Copy constructor.
   * @param other - other Any.
   */
  Tree(const Tree& other);

  /**
   * Move constructor.
   * @param other
   */
  Tree(Tree&& other);

  /**
   * Constructor from `mapping::Tree`.
   * @param other
   */
  Tree(const mapping::Tree& other);

  /**
   * Constructor.
   * Construct from `mapping::Tree`
   * @param other
   */
  Tree(mapping::Tree&& other);

  Tree(const std::shared_ptr<mapping::Tree>& node, const Type* const type);

  Tree& operator = (std::nullptr_t);

  Tree& operator = (const Tree& other);
  Tree& operator = (Tree&& other);

  Tree& operator = (const mapping::Tree& other);
  Tree& operator = (mapping::Tree&& other);

  bool operator == (std::nullptr_t) const;
  bool operator != (std::nullptr_t) const;

  bool operator == (const Tree& other) const;
  bool operator != (const Tree& other) const;

  mapping::Tree* operator->();
  mapping::Tree* operator->() const;

  mapping::Tree& operator*();
  const mapping::Tree& operator*() const;

  mapping::Tree& operator [] (const String& key);
  const mapping::Tree& operator [] (const String& key) const;

  mapping::Tree& operator [] (v_uint64 index);
  const mapping::Tree& operator [] (v_uint64 index) const;

};

}}}

#endif //oatpp_data_type_Tree_hpp
