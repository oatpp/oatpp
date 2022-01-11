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

#include "Pattern.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace url { namespace mapping {

const char* Pattern::Part::FUNCTION_CONST = "const";
const char* Pattern::Part::FUNCTION_VAR = "var";
const char* Pattern::Part::FUNCTION_ANY_END = "tail";

std::shared_ptr<Pattern> Pattern::parse(p_char8 data, v_buff_size size){
  
  if(size <= 0){
    return nullptr;
  }
  
  auto result = Pattern::createShared();

  v_buff_size lastPos = 0;
  v_buff_size i = 0;
  
  while(i < size){
    
    v_char8 a = data[i];
    
    if(a == '/'){
      
      if(i - lastPos > 0){
        auto part = Part::createShared(Part::FUNCTION_CONST, oatpp::String((const char*)&data[lastPos], i - lastPos));
        result->m_parts->push_back(part);
      }
      
      lastPos = i + 1;
      
    } else if(a == '*'){
      lastPos = i + 1;
      if(size > lastPos){
        auto part = Part::createShared(Part::FUNCTION_ANY_END, oatpp::String((const char*)&data[lastPos], size - lastPos));
        result->m_parts->push_back(part);
      }else{
        auto part = Part::createShared(Part::FUNCTION_ANY_END, oatpp::String((v_buff_size)0));
        result->m_parts->push_back(part);
      }
      return result;
    
    } else if(a == '{'){
      
      lastPos = i + 1;
      while(i < size && data[i] != '}'){
        i++;
      }
      
      if(i > lastPos){
        auto part = Part::createShared(Part::FUNCTION_VAR, oatpp::String((const char*)&data[lastPos], i - lastPos));
        result->m_parts->push_back(part);
      }else{
        auto part = Part::createShared(Part::FUNCTION_VAR, oatpp::String((v_buff_size)0));
        result->m_parts->push_back(part);
      }
      
      lastPos = i + 1;
      
    }
    
    i++;
    
  }
  
  if(i - lastPos > 0){
    auto part = Part::createShared(Part::FUNCTION_CONST, oatpp::String((const char*)&data[lastPos], i - lastPos));
    result->m_parts->push_back(part);
  }
  
  return result;
}

std::shared_ptr<Pattern> Pattern::parse(const char* data){
  return parse((p_char8) data, std::strlen(data));
}

std::shared_ptr<Pattern> Pattern::parse(const oatpp::String& data){
  return parse((p_char8) data->data(), data->size());
}
  
v_char8 Pattern::findSysChar(oatpp::parser::Caret& caret) {
  auto data = caret.getData();
  for (v_buff_size i = caret.getPosition(); i < caret.getDataSize(); i++) {
    v_char8 a = data[i];
    if(a == '/' || a == '?') {
      caret.setPosition(i);
      return a;
    }
  }
  caret.setPosition(caret.getDataSize());
  return 0;
}
  
bool Pattern::match(const StringKeyLabel& url, MatchMap& matchMap) {
  
  oatpp::parser::Caret caret((const char*) url.getData(), url.getSize());
  
  if (m_parts->empty()) {
    return !caret.skipChar('/');    
  }
  
  auto curr = std::begin(*m_parts);
  const auto end = std::end(*m_parts);
  while(curr != end){
    const std::shared_ptr<Part>& part = *curr;
    ++curr;
    caret.skipChar('/');
    
    if(part->function == Part::FUNCTION_CONST){
      
      if(!caret.isAtText(part->text->data(), part->text->size(), true)) {
        return false;
      }
      
      if(caret.canContinue() && !caret.isAtChar('/')){
        if(caret.isAtChar('?') && (curr == end || (*curr)->function == Part::FUNCTION_ANY_END)) {
          matchMap.m_tail = StringKeyLabel(url.getMemoryHandle(), caret.getCurrData(), caret.getDataSize() - caret.getPosition());
          return true;
        }
        return false;
      }
      
    }else if(part->function == Part::FUNCTION_ANY_END){
      if(caret.getDataSize() > caret.getPosition()){
        matchMap.m_tail = StringKeyLabel(url.getMemoryHandle(), caret.getCurrData(), caret.getDataSize() - caret.getPosition());
      }
      return true;
    }else if(part->function == Part::FUNCTION_VAR){
      
      if(!caret.canContinue()){
        return false;
      }

      auto label = caret.putLabel();
      v_char8 a = findSysChar(caret);
      if(a == '?') {
        if(curr == end || (*curr)->function == Part::FUNCTION_ANY_END) {
          matchMap.m_variables[part->text] = StringKeyLabel(url.getMemoryHandle(), label.getData(), label.getSize());
          matchMap.m_tail = StringKeyLabel(url.getMemoryHandle(), caret.getCurrData(), caret.getDataSize() - caret.getPosition());
          return true;
        }
        caret.findChar('/');
      }
      
      matchMap.m_variables[part->text] = StringKeyLabel(url.getMemoryHandle(), label.getData(), label.getSize());
      
    }
    
  }
  
  caret.skipChar('/');
  return !caret.canContinue();
  
}

oatpp::String Pattern::toString() {
  oatpp::data::stream::BufferOutputStream stream;
  for (const std::shared_ptr<Part>& part : *m_parts) {
    if(part->function == Part::FUNCTION_CONST) {
      stream.writeSimple("/", 1);
      stream.writeSimple(part->text);
    } else if(part->function == Part::FUNCTION_VAR) {
      stream.writeSimple("/{", 2);
      stream.writeSimple(part->text);
      stream.writeSimple("}", 1);
    } else if(part->function == Part::FUNCTION_ANY_END) {
      stream.writeSimple("/*", 2);
    }
  }
  return stream.toString();
}
  
}}}}
