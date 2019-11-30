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

#ifndef oatpp_collection_ListMap_hpp
#define oatpp_collection_ListMap_hpp

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"

namespace oatpp { namespace collection {
  
template<class K, class V>
class ListMap : public oatpp::base::Countable {
public:
  OBJECT_POOL(ListMap_Pool, ListMap, 32)
  SHARED_OBJECT_POOL(Shared_ListMap_Pool, ListMap, 32)
public:
  
  //--------------------------------------------------------------------------------------
  // Entry
  
  class Entry{
    friend ListMap;
  public:
    OBJECT_POOL_THREAD_LOCAL(ListMap_Entry_Pool, Entry, 64)
  private:
    K           key;
    V           value;
    Entry*      next;
  protected:
    Entry(const K& pKey, const V& pValue, Entry* pNext)
      : key(pKey)
      , value(pValue)
      , next(pNext)
    {}
    
    ~Entry(){
    }
  public:
    
    const K& getKey() const{
      return key;
    }
    
    const V& getValue() const{
      return value;
    }
    
    Entry* getNext() const{
      return next;
    }
    
  };
  
private:
  
  Entry* m_first;
  Entry* m_last;
  v_int32 m_count;
  
  oatpp::base::memory::MemoryPool& m_itemMemoryPool;
  
private:
  
  Entry* createEntry(const K& pKey, const V& pValue, Entry* pNext){
    return new (m_itemMemoryPool.obtain()) Entry(pKey, pValue, pNext);
  }
  
  void destroyEntry(Entry* entry){
    entry->~Entry();
    oatpp::base::memory::MemoryPool::free(entry);
  }
  
private:
  
  template<class Key>
  Entry* getEntryByKey(const Key& key) const{
    
    Entry* curr = m_first;
    
    while(curr != nullptr){
      if(key == curr->key){
        return curr;
      }
      curr = curr->next;
    }
    
    return nullptr;
    
  }
  
  void addOneEntry(Entry* entry){
    
    if(m_last == nullptr){
      m_first = entry;
      m_last = entry;
    }else{
      m_last->next = entry;
      m_last = entry;
    }
    
    m_count++;
  }
  
public:
  ListMap()
    : m_first(nullptr)
    , m_last(nullptr)
    , m_count(0)
    , m_itemMemoryPool(Entry::ListMap_Entry_Pool::getPool())
  {}
public:
  
  static std::shared_ptr<ListMap> createShared(){
    return Shared_ListMap_Pool::allocateShared();
  }
  
  ~ListMap() override {
    clear();
  }
  
  Entry* put(const K& key, const V& value){
    Entry* entry = getEntryByKey(key);
    if(entry != nullptr){
      if(entry->value != value){
        entry->value = value;
      }
    }else{
      entry = createEntry(key, value, nullptr);
      addOneEntry(entry);
    }
    return entry;
  }
  
  bool putIfNotExists(const K& key, const V& value){
    Entry* entry = getEntryByKey(key);
    if(entry == nullptr){
      entry = createEntry(key, value, nullptr);
      addOneEntry(entry);
      return true;
    }
    return false;
  }
  
  const Entry* find(const K& key) const{
    Entry* entry = getEntryByKey<K>(key);
    if(entry != nullptr){
      return entry;
    }
    return nullptr;
  }
  
  const V& get(const K& key, const V& defaultValue) const {
    Entry* entry = getEntryByKey<K>(key);
    if(entry != nullptr){
      return entry->getValue();
    }
    return defaultValue;
  }
  
  /*
  template <class Key>
  const Entry* getByKeyTemplate(const Key& key) const{
    
    Entry* entry = getEntryByKey(key);
    if(entry != nullptr){
      return entry;
    }
    
    return nullptr;
    
  }
  */
   
  V remove(const K& key){
    
    if(m_first != nullptr){
      
      if(m_first->key->equals(key)){
        Entry* next = m_first->next;
        V result = m_first->value;
        destroyEntry(m_first);
        m_first = next;
        return result;
      }
    
      Entry* curr = m_first;
      Entry* next = m_first->next;
      
      while(next != nullptr){
        if(next->key->equals(key)){
          V result = next->value;
          curr->next = next->next;
          destroyEntry(next);
          return result;
        }
        curr = next;
        next = curr->next;
      }
      
    }
    
    return V::empty();
  }

  Entry* getEntryByIndex(v_int32 index) const{

    if(index >= m_count){
      return nullptr;
    }

    v_int32 i = 0;
    Entry* curr = m_first;

    while(curr != nullptr){

      if(i == index){
        return curr;
      }

      curr = curr->next;
      i++;
    }

    return nullptr;

  }

  Entry* getFirstEntry() const {
    return m_first;
  }
  
  v_int32 count() const{
    return m_count;
  }
  
  void clear(){
    
    Entry* curr = m_first;
    while(curr != nullptr){
      Entry* next = curr->next;
      destroyEntry(curr);
      curr = next;
    }
    
    m_first = nullptr;
    m_last = nullptr;
    m_count = 0;
    
  }
  
};

}}

#endif /* oatpp_collection_ListMap_hpp */
