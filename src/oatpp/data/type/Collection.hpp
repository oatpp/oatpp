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

#ifndef oatpp_data_type_Collection_hpp
#define oatpp_data_type_Collection_hpp

#include "./Type.hpp"
#include <unordered_set>

namespace oatpp { namespace data { namespace type {

namespace __class {

/**
 * Abstract Collection. <br>
 * Ex.: Vector, List, Set.
 */
class Collection {
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
     * Get current item.
     * @return
     */
    virtual type::Void get() = 0;

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
     * Create Collection.
     * @return
     */
    virtual type::Void createObject() const = 0;

    /**
     * Get type of collection items.
     * @return
     */
    virtual const type::Type* getItemType() const = 0;

    /**
     * Get collection size.
     * @param object - collection.
     * @return - size of the collection (elements count).
     */
    virtual v_int64 getCollectionSize(const type::Void& object) const = 0;

    /**
     * Add item.
     * @param object - Collection.
     * @param item - Item to add.
     */
    virtual void addItem(const type::Void& object, const type::Void& item) const = 0;

    /**
     * Begin collection iteration.
     * @param object - Collection.
     * @return
     */
    virtual std::unique_ptr<Iterator> beginIteration(const type::Void& object) const = 0;

  };

  template<class ContainerType, class ItemType>
  struct Inserter {

    static void insert(ContainerType* c, const ItemType& i) {
      c->emplace_back(i);
    }

  };

};

template<class ContainerType, class ItemType, class Clazz>
class StandardCollection {
public:

  struct Iterator : public Collection::Iterator {

    typename ContainerType::iterator iterator;
    typename ContainerType::iterator end;

    type::Void get() override {
      return *iterator;
    }

    void next() override {
      std::advance(iterator, 1);
    }

    bool finished() override {
      return iterator == end;
    }

  };

public:

  class PolymorphicDispatcher : public Collection::PolymorphicDispatcher {
  public:

    type::Void createObject() const override {
      return type::Void(std::make_shared<ContainerType>(), Clazz::getType());
    }

    const type::Type* getItemType() const override {
      const type::Type* collectionType = Clazz::getType();
      return collectionType->params[0];
    }

    v_int64 getCollectionSize(const type::Void& object) const override {
      ContainerType* collection = static_cast<ContainerType*>(object.get());
      return static_cast<v_int64>(collection->size());
    }

    void addItem(const type::Void& object, const type::Void& item) const override {
      ContainerType* collection = static_cast<ContainerType*>(object.get());
      const auto& collectionItem = item.template cast<ItemType>();
      Collection::Inserter<ContainerType, ItemType>::insert(collection, collectionItem);
    }

    std::unique_ptr<Collection::Iterator> beginIteration(const type::Void& object) const override {
      ContainerType* collection = static_cast<ContainerType*>(object.get());
      auto iterator = new Iterator();
      iterator->iterator = collection->begin();
      iterator->end = collection->end();
      return std::unique_ptr<Collection::Iterator>(iterator);
    }

  };

};

template<class ItemType>
struct Collection::Inserter<std::unordered_set<ItemType>, ItemType> {
  static void insert(std::unordered_set<ItemType>* c, const ItemType& i) {
    c->emplace(i);
  }
};

}

}}}

#endif //oatpp_data_type_Collection_hpp
