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

#include "./Primitive.hpp"

#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/utils/Conversion.hpp"
#include "oatpp/data/share/MemoryLabel.hpp"

#include <fstream>

namespace oatpp { namespace data { namespace type {

String::String(const std::shared_ptr<std::string>& ptr, const type::Type* const valueType)
  : oatpp::data::type::ObjectWrapper<std::string, __class::String>(ptr)
{
  if(type::__class::String::getType() != valueType) {
    throw std::runtime_error("Value type does not match");
  }
}

String String::loadFromFile(const char* filename) {
  std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);
  if (file.is_open()) {
    auto result = data::type::String(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(result->data(), static_cast<std::streamsize>(result->size()));
    file.close();
    return result;
  }
  return nullptr;
}

void String::saveToFile(const char* filename) const {
  std::ofstream fs(filename, std::ios::out | std::ios::binary);
  if(m_ptr != nullptr) {
    fs.write(m_ptr->data(), static_cast<std::streamsize>(m_ptr->size()));
  }
  fs.close();
}

const std::string& String::operator*() const {
  return this->m_ptr.operator*();
}

bool String::equalsCI_ASCII(const std::string& other) {
  auto ciLabel = share::StringKeyLabelCI(m_ptr);
  return ciLabel == other;
}

bool String::equalsCI_ASCII(const String& other) {
  auto ciLabel = share::StringKeyLabelCI(m_ptr);
  return ciLabel == other;
}

bool String::equalsCI_ASCII(const char* other) {
  auto ciLabel = share::StringKeyLabelCI(m_ptr);
  return ciLabel == other;
}

std::string String::getValue(const std::string& defaultValue) const {
  if(m_ptr) {
    return *m_ptr;
  }
  return defaultValue;
}

String operator + (const char* a, const String& b) {
  data::stream::BufferOutputStream stream;
  stream << a << b;
  return stream.toString();
}

String operator + (const String& a, const char* b) {
  data::stream::BufferOutputStream stream;
  stream << a << b;
  return stream.toString();
}

String operator + (const String& a, const String& b) {
  data::stream::BufferOutputStream stream;
  stream << a << b;
  return stream.toString();
}
  
namespace __class {
  
  const ClassId String::CLASS_ID("String");

  const ClassId Int8::CLASS_ID("Int8");
  const ClassId UInt8::CLASS_ID("UInt8");

  const ClassId Int16::CLASS_ID("Int16");
  const ClassId UInt16::CLASS_ID("UInt16");

  const ClassId Int32::CLASS_ID("Int32");
  const ClassId UInt32::CLASS_ID("UInt32");

  const ClassId Int64::CLASS_ID("Int64");
  const ClassId UInt64::CLASS_ID("UInt64");

  const ClassId Float32::CLASS_ID("Float32");
  const ClassId Float64::CLASS_ID("Float64");

  const ClassId Boolean::CLASS_ID("Boolean");

  Type* String::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* Int8::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* UInt8::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* Int16::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* UInt16::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* Int32::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* UInt32::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* Int64::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* UInt64::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* Float32::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* Float64::getType(){
    static Type type(CLASS_ID);
    return &type;
  }

  Type* Boolean::getType(){
    static Type type(CLASS_ID);
    return &type;
  }
}
  
}}}
