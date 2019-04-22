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

#include "Serializer.hpp"

#include "oatpp/parser/json/Utils.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {
  
void Serializer::writeString(oatpp::data::stream::ConsistentOutputStream* stream, p_char8 data, v_int32 size) {
  auto encodedValue = Utils::escapeString(data, size, false);
  stream->writeChar('\"');
  stream->write(encodedValue);
  stream->writeChar('\"');
}

void Serializer::writeString(oatpp::data::stream::ConsistentOutputStream* stream, const char* data) {
  writeString(stream, (p_char8)data, (v_int32)std::strlen(data));
}
  
void Serializer::writeList(oatpp::data::stream::ConsistentOutputStream* stream, const AbstractList::ObjectWrapper& list, const std::shared_ptr<Config>& config) {
  stream->writeChar('[');
  bool first = true;
  auto curr = list->getFirstNode();
  
  while(curr != nullptr){
    auto value = curr->getData();
    if(value || config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      writeValue(stream, curr->getData(), config);
    }
    curr = curr->getNext();
  }
  
  stream->writeChar(']');
}

void Serializer::writeFieldsMap(oatpp::data::stream::ConsistentOutputStream* stream, const AbstractFieldsMap::ObjectWrapper& map, const std::shared_ptr<Config>& config) {
  stream->writeChar('{');
  bool first = true;
  auto curr = map->getFirstEntry();
  
  while(curr != nullptr){
    auto value = curr->getValue();
    if(value || config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      auto key = curr->getKey();
      writeString(stream, key->getData(), key->getSize());
      stream->write(": ", 2);
      writeValue(stream, curr->getValue(), config);
    }
    curr = curr->getNext();
  }
  
  stream->writeChar('}');
}

void Serializer::writeObject(oatpp::data::stream::ConsistentOutputStream* stream, const PolymorphicWrapper<Object>& polymorph, const std::shared_ptr<Config>& config) {
  
  stream->writeChar('{');
  
  bool first = true;
  auto fields = polymorph.valueType->properties->getList();
  Object* object = polymorph.get();
  
  for (auto const& field : fields) {
    
    auto value = field->get(object);
    if(value || config->includeNullFields) {
      (first) ? first = false : stream->write(", ", 2);
      writeString(stream, field->name);
      stream->write(": ", 2);
      writeValue(stream, value, config);
    }
    
  }
  
  stream->writeChar('}');
  
}
  
void Serializer::writeValue(oatpp::data::stream::ConsistentOutputStream* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config) {

  if(!polymorph) {
    stream->write("null", 4);
    return;
  }
  
  const char* typeName = polymorph.valueType->name;
  
  if(typeName == oatpp::data::mapping::type::__class::String::CLASS_NAME) {
    auto str = oatpp::data::mapping::type::static_wrapper_cast<oatpp::base::StrBuffer>(polymorph);
    writeString(stream, str->getData(), str->getSize());
  } else if(typeName == oatpp::data::mapping::type::__class::Int8::CLASS_NAME) {
    writeSimpleData(stream, oatpp::data::mapping::type::static_wrapper_cast<Int8::ObjectType>(polymorph));
  } else if(typeName == oatpp::data::mapping::type::__class::Int16::CLASS_NAME) {
    writeSimpleData(stream, oatpp::data::mapping::type::static_wrapper_cast<Int16::ObjectType>(polymorph));
  } else if(typeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME) {
    writeSimpleData(stream, oatpp::data::mapping::type::static_wrapper_cast<Int32::ObjectType>(polymorph));
  } else if(typeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME) {
    writeSimpleData(stream, oatpp::data::mapping::type::static_wrapper_cast<Int64::ObjectType>(polymorph));
  } else if(typeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME) {
    writeSimpleData(stream, oatpp::data::mapping::type::static_wrapper_cast<Float32::ObjectType>(polymorph));
  } else if(typeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME) {
    writeSimpleData(stream, oatpp::data::mapping::type::static_wrapper_cast<Float64::ObjectType>(polymorph));
  } else if(typeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME) {
    writeSimpleData(stream, oatpp::data::mapping::type::static_wrapper_cast<Boolean::ObjectType>(polymorph));
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
    writeList(stream, oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(polymorph), config);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
    // TODO Assert that key is String
    writeFieldsMap(stream, oatpp::data::mapping::type::static_wrapper_cast<AbstractFieldsMap>(polymorph), config);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
    writeObject(stream, oatpp::data::mapping::type::static_wrapper_cast<Object>(polymorph), config);
  } else {
    if(config->throwOnUnknownTypes) {
      throw std::runtime_error("[oatpp::kafka::protocol::mapping::Serializer::writeField]: Unknown data type");
    } else {
      writeString(stream, "<unknown-type>");
    }
  }
  
}
  
}}}}
