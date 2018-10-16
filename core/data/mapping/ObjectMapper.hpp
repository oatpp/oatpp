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

#ifndef oatpp_data_mapping_ObjectMapper_hpp
#define oatpp_data_mapping_ObjectMapper_hpp

#include "type/Object.hpp"
#include "type/Object.hpp"
#include "type/Type.hpp"

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

#include "oatpp/core/parser/ParsingCaret.hpp"

namespace oatpp { namespace data { namespace mapping {
  
class ObjectMapper {
public:
  class Info {
  public:
    Info(const char* _http_content_type)
      : http_content_type(_http_content_type)
    {}
    const char* const http_content_type;
  };
private:
  Info m_info;
public:
  
  ObjectMapper(const Info& info)
    : m_info(info)
  {}
  
  const Info& getInfo() const {
    return m_info;
  }
  
  virtual void write(const std::shared_ptr<oatpp::data::stream::OutputStream>& stream,
                     const type::AbstractObjectWrapper& variant) const = 0;
  
  virtual type::AbstractObjectWrapper read(oatpp::parser::ParsingCaret& caret,
                                           const type::Type* const type) const = 0;
  
  oatpp::String writeToString(const type::AbstractObjectWrapper& variant) const {
    auto stream = stream::ChunkedBuffer::createShared();
    write(stream, variant);
    return stream->toString();
  }
  
  template<class Class>
  typename Class::ObjectWrapper readFromCaret(oatpp::parser::ParsingCaret& caret) const {
    auto type = Class::ObjectWrapper::Class::getType();
    return oatpp::data::mapping::type::static_wrapper_cast<typename Class::ObjectWrapper::ObjectType>(read(caret, type));
  }
  
  template<class Class>
  typename Class::ObjectWrapper readFromString(const oatpp::String& str) const {
    auto type = Class::ObjectWrapper::Class::getType();
    oatpp::parser::ParsingCaret caret(str);
    auto result = oatpp::data::mapping::type::static_wrapper_cast<typename Class::ObjectWrapper::ObjectType>(read(caret, type));
    if(caret.hasError()) {
      OATPP_LOGD("test", "caret->hasError '%s', %d", caret.getError(), caret.getPosition());
    }
    return result;
  }
  
};
  
}}}

#endif /* oatpp_data_mapping_ObjectMapper_hpp */
