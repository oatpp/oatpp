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

#ifndef oatpp_base_ObjectHandle_hpp
#define oatpp_base_ObjectHandle_hpp

#include "oatpp/Environment.hpp"

namespace oatpp { namespace base {

template<class T>
class ObjectHandle {
private:
  T* m_object;
  std::shared_ptr<T> m_ptr;
public:

  ObjectHandle(T* object)
    : m_object(object)
  {}

  template<class Q>
  ObjectHandle(const std::shared_ptr<Q>& sharedObject)
    : m_object(sharedObject.get())
    , m_ptr(sharedObject)
  {}

  std::shared_ptr<T> getPtr() const {
    return m_ptr;
  }

  T* get() const {
    return m_object;
  }

  T* operator->() const {
    return m_object;
  }

  explicit operator bool() const {
    return m_object != nullptr;
  }

};

}}

#endif //oatpp_base_ObjectHandle_hpp
