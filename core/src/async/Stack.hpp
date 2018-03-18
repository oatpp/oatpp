//
//  Stack.hpp
//  crud
//
//  Created by Leonid on 3/17/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_async_Stack_hpp
#define oatpp_async_Stack_hpp

#include <stdexcept>

namespace oatpp { namespace async {

class Processor;
  
template<class T>
class Stack {
  friend Processor;
private:
  
  class Entry {
  public:
    Entry(const T& pData, Entry* pNext)
      : data(pData)
      , next(pNext)
    {}
    T data;
    Entry* next;
  };
private:
  void null(){
    m_root = nullptr;
    m_last = nullptr;
  }
private:
  Entry* m_root;
  Entry* m_last;
public:
  Stack()
    : m_root(nullptr)
    , m_last(nullptr)
  {}
  
  ~Stack(){
    clear();
  }
  
  void push(const T& data){
    m_root = new Entry(data, m_root);
    if(m_last == nullptr) {
      m_last = m_root;
    }
  }
  
  void pushBack(const T& data){
    Entry* entry = new Entry(data, nullptr);
    if(m_last != nullptr) {
      m_last->next = entry;
      m_last = entry;
    } else {
      m_root = entry;
      m_last = entry;
    }
  }
  
  T& peek() const {
    if(m_root != nullptr) {
      return m_root->data;
    } else {
      throw std::runtime_error("Peek on empty stack");
    }
  }
  
  T pop() {
    if(m_root != nullptr) {
      Entry* resultEntry = m_root;
      m_root = m_root->next;
      if(m_root == nullptr){
        m_last = nullptr;
      }
      T result = resultEntry->data;
      delete resultEntry;
      return result;
    } else {
      throw std::runtime_error("Pop on empty stack");
    }
  }
  
  void popNoData() {
    if(m_root != nullptr) {
      Entry* resultEntry = m_root;
      m_root = m_root->next;
      if(m_root == nullptr){
        m_last = nullptr;
      }
      delete resultEntry;
    } else {
      throw std::runtime_error("Pop on empty stack");
    }
  }
  
  void clear(){
    Entry* curr = m_root;
    while (curr != nullptr) {
      Entry* next = curr->next;
      delete curr;
      curr = next;
    }
    m_root = nullptr;
    m_last = nullptr;
  }
  
  bool isEmpty() const {
    return m_root == nullptr;
  }
  
};
  
}}

#endif /* Stack_hpp */
