/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#ifndef oatpp_base_PtrWrapper
#define oatpp_base_PtrWrapper

#include "Controllable.hpp"

namespace oatpp { namespace base {

template<class T>
class PtrWrapper {
protected:
  std::shared_ptr<T> m_ptr;
public:
  
  PtrWrapper() {}
  
  PtrWrapper(const std::shared_ptr<T>& ptr)
    : m_ptr(ptr)
  {}
  
  PtrWrapper(std::shared_ptr<T>&& ptr)
    : m_ptr(std::move(ptr))
  {}
  
  PtrWrapper(const PtrWrapper& other)
    : m_ptr(other.m_ptr)
  {}
  
  PtrWrapper(PtrWrapper&& other)
    : m_ptr(std::move(other.m_ptr))
  {}
  
  PtrWrapper& operator = (const PtrWrapper& other){
    m_ptr = other.m_ptr;
    return *this;
  }
  
  PtrWrapper& operator = (PtrWrapper&& other){
    m_ptr = std::move(other.m_ptr);
    return *this;
  }
  
  T* operator->() const {
    return m_ptr.operator->();
  }
  
  T* get() const {
    return m_ptr.get();
  }
  
  std::shared_ptr<T> getPtr() const {
    return m_ptr;
  }
  
  bool isNull() const {
    return m_ptr.get() == nullptr;
  }
  
  static const PtrWrapper& empty(){
    static PtrWrapper empty;
    return empty;
  }
  
  inline bool operator == (const PtrWrapper& other){
    return m_ptr.get() == other.m_ptr.get();
  }
  
  inline bool operator != (const PtrWrapper& other){
    return m_ptr.get() != other.m_ptr.get();
  }
  
};
  
template<class T, class F>
inline PtrWrapper<T> static_wrapper_cast(const F& from){
  return PtrWrapper<T>(std::static_pointer_cast<T>(from.getPtr()));
}
  
}}


#endif /* oatpp_base_PtrWrapper */
