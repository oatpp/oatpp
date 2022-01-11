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

#ifndef oatpp_data_mapping_type_Map_hpp
#define oatpp_data_mapping_type_Map_hpp

#include "./Type.hpp"
#include <list>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

/**
 * Abstract Map. <br>
 * Ex.: UnorderedMap, Fields.
 */
class Map {
public:

  /**
   * Iterator.
   */
  struct Iterator {

    /**
     * Default virtual destructor.
     */
    virtual ~Iterator() = default;

    /**
     * Get current item key.
     * @return
     */
    virtual type::Void getKey() = 0;

    /**
     * Get current item value.
     * @return
     */
    virtual type::Void getValue() = 0;

    /**
     * Iterate to next item.
     */
    virtual void next() = 0;

    /**
     * Check if iterator finished.
     * @return
     */
    virtual bool finished() = 0;

  };

public:

  /**
   * Polymorphic Dispatcher
   */
  class PolymorphicDispatcher {
  public:

    /**
     * Virtual destructor.
     */
    virtual ~PolymorphicDispatcher() = default;

    /**
     * Create Map.
     * @return
     */
    virtual type::Void createObject() const = 0;

    /**
     * Get type of map keys.
     * @return
     */
    virtual const type::Type* getKeyType() const = 0;

    /**
     * Get type of map values.
     * @return
     */
    virtual const type::Type* getValueType() const = 0;

    /**
     * Get map size.
     * @param object - map object.
     * @return - size of the map.
     */
    virtual v_int64 getMapSize(const type::Void& object) const = 0;

    /**
     * Add item.
     * @param object - Map.
     * @param key
     * @param value
     */
    virtual void addItem(const type::Void& object, const type::Void& key, const type::Void& value) const = 0;

    /**
     * Begin map iteration.
     * @param object - Map.
     * @return
     */
    virtual std::unique_ptr<Iterator> beginIteration(const type::Void& object) const = 0;

  };

  template<class ContainerType, class KeyType, class ValueType>
  struct Inserter {

    static void insert(ContainerType* c, const KeyType& k, const ValueType& v) {
      (*c)[k] = v;
    }

  };

};

template<class ContainerType, class KeyType, class ValueType, class Clazz>
class StandardMap {
public:

  struct Iterator : public Map::Iterator {

    typename ContainerType::iterator iterator;
    typename ContainerType::iterator end;

    type::Void getKey() override {
      return iterator->first;
    }

    type::Void getValue() override {
      return iterator->second;
    }

    void next() override {
      std::advance(iterator, 1);
    }

    bool finished() override {
      return iterator == end;
    }

  };

public:

  class PolymorphicDispatcher : public Map::PolymorphicDispatcher {
  public:

    type::Void createObject() const override {
      return type::Void(std::make_shared<ContainerType>(), Clazz::getType());
    }

    const type::Type* getKeyType() const override {
      const type::Type* mapType = Clazz::getType();
      return mapType->params[0];
    }

    const type::Type* getValueType() const override {
      const type::Type* mapType = Clazz::getType();
      return mapType->params[1];
    }

    v_int64 getMapSize(const type::Void& object) const override {
      ContainerType* map = static_cast<ContainerType*>(object.get());
      return map->size();
    }

    void addItem(const type::Void& object, const type::Void& key, const type::Void& value) const override {
      ContainerType* map = static_cast<ContainerType*>(object.get());
      const auto& mapKey = key.template cast<KeyType>();
      const auto& mapValue = value.template cast<ValueType>();
      Map::Inserter<ContainerType, KeyType, ValueType>::insert(map, mapKey, mapValue);
    }

    std::unique_ptr<Map::Iterator> beginIteration(const type::Void& object) const override {
      ContainerType* map = static_cast<ContainerType*>(object.get());
      auto iterator = new Iterator();
      iterator->iterator = map->begin();
      iterator->end = map->end();
      return std::unique_ptr<Map::Iterator>(iterator);
    }

  };

};

template<class KeyType, class ValueType>
struct Map::Inserter<std::list<std::pair<KeyType, ValueType>>, KeyType, ValueType> {
  static void insert(std::list<std::pair<KeyType, ValueType>>* c, const KeyType& k, const ValueType& v) {
    c->push_back({k, v});
  }
};

}

}}}}

#endif //oatpp_data_mapping_type_Map_hpp
