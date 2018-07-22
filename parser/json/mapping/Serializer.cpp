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
  
void Serializer::writeStringValue(oatpp::data::stream::OutputStream* stream, p_char8 data, v_int32 size) {
  auto encodedValue = Utils::escapeString(data, size, false);
  stream->writeChar('\"');
  stream->write(encodedValue);
  stream->writeChar('\"');
}
  
void Serializer::writeStringValue(oatpp::data::stream::OutputStream* stream, const char* data) {
  writeStringValue(stream, (p_char8)data, (v_int32)std::strlen(data));
}
  
void Serializer::writeString(oatpp::data::stream::OutputStream* stream,
                             void* object,
                             Property* field,
                             const std::shared_ptr<Config>& config) {
  auto value = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(field->get(object));
  if(value) {
    writeStringValue(stream, field->name);
    stream->write(": ", 2);
    writeStringValue(stream, value->getData(), value->getSize());
  } else if(config->includeNullFields) {
    writeStringValue(stream, field->name);
    stream->write(": null", 6);
  }
}

void Serializer::writeObject(oatpp::data::stream::OutputStream* stream,
                             void* object,
                             Property* field,
                             const std::shared_ptr<Config>& config){
  auto value = oatpp::data::mapping::type::static_wrapper_cast<Object>(field->get(object));
  if(value) {
    writeStringValue(stream, field->name);
    stream->write(": ", 2);
    writeObject(stream, field->type, value.get(), config);
  } else if(config->includeNullFields) {
    writeStringValue(stream, field->name);
    stream->write(": null", 6);
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
      writeStringValue(stream, value->getData(), value->getSize());
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
  
void Serializer::writeListOfListMap(oatpp::data::stream::OutputStream* stream,
                                    AbstractList* list,
                                    const Type* const type,
                                    const std::shared_ptr<Config>& config){
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractListMap>(curr->getData());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      writeListMapCollection(stream, value.get(), type, config);
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
  } else if(itemTypeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
    writeListOfListMap(stream, list, itemType, config);
  }
  
}

void Serializer::writeList(oatpp::data::stream::OutputStream* stream,
                           void* object,
                           Property* field,
                           const std::shared_ptr<Config>& config){
  auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(field->get(object));
  if(value) {
    writeStringValue(stream, field->name);
    stream->write(": ", 2);
    writeListCollection(stream, value.get(), field->type, config);
  } else if(config->includeNullFields) {
    writeStringValue(stream, field->name);
    stream->write(": null", 6);
  }
}
  
void Serializer::writeListMapOfString(oatpp::data::stream::OutputStream* stream,
                                      AbstractListMap* map,
                                      const std::shared_ptr<Config>& config) {
  
  stream->writeChar('{');
  bool first = true;
  auto curr = map->getFirstEntry();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getValue());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": ", 2);
      writeStringValue(stream, value->getData(), value->getSize());
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": null", 6);
    }
    
    curr = curr->getNext();
    
  }
  
  stream->writeChar('}');
  
}
  
void Serializer::writeListMapOfObject(oatpp::data::stream::OutputStream* stream,
                                      AbstractListMap* map,
                                      const Type* const type,
                                      const std::shared_ptr<Config>& config){
  stream->writeChar('{');
  bool first = true;
  auto curr = map->getFirstEntry();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<Object>(curr->getValue());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": ", 2);
      writeObject(stream, type, value.get(), config);
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": null", 6);
    }
    
    curr = curr->getNext();
    
  }
  
  stream->writeChar('}');
  
}

void Serializer::writeListMapOfList(oatpp::data::stream::OutputStream* stream,
                                    AbstractListMap* map,
                                    const Type* const type,
                                    const std::shared_ptr<Config>& config){
  stream->writeChar('{');
  bool first = true;
  auto curr = map->getFirstEntry();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(curr->getValue());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": ", 2);
      writeListCollection(stream, value.get(), type, config);
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": null", 6);
    }
    
    curr = curr->getNext();
    
  }
  
  stream->writeChar('}');
  
}
  
void Serializer::writeListMapOfListMap(oatpp::data::stream::OutputStream* stream,
                                       AbstractListMap* map,
                                       const Type* const type,
                                       const std::shared_ptr<Config>& config){
  stream->writeChar('{');
  bool first = true;
  auto curr = map->getFirstEntry();
  while(curr != nullptr){
    
    auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractListMap>(curr->getValue());
    if(value) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": ", 2);
      writeListMapCollection(stream, value.get(), type, config);
    } else if(config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(curr->getKey());
      writeStringValue(stream, key->getData(), key->getSize());
      stream->write(": null", 6);
    }
    
    curr = curr->getNext();
    
  }
  
  stream->writeChar('}');
  
}
  
void Serializer::writeListMapCollection(oatpp::data::stream::OutputStream* stream,
                                        AbstractListMap* map,
                                        const Type* const type,
                                        const std::shared_ptr<Config>& config){
  auto it = type->params.begin();
  Type* keyType = *it ++;
  auto keyTypeName = keyType->name;
  
  Type* valueType = *it;
  auto valueTypeName = valueType->name;
  
  if(keyTypeName != oatpp::data::mapping::type::__class::String::CLASS_NAME){
    throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::writeListMapCollection()]: Invalid json map key. Key should be String");
  }
  
  if(valueTypeName == oatpp::data::mapping::type::__class::String::CLASS_NAME){
    writeListMapOfString(stream, map, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
    writeListMapOfSimpleData<oatpp::data::mapping::type::Int32::ObjectType>(stream, map, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
    writeListMapOfSimpleData<oatpp::data::mapping::type::Int64::ObjectType>(stream, map, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
    writeListMapOfSimpleData<oatpp::data::mapping::type::Float32::ObjectType>(stream, map, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
    writeListMapOfSimpleData<oatpp::data::mapping::type::Float64::ObjectType>(stream, map, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
    writeListMapOfSimpleData<oatpp::data::mapping::type::Boolean::ObjectType>(stream, map, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
    writeListMapOfObject(stream, map, valueType, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
    writeListMapOfList(stream, map, valueType, config);
  } else if(valueTypeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
    writeListMapOfListMap(stream, map, valueType, config);
  }
  
}

void Serializer::writeListMap(oatpp::data::stream::OutputStream* stream,
                              void* object,
                              Property* field,
                              const std::shared_ptr<Config>& config){
  auto value = oatpp::data::mapping::type::static_wrapper_cast<AbstractListMap>(field->get(object));
  if(value) {
    writeStringValue(stream, field->name);
    stream->write(": ", 2);
    writeListMapCollection(stream, value.get(), field->type, config);
  } else if(config->includeNullFields) {
    writeStringValue(stream, field->name);
    stream->write(": null", 6);
  }
}

void Serializer::writeObject(oatpp::data::stream::OutputStream* stream,
                             const Type* type,
                             Object* object,
                             const std::shared_ptr<Config>& config){
  
  stream->writeChar('{');
  bool first = true;
  auto fields = type->properties->getList();
  for (auto const& field : fields) {
    
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
    } else if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
      writeListMap(stream, object, field, config);
    }
    
  }
  
  stream->writeChar('}');
  
}
  
}}}}
