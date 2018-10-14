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

#include "./Type.hpp"


namespace oatpp { namespace data { namespace mapping { namespace type {

  const char* const __class::Void::CLASS_NAME_NONCONSTEXPR = "Void";

namespace __class {
  
  Type* Void::getType(){
    static Type type(CLASS_NAME, nullptr);
    return &type;
  }
  
}
  
void Type::Properties::pushBack(Property* property) {
  m_map.insert({property->name, property});
  m_list.push_back(property);
}
  
void Type::Properties::pushFrontAll(Properties* properties) {
  m_map.insert(properties->m_map.begin(), properties->m_map.end());
  m_list.insert(m_list.begin(), properties->m_list.begin(), properties->m_list.end());
}
  
}}}}
