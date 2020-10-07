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

#include "TypeResolver.hpp"

namespace oatpp { namespace data { namespace mapping {

void TypeResolver::setKnownClass(const type::ClassId& classId, bool isKnown) {
  m_propertyTraverser.setKnownClass(classId, isKnown);
}

void TypeResolver::addKnownClasses(const std::vector<type::ClassId>& knownClasses) {
  m_propertyTraverser.addKnownClasses(knownClasses);
}

bool TypeResolver::isKnownClass(const type::ClassId& classId) const {
  return m_propertyTraverser.isKnownType(classId);
}

bool TypeResolver::isKnownType(const type::Type* type) const {
  if (type != nullptr) {
    return isKnownClass(type->classId);
  }
  return false;
}

void TypeResolver::setEnabledInterpretations(const std::vector<std::string>& interpretations) {
  m_enabledInterpretations = interpretations;
}

const std::vector<std::string>& TypeResolver::getEnabledInterpretations() const {
  return m_enabledInterpretations;
}

const type::Type* TypeResolver::resolveType(const type::Type* type) const {

  if(type == nullptr) {
    return nullptr;
  }

  if(isKnownClass(type->classId)) {
    return type;
  }

  auto interpretation = type->findInterpretation(m_enabledInterpretations);
  if(interpretation) {
    return resolveType(interpretation->getInterpretationType());
  }

  return nullptr;

}

type::Void TypeResolver::resolveValue(const type::Void& value) const {

  if(value.valueType == nullptr) {
    return nullptr;
  }

  if(isKnownClass(value.valueType->classId)) {
    return value;
  }

  auto interpretation = value.valueType->findInterpretation(m_enabledInterpretations);
  if(interpretation) {
    return resolveValue(interpretation->toInterpretation(value));
  }

  return nullptr;

}

const type::Type* TypeResolver::resolveObjectPropertyType(const type::Type* objectType,
                                                       const std::vector<std::string>& path) const
{
  return m_propertyTraverser.findPropertyType(objectType, path, m_enabledInterpretations);
}

type::Void TypeResolver::resolveObjectPropertyValue(const type::Void& object,
                                                 const std::vector<std::string>& path) const
{
  return m_propertyTraverser.findPropertyValue(object, path, m_enabledInterpretations);
}

}}}
