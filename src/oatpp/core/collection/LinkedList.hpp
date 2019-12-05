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

#ifndef oatpp_collection_LinkedList_hpp
#define oatpp_collection_LinkedList_hpp

#include "oatpp/core/base/memory/ObjectPool.hpp"


#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace collection {

template<class T>
class LinkedList : public base::Countable {
public:
  OBJECT_POOL(LinkedList_Pool, LinkedList, 32)
  SHARED_OBJECT_POOL(Shared_LinkedList_Pool, LinkedList, 32)
public:
  
  class LinkedListNode {
    friend LinkedList;
    friend oatpp::base::memory::MemoryPool;
  public:
    OBJECT_POOL_THREAD_LOCAL(LinkedList_Node_Pool, LinkedListNode, 32)
  private:
    T                 data;
    LinkedListNode*   next;
    
  protected:
    
    LinkedListNode(const T& nodeData, LinkedListNode* nextNode)
      : data(nodeData)
      , next(nextNode)
    {}
    
    ~LinkedListNode(){
    }
    
  public:
    
    const T& getData(){
      return data;
    }
    
    LinkedListNode* getNext(){
      return next;
    }
    
  };
  
private:
  LinkedListNode* m_first;
  LinkedListNode* m_last;
  v_int32 m_count;
  oatpp::base::memory::MemoryPool& m_itemMemoryPool;
  
  LinkedListNode* createNode(const T& data, LinkedListNode* next){
    return new (m_itemMemoryPool.obtain()) LinkedListNode(data, next);
  }
  
  void destroyNode(LinkedListNode* node){
    node->~LinkedListNode();
    oatpp::base::memory::MemoryPool::free(node);
  }
  
public:
  
  LinkedList()
    : m_first(nullptr)
    , m_last(nullptr)
    , m_count(0)
    , m_itemMemoryPool(LinkedListNode::LinkedList_Node_Pool::getPool())
  {}
  
public:
  
  static std::shared_ptr<LinkedList> createShared(){
    return Shared_LinkedList_Pool::allocateShared();
  }
  
  static std::shared_ptr<LinkedList> copy(LinkedList<T>* other){
    auto result = createShared();
    auto curr = other->m_first;
    while(curr != nullptr){
      result->pushBack(curr->data);
      curr = curr->next;
    }
    return result;
  }
  
  virtual ~LinkedList() {
    clear();
  }
  
  void pushFront(const T& data){
    
    if(m_first == nullptr){
      LinkedListNode* newNode = createNode(data, nullptr);
      m_first = newNode;
      m_last = newNode;
    }else{
      LinkedListNode* newNode = createNode(data, m_first);
      m_first = newNode;
    }
    m_count++;
    
  }
  
  void pushBack(const T& data){
    
    LinkedListNode* newNode = createNode(data, nullptr);
    
    if(m_last == nullptr){
      m_first = newNode;
      m_last = newNode;
    }else{
      m_last->next = newNode;
      m_last = newNode;
    }
    
    m_count++;
    
  }
  
  void pushBackAll(const std::shared_ptr<LinkedList>& list){
    auto curr = list->getFirstNode();
    while(curr != nullptr) {
      pushBack(curr->getData());
      curr = curr->getNext();
    }
  }
  
  void insertAfterNode(const T& data, LinkedListNode* currentNode){
    LinkedListNode* node = createNode(data, currentNode->next);
    currentNode->next = node;
    if(currentNode == m_last){
      m_last = node;
    }
    m_count++;
  }
  
  T popFront(){
    if(m_first != nullptr){
      LinkedListNode* node = m_first;
      m_first = m_first->next;
      if(m_first == nullptr){
        m_last = nullptr;
      }
      m_count --;
      T result = node->data;
      destroyNode(node);
      return result;
    }
    throw std::runtime_error("[oatpp::collection::LinkedList::popFront()]: index out of bounds");
  }

  const T& getFirst() const{
    return m_first->data;
  }
  
  const T& getLast() const{
    return m_last->data;
  }
  
  const T& get(v_int32 index) const{

    LinkedListNode* node = getNode(index);
    if(node != nullptr){
      return node->data;
    }
    
    throw std::runtime_error("[oatpp::collection::LinkedList::get(index)]: index out of bounds");
    
  }

  LinkedListNode* getNode(v_int32 index) const {

    if(index >= m_count){
      return nullptr;
    }

    v_int32 i = 0;
    LinkedListNode* curr = m_first;

    while(curr != nullptr){

      if(i == index){
        return curr;
      }

      curr = curr->next;
      i++;
    }

    return nullptr;

  }
  
  LinkedListNode* getFirstNode() const {
    return m_first;
  }

  v_int32 count() const{
    return m_count;
  }

  /**
   *  for each item call a function
   *
   *  list->forEachNode([](auto item){
   *     // your code here
   *  });
   */
  template<typename F>
  void forEach(const F& lambda) const {
    auto curr = m_first;
    while(curr != nullptr) {
      lambda(curr->data);
      curr = curr->next;
    }
  }
  
  /**
   *  for each node call a function
   *
   *  list->forEachNode([](auto node){
   *     // your code here
   *  });
   */
  template<typename F>
  void forEachNode(const F& lambda) const {
    auto curr = m_first;
    while(curr != nullptr) {
      lambda(curr);
      curr = curr->next;
    }
  }
  
  void clear(){
      
    LinkedListNode* curr = m_first;
    while(curr != nullptr){
      LinkedListNode* next = curr->next;
      destroyNode(curr);
      curr = next;
    }
    
    m_first = nullptr;
    m_last = nullptr;
    m_count = 0;
    
  }
    
};

}}

#endif /* oatpp_collection_LinkedList_hpp */
