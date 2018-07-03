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
                             Property* field,
                             const std::shared_ptr<Config>& config) {
  auto value = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(field->get(object));
  if(value) {
    stream->writeChar('\"');
    stream->write(field->name);
    stream->write("\": ", 3);
    auto encodedValue = Utils::escapeString(value->getData(), value->getSize());
    stream->writeChar('\"');
    stream->write(encodedValue);
    stream->writeChar('\"');
  } else if(config->includeNullFields) {
    stream->writeChar('\"');
    stream->write(field->name);
    stream->write("\": null", 7);
  }
}

void Serializer::writeObject(oatpp::data::stream::OutputStream* stream,
                             void* object,
                             Property* field,
                             const std::shared_ptr<Config>& config){
  auto value = oatpp::data::mapping::type::static_wrapper_cast<Object>(field->get(object));
  if(value) {
    stream->writeChar('\"');
    stream->write(field->name);
    stream->write("\": ", 3);
    writeObject(stream, field->type, value.get(), config);
  } else if(config->includeNullFields) {
    stream->writeChar('\"');
    stream->write(field->name);
    stream->write("\": null", 7);
  }
}

void Serializer::writeListOfString(oatpp::data::stream::OutputStream* stream,
                                   AbstractList* list,
                                   const std::shared_ptr<Config>& config){
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getData());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      auto encodedValue = Utils::escapeString(value->getData(), value->getSize());
      stream->writeChar('\"');
      stream->write(encodedValue);
      stream->writeChar('\"');
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      stream->write("null", 4);
    }
    
    curr = curr->getNext();
    
  }
  
  stream->writeChar(']');
  
}

void Serializer::writeListOfObject(oatpp::data::stream::OutputStream* stream,
                                   AbstractList* list,
                                   const Type* const type,
                                   const std::shared_ptr<Config>& config){
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<Object>(curr->getData());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      writeObject(stream, type, value.get(), config);
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      stream->write("null", 4);
    }
    
    curr = curr->getNext();
    
  }
  
  stream->writeChar(']');
  
}

void Serializer::writeListOfList(oatpp::data::stream::OutputStream* stream,
                                 AbstractList* list,
                                 const Type* const type,
                                 const std::shared_ptr<Config>& config){
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(curr->getData());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      writeListCollection(stream, value.get(), type, config);
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      stream->write("null", 4);
    }
    
    curr = curr->getNext();
    
  }
  
  stream->writeChar(']');
  
}

void Serializer::writeListCollection(oatpp::data::stream::OutputStream* stream,
                                     AbstractList* list,
                                     const Type* const type,
                                     const std::shared_ptr<Config>& config){
  
  Type* itemType = *(type->params.begin());
  auto itemTypeName = itemType->name;
  
  if(itemTypeName == oatpp::data::mapping::type::__class::String::CLASS_NAME){
    writeListOfString(stream, list, config);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Int32::ObjectType>(stream, list, config);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Int64::ObjectType>(stream, list, config);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Float32::ObjectType>(stream, list, config);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Float64::ObjectType>(stream, list, config);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
    writeListOfSimpleData<oatpp::data::mapping::type::Boolean::ObjectType>(stream, list, config);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
    writeListOfObject(stream, list, itemType, config);
  } else if(itemTypeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
    writeListOfList(stream, list, itemType, config);
  }
  
}

void Serializer::writeList(oatpp::data::stream::OutputStream* stream,
                           void* object,
                           Property* field,
                           const std::shared_ptr<Config>& config){
  auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(field->get(object));
  if(value) {
    stream->writeChar('\"');
    stream->write(field->name);
    stream->write("\": ", 3);
    writeListCollection(stream, value.get(), field->type, config);
  } else if(config->includeNullFields) {
    stream->writeChar('\"');
    stream->write(field->name);
    stream->write("\": null", 7);
  }
}

void Serializer::writeObject(oatpp::data::stream::OutputStream* stream,
                             const Type* type,
                             Object* object,
                             const std::shared_ptr<Config>& config){
  
  stream->writeChar('{');
  bool first = true;
  auto fieldsMap = type->properties;
  for (auto const& iterator : *fieldsMap) {
    
    auto field = iterator.second;
    auto abstractValue = field->get(object);
    if(abstractValue) {
      (first) ? first = false : stream->write(", ", 2);
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
    }
    
    auto type = field->type;
    auto typeName = type->name;
    
    if(typeName == oatpp::data::mapping::type::__class::String::CLASS_NAME){
      writeString(stream, object, field, config);
    } else if(typeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Int32::ObjectType>(stream, object, field, config);
    } else if(typeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Int64::ObjectType>(stream, object, field, config);
    } else if(typeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Float32::ObjectType>(stream, object, field, config);
    } else if(typeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Float64::ObjectType>(stream, object, field, config);
    } else if(typeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
      writeSimpleData<oatpp::data::mapping::type::Boolean::ObjectType>(stream, object, field, config);
    } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
      writeObject(stream, object, field, config);
    } else if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
      writeList(stream, object, field, config);
    }
    
  }
  
  stream->writeChar('}');
  
}
  
}}}}
