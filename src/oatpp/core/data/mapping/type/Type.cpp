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

#include "./Type.hpp"


namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {
  
  const ClassId Void::CLASS_ID("Void");
  
  Type* Void::getType(){
    static Type type(CLASS_ID, nullptr);
    return &type;
  }
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ClassId

std::atomic_int ClassId::ID_COUNTER(0);

ClassId::ClassId(const char* pName)
  : name(pName)
  , id(ID_COUNTER ++)
{}

int ClassId::getClassCount() {
  return ID_COUNTER;
}

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
// Type::Properties

Type::Property* Type::Properties::pushBack(Property* property) {
  m_map.insert({property->name, property});
  m_list.push_back(property);
  return property;
}
  
void Type::Properties::pushFrontAll(Properties* properties) {
  m_map.insert(properties->m_map.begin(), properties->m_map.end());
  m_list.insert(m_list.begin(), properties->m_list.begin(), properties->m_list.end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type::Property

Type::Property::Property(v_int64 pOffset, const char* pName, const Type* pType)
  : offset(pOffset)
  , name(pName)
  , type(pType)
{}

void Type::Property::set(BaseObject* object, const Void& value) {
  object->set(offset, value);
}

Void Type::Property::get(BaseObject* object) {
  return object->get(offset);
}

Void& Type::Property::getAsRef(BaseObject* object) {
  return object->getAsRef(offset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type

Type::Type(const ClassId& pClassId,
           const char* pNameQualifier,
           void* pPolymorphicDispatcher,
           InterpretationMap&& pInterpretationMap)
  : classId(pClassId)
  , nameQualifier(pNameQualifier)
  , polymorphicDispatcher(pPolymorphicDispatcher)
  , interpretationMap(pInterpretationMap)
{}
  
}}}}
