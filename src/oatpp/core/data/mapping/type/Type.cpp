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

Type::Property::Property(v_int64 pOffset, const char* pName, Type* pType)
  : offset(pOffset)
  , name(pName)
  , type(pType)
{}

void Type::Property::set(void* object, const Void& value) {
  Void* property = (Void*)(((v_int64) object) + offset);
  *property = value;
}

Void Type::Property::get(void* object) {
  Void* property = (Void*)(((v_int64) object) + offset);
  return *property;
}

Void& Type::Property::getAsRef(void* object) {
  Void* property = (Void*)(((v_int64) object) + offset);
  return *property;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type

Type::Type(const ClassId& pClassId,
           const char* pNameQualifier,
           Creator pCreator,
           PropertiesGetter pPropertiesGetter,
           void* pPolymorphicDispatcher)
  : classId(pClassId)
  , nameQualifier(pNameQualifier)
  , creator(pCreator)
  , propertiesGetter(pPropertiesGetter)
  , polymorphicDispatcher(pPolymorphicDispatcher)
{}
  
}}}}
