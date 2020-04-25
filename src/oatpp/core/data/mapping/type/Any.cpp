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

#include "Any.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {
  const ClassId Any::CLASS_ID("Any");
}

Any::Any()
  : ObjectWrapper(__class::Any::getType())
{}

Any::Any(std::nullptr_t) : Any() {}

Any::Any(const std::shared_ptr<base::Countable>& ptr, const Type* const type)
  : ObjectWrapper(std::make_shared<AnyHandle>(ptr, type), __class::Any::getType())
{}

Any::Any(const Any& other)
  : ObjectWrapper(std::make_shared<AnyHandle>(other.m_ptr->ptr, other.m_ptr->type), __class::Any::getType())
{}

Any::Any(Any&& other)
  : ObjectWrapper(std::move(other.m_ptr),  __class::Any::getType())
{}

void Any::store(const AbstractObjectWrapper& polymorph) {
  m_ptr = std::make_shared<AnyHandle>(polymorph.getPtr(), polymorph.valueType);
}

const Type* const Any::getStoredType() {
  if(m_ptr) {
    return m_ptr->type;
  }
  return nullptr;
}

Any& Any::operator=(std::nullptr_t) {
  m_ptr.reset();
  return *this;
}

Any& Any::operator=(const AbstractObjectWrapper& other) {
  m_ptr = std::make_shared<AnyHandle>(other.getPtr(), other.valueType);
  return *this;
}

Any& Any::operator=(const Any& other) {
  if(other) {
    m_ptr = std::make_shared<AnyHandle>(other.m_ptr->ptr, other.m_ptr->type);
  } else {
    m_ptr.reset();
  }
  return *this;
}

Any& Any::operator=(Any&& other) {
  m_ptr = std::move(other.m_ptr);
  return *this;
}

bool Any::operator == (const Any& other) {
  if(!m_ptr && !other.m_ptr) return true;
  if(!m_ptr || !other.m_ptr) return false;
  return m_ptr->ptr.get() == other.m_ptr->ptr.get();
}

bool Any::operator != (const Any& other) {
  return !operator == (other);
}

}}}}
