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

#include "./Object.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseObject

void BaseObject::set(v_int64 offset, const Void& value) {
  Void* property = (Void*)(((v_int64) m_basePointer) + offset);
  *property = value;
}

Void BaseObject::get(v_int64 offset) const {
  Void* property = (Void*)(((v_int64) m_basePointer) + offset);
  return *property;
}

Void& BaseObject::getAsRef(v_int64 offset) const {
  Void* property = (Void*)(((v_int64) m_basePointer) + offset);
  return *property;
}

void BaseObject::setBasePointer(void* basePointer) {
  m_basePointer = basePointer;
}

void* BaseObject::getBasePointer() const {
  return m_basePointer;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseObject::Properties

BaseObject::Property* BaseObject::Properties::pushBack(Property* property) {
  m_map.insert({property->name, property});
  m_list.push_back(property);
  return property;
}

void BaseObject::Properties::pushFrontAll(Properties* properties) {
  m_map.insert(properties->m_map.begin(), properties->m_map.end());
  m_list.insert(m_list.begin(), properties->m_list.begin(), properties->m_list.end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseObject::Property

BaseObject::Property::Property(v_int64 pOffset, const char* pName, const Type* pType)
  : offset(pOffset)
  , name(pName)
  , type(pType)
{}

void BaseObject::Property::set(BaseObject* object, const Void& value) {
  object->set(offset, value);
}

Void BaseObject::Property::get(BaseObject* object) {
  return object->get(offset);
}

Void& BaseObject::Property::getAsRef(BaseObject* object) {
  return object->getAsRef(offset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Object

namespace __class {

  const ClassId AbstractObject::CLASS_ID("Object");

}

const mapping::type::Type* DTO::getParentType() {
  return nullptr;
}

const char* DTO::Z__CLASS_TYPE_NAME() {
  return "DTO";
}

oatpp::data::mapping::type::BaseObject::Properties* DTO::Z__CLASS_GET_FIELDS_MAP() {
  static data::mapping::type::BaseObject::Properties map;
  return &map;
}

BaseObject::Properties* DTO::Z__CLASS_EXTEND(BaseObject::Properties* properties, BaseObject::Properties* extensionProperties) {
  properties->pushFrontAll(extensionProperties);
  return properties;
}
  
}}}}
