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

#include "MemoryLabel.hpp"

#include <cstring>

namespace oatpp { namespace data { namespace share {

MemoryLabel::MemoryLabel(const std::shared_ptr<std::string>& memHandle, const void* data, v_buff_size size)
  : m_memoryHandle(memHandle)
  , m_data(data)
  , m_size(size)
{}
  
StringKeyLabel::StringKeyLabel(const std::shared_ptr<std::string>& memHandle, const char* data, v_buff_size size)
  : oatpp::data::share::MemoryLabel(memHandle, data, size)
{}
  
StringKeyLabel::StringKeyLabel(const char* constText)
  : oatpp::data::share::MemoryLabel(nullptr, constText, static_cast<v_buff_size>(std::strlen(constText)))
{}
  
StringKeyLabel::StringKeyLabel(const String& str)
  : oatpp::data::share::MemoryLabel(str.getPtr(), str->data(), static_cast<v_buff_size>(str->size()))
{}
  
StringKeyLabelCI::StringKeyLabelCI(const std::shared_ptr<std::string>& memHandle, const char* data, v_buff_size size)
  : oatpp::data::share::MemoryLabel(memHandle, data, size)
{}

StringKeyLabelCI::StringKeyLabelCI(const char* constText)
  : oatpp::data::share::MemoryLabel(nullptr, constText, static_cast<v_buff_size>(std::strlen(constText)))
{}

StringKeyLabelCI::StringKeyLabelCI(const String& str)
  : oatpp::data::share::MemoryLabel(str.getPtr(), str->data(), static_cast<v_buff_size>(str->size()))
{}
  
}}}
