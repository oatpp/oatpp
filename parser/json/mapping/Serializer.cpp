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

#include "./Serializer.hpp"

#include "oatpp/parser/json/Utils.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {
  
void Serializer::writeString(oatpp::data::stream::OutputStream* stream,
                             void* object,
                             Property* field) {
  auto value = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(field->get(object));
  stream->writeChar('\"');
  stream->write(field->name);
  stream->write("\": ", 3);
  if(value.isNull()){
    stream->write("null", 4);
  } else {
    auto encodedValue = Utils::escapeString(value->getData(), value->getSize());
    stream->writeChar('\"');
    stream->write(encodedValue);
    stream->writeChar('\"');
  }
}

void Serializer::writeObject(oatpp::data::stream::OutputStream* stream,
                             void* object,
                             Property* field){
  
  auto value = oatpp::data::mapping::type::static_wrapper_cast<Object>(field->get(object));
  stream->writeChar('\"');
  stream->write(field->name);
  stream->write("\": ", 3);
  if(value.isNull()){
    stream->write("null", 4);
  } else {
    writeObject(stream, field->type, value.get());
  }
  
}

void Serializer::writeListOfString(oatpp::data::stream::OutputStream* stream,
                                   AbstractList* list){
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getData());
    
    if(first){
      first = false;
    } else {
      stream->write(", ", 2);
    }
    
    if(value.isNull()){
      stream->write("null", 4);
    } else {
      auto encodedValue = Utils::escapeString(value->getData(), value->getSize());
      stream->writeChar('\"');
      stream->write(encodedValue);
      stream->writeChar('\"');
    }
    
    curr = curr->getNext();
  }
  
  stream->writeChar(']');
  
}

void Serializer::writeListOfObject(oatpp::data::stream::OutputStream* stream,
                                   AbstractList* list,
                                   const Type* const type){
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<Object>(curr->getData());
    
    if(first){
      first = false;
    } else {
      stream->write(", ", 2);
    }
    
    if(value.isNull()){
      stream->write("null", 4);
    } else {
      writeObject(stream, type, value.get());
    }
    
    curr = curr->getNext();
  }
  
  stream->writeChar(']');
  
}

void Serializer::writeListOfList(oatpp::data::stream::OutputStream* stream,
                                 AbstractList* list,
                                 const Type* const type){
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(curr->getData());
    
    if(first){
      first = false;
    } else {
      stream->write(", ", 2);
    }
    
    if(value.isNull()){
      stream->write("null", 4);
    } else {
      writeListCollection(stream, value.get(), type);
    }
    
    curr = curr->getNext();
  }
  
  stream->writeChar(']');
  
}

void Serializer::writeListCollection(oatpp::data::stream::OutputStream* stream,
                                     AbstractList* list,
                                     const Type* const type){
  
  Type* itemType = *(type->params.begin());
  auto itemTypeName = itemType->name;
  
  if(itemTypeName == oatpp::data::mapping::type::__class::String::CLASS_NAME){
    writeListOfString(stream, list);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Int32::ObjectType>(stream, list);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Int64::ObjectType>(stream, list);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Float32::ObjectType>(stream, list);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Float64::ObjectType>(stream, list);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Boolean::ObjectType>(stream, list);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
    writeListOfObject(stream, list, itemType);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
    writeListOfList(stream, list, itemType);
  }
  
}

void Serializer::writeList(oatpp::data::stream::OutputStream* stream,
                           void* object,
                           Property* field){
  auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(field->get(object));
  stream->writeChar('\"');
  stream->write(field->name);
  stream->write("\": ", 3);
  if(value.isNull()){
    stream->write("null", 4);
  } else {
    writeListCollection(stream, value.get(), field->type);
  }
}

void Serializer::writeObject(oatpp::data::stream::OutputStream* stream,
                             const Type* type,
                             Object* object){
  
  stream->writeChar('{');
  bool firstField = true;
  auto fieldsMap = type->properties;
  for (auto const& iterator : *fieldsMap) {
    
    auto field = iterator.second;
    
    if(firstField){
      firstField = false;
    } else {
      stream->write(", ", 2);
    }
    
    auto type = field->type;
    auto typeName = type->name;
    
    if(typeName == oatpp::data::mapping::type::__class::String::CLASS_NAME){
      writeString(stream, object, field);
    } else if(typeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Int32::ObjectType>(stream, object, field);
    } else if(typeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Int64::ObjectType>(stream, object, field);
    } else if(typeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Float32::ObjectType>(stream, object, field);
    } else if(typeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Float64::ObjectType>(stream, object, field);
    } else if(typeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Boolean::ObjectType>(stream, object, field);
    } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
      writeObject(stream, object, field);
    } else if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
      writeList(stream, object, field);
    }
    
  }
  
  stream->writeChar('}');
  
}
  
}}}}
