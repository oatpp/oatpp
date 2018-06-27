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

#include "ParsingCaret.hpp"

#include <stdlib.h>
#include <cstdlib>
#include <algorithm>

namespace oatpp { namespace parser {
  
  const char* const ParsingCaret::ERROR_INVALID_INTEGER = "ERROR_INVALID_INTEGER";
  const char* const ParsingCaret::ERROR_INVALID_FLOAT = "ERROR_INVALID_FLOAT";
  const char* const ParsingCaret::ERROR_INVALID_BOOLEAN = "ERROR_INVALID_BOOLEAN";
  const char* const ParsingCaret::ERROR_NO_OPEN_TAG = "ERROR_NO_OPEN_TAG";
  const char* const ParsingCaret::ERROR_NO_CLOSE_TAG = "ERROR_NO_CLOSE_TAG";
  const char* const ParsingCaret::ERROR_NAME_EXPECTED = "ERROR_NAME_EXPECTED";
  
  ParsingCaret::ParsingCaret(const char* text)
    : m_data((p_char8)text)
    , m_size((v_int32) std::strlen(text))
    , m_pos(0)
    , m_error(nullptr)
  {}
  
  ParsingCaret::ParsingCaret(p_char8 parseData, v_int32 dataSize)
    : m_data(parseData)
    , m_size(dataSize)
    , m_pos(0)
    , m_error(nullptr)
  {}
  
  ParsingCaret::ParsingCaret(const oatpp::String& str)
    : m_data(str->getData())
    , m_size(str->getSize())
    , m_pos(0)
    , m_error(nullptr)
  {}
  
  std::shared_ptr<ParsingCaret> ParsingCaret::createShared(const char* text){
    return std::shared_ptr<ParsingCaret>(new ParsingCaret(text));
  }
  
  std::shared_ptr<ParsingCaret> ParsingCaret::createShared(p_char8 parseData, v_int32 dataSize){
    return std::shared_ptr<ParsingCaret>(new ParsingCaret(parseData, dataSize));
  }
  
  std::shared_ptr<ParsingCaret> ParsingCaret::createShared(const oatpp::String& str){
    return std::shared_ptr<ParsingCaret>(new ParsingCaret(str->getData(), str->getSize()));
  }
  
  ParsingCaret::~ParsingCaret(){
  }
  
  p_char8 ParsingCaret::getData(){
    return m_data;
  }
  
  p_char8 ParsingCaret::getCurrData(){
    return &m_data[m_pos];
  }
  
  v_int32 ParsingCaret::getSize(){
    return m_size;
  }
  
  void ParsingCaret::setPosition(v_int32 position){
    m_pos = position;
  }
  
  v_int32 ParsingCaret::getPosition(){
    return m_pos;
  }
  
  void ParsingCaret::setError(const char* error){
    m_error = error;
  }
  
  const char* ParsingCaret::getError(){
    return m_error;
  }
  
  bool ParsingCaret::hasError(){
    return m_error != nullptr;
  }
  
  void ParsingCaret::clearError(){
    m_error = nullptr;
  }
  
  void ParsingCaret::inc(){
    m_pos ++;
  }
  
  void ParsingCaret::inc(v_int32 amount){
    m_pos+= amount;
  }
  
  bool ParsingCaret::findNotBlankChar(){
    
    while(m_pos < m_size){
      v_char8 a = m_data[m_pos];
      if(a != ' ' && a != '\t' && a != '\n' && a != '\r' && a != '\f')
        return true;
      m_pos ++;
    }
    
    return false;
  }
  
  bool ParsingCaret::findNotSpaceChar(){
    while(m_pos < m_size){
      if(m_data[m_pos] != ' ')
        return true;
      m_pos ++;
    }
    return false;
  }
  
  bool ParsingCaret::skipChar(v_char8 c) {
    while(m_pos < m_size){
      if(m_data[m_pos] != c)
        return true;
      m_pos ++;
    }
    return false;
  }
  
  bool ParsingCaret::findChar(v_char8 c){
    
    while(m_pos < m_size){
      if(m_data[m_pos] == c)
        return true;
      m_pos ++;
    }
    
    return false;
  }
  
  bool ParsingCaret::findCharNotFromSet(const char* set){
    return findCharNotFromSet((p_char8)set, (v_int32) std::strlen(set));
  }
  
  bool ParsingCaret::findCharNotFromSet(p_char8 set, v_int32 setSize){
    
    while(m_pos < m_size){
      if(notAtCharFromSet(set, setSize)){
        return true;
      }
      m_pos++;
    }
    
    return false;
    
  }
  
  v_int32 ParsingCaret::findCharFromSet(const char* set){
    return findCharFromSet((p_char8) set, (v_int32) std::strlen(set));
  }
  
  v_int32 ParsingCaret::findCharFromSet(p_char8 set, v_int32 setSize){
    
    while(m_pos < m_size){
      
      v_char8 a = m_data[m_pos];
      
      for(v_int32 i = 0; i < setSize; i++){
        if(set[i] == a)
          return a;
      }
      
      m_pos ++;
    }
    
    return -1;
    
  }
  
  bool ParsingCaret::findNextLine(){
    
    bool nlFound = false;
    
    while(m_pos < m_size){
      
      if(nlFound){
        
        if(m_data[m_pos] != '\n' && m_data[m_pos] != '\r'){
          return true;
        }
        
      }else{
        
        if(m_data[m_pos] == '\n' || m_data[m_pos] == '\r'){
          nlFound = true;
        }
        
      }
      
      m_pos ++;
    }
    
    return false;
    
  }
  
  bool ParsingCaret::findRN() {
    
    while(m_pos < m_size){
      if(m_data[m_pos] == '\r'){
        if(m_pos + 1 < m_size && m_data[m_pos + 1] == '\n'){
          return true;
        }
      }
      m_pos ++;
    }
    
    return false;
  }
  
  bool ParsingCaret::skipRN() {
    if(m_pos + 1 < m_size && m_data[m_pos] == '\r' && m_data[m_pos + 1] == '\n'){
      m_pos += 2;
      return true;
    }
    return false;
  }
  
  bool ParsingCaret::isAtRN() {
    return (m_pos + 1 < m_size && m_data[m_pos] == '\r' && m_data[m_pos + 1] == '\n');
  }
  
  v_int32 ParsingCaret::parseInteger(bool allowNegative){
    
    bool negative = m_data[m_pos] == '-';
    
    if(negative){
      
      if(!allowNegative){
        m_error = ERROR_INVALID_INTEGER;
        return 0;
      }
      
      inc();
      if(findNotSpaceChar() == false){
        m_error = ERROR_INVALID_INTEGER;
        return 0;
      }
    }
    
    v_int32 ipos = m_pos;
    
    while(m_pos < m_size && m_data[m_pos] >= '0' && m_data[m_pos] <= '9'){
      m_pos ++;
    }
    
    v_int32 len = m_pos - ipos;
    
    if(len > 0){
      
      auto str = oatpp::String((const char*)&m_data[ipos], len, true);
      v_int32 result = atoi((const char*)str->getData());
      
      if(negative){
        result = - result;
      }
      
      return result;
      
    }else{
      m_error = ERROR_INVALID_INTEGER;
      return 0;
    }
    
  }
  
  v_int32 ParsingCaret::parseInt32(){
    char* end;
    char* start = (char*)&m_data[m_pos];
    v_int32 result = (v_float32) std::strtol(start, &end, 10);
    if(start == end){
      m_error = ERROR_INVALID_INTEGER;
    }
    m_pos = (v_int32)((v_int64) end - (v_int64) m_data);
    return result;
  }
  
  v_int64 ParsingCaret::parseInt64(){
    char* end;
    char* start = (char*)&m_data[m_pos];
    v_int64 result = std::strtol(start, &end, 10);
    if(start == end){
      m_error = ERROR_INVALID_INTEGER;
    }
    m_pos = (v_int32)((v_int64) end - (v_int64) m_data);
    return result;
  }
  
  v_float32 ParsingCaret::parseFloat32(){
    char* end;
    char* start = (char*)&m_data[m_pos];
    v_float32 result = std::strtof(start , &end);
    if(start == end){
      m_error = ERROR_INVALID_FLOAT;
    }
    m_pos = (v_int32)((v_int64) end - (v_int64) m_data);
    return result;
  }
  
  v_float64 ParsingCaret::parseFloat64(){
    char* end;
    char* start = (char*)&m_data[m_pos];
    v_float64 result = std::strtod(start , &end);
    if(start == end){
      m_error = ERROR_INVALID_FLOAT;
    }
    m_pos = (v_int32)((v_int64) end - (v_int64) m_data);
    return result;
  }
  
  bool ParsingCaret::parseBoolean(const char* trueText, const char* falseText){
    if(proceedIfFollowsText(trueText)){
      return true;
    } else if(proceedIfFollowsText(falseText)){
      return false;
    }
    setError(ERROR_INVALID_BOOLEAN);
    return false;
  }
  
  bool ParsingCaret::proceedIfFollowsText(const char* text){
    return proceedIfFollowsText((p_char8)text, (v_int32) std::strlen(text));
  }
  
  bool ParsingCaret::proceedIfFollowsText(p_char8 text, v_int32 textSize){
    
    if(textSize <= m_size - m_pos){
      
      for(v_int32 i = 0; i < textSize; i++){
        
        if(text[i] != m_data[m_pos + i]){
          return false;
        }
        
      }
      
      m_pos = m_pos + textSize;
      
      return true;
      
    }else{
      return false;
    }
    
  }
  
  bool ParsingCaret::proceedIfFollowsTextNCS(const char* text){
    return proceedIfFollowsTextNCS((p_char8)text, (v_int32) std::strlen(text));
  }
  
  bool ParsingCaret::proceedIfFollowsTextNCS(p_char8 text, v_int32 textSize){
    
    if(textSize <= m_size - m_pos){
      
      for(v_int32 i = 0; i < textSize; i++){
        
        v_char8 c1 = text[i];
        v_char8 c2 = m_data[m_pos + i];
        
        if(c1 >= 'a' && c1 <= 'z'){
          c1 = 'A' + c1 - 'a';
        }
        
        if(c2 >= 'a' && c2 <= 'z'){
          c2 = 'A' + c2 - 'a';
        }
        
        if(c1 != c2){
          return false;
        }
        
      }
      
      m_pos = m_pos + textSize;
      
      return true;
      
    }else{
      return false;
    }
    
  }
  
  bool ParsingCaret::proceedIfFollowsWord(const char* text){
    return proceedIfFollowsWord((p_char8)text, (v_int32) std::strlen(text));
  }
  
  bool ParsingCaret::proceedIfFollowsWord(p_char8 text, v_int32 textSize){
    
    if(textSize <= m_size - m_pos){
      
      for(v_int32 i = 0; i < textSize; i++){
        
        if(text[i] != m_data[m_pos + i]){
          return false;
        }
        
      }
      
      v_char8 a = m_data[m_pos + textSize];
      
      if(!(a >= '0' && a <= '9') && !(a >= 'a' && a <= 'z') &&
         !(a >= 'A' && a <= 'Z') && a != '_'){
        
        m_pos = m_pos + textSize;
        return true;
        
      }
      
    }
    
    return false;
    
  }
  
  oatpp::String ParsingCaret::parseStringEnclosed(char openChar, char closeChar, char escapeChar, bool saveAsOwnData){
    
    if(m_data[m_pos] == openChar){
      
      m_pos++;
      
      v_int32 ipos = m_pos;
      
      while(canContinue()){
        
        if(m_data[m_pos] == escapeChar){
          m_pos++;
        }else if(m_data[m_pos] == closeChar){
          oatpp::String result = oatpp::String((const char*)&m_data[ipos], m_pos - ipos, saveAsOwnData);
          m_pos++;
          return result;
        }
        
        m_pos++;
      }
      
      m_error = ERROR_NO_CLOSE_TAG;
      
    }else{
      m_error = ERROR_NO_OPEN_TAG;
    }
    
    return nullptr;
    
  }
  
  oatpp::String ParsingCaret::parseName(bool saveAsOwnData){
    
    v_int32 ipos = m_pos;
    while(m_pos < m_size){
      
      v_char8 a = m_data[m_pos];
      
      if(  (a >= '0' && a <= '9')||
         (a >= 'a' && a <= 'z')||
         (a >= 'A' && a <= 'Z')||
         (a == '_')){
        m_pos ++;
      }else{
        
        if(ipos < m_pos){
          return oatpp::String((const char*)&m_data[ipos], m_pos - ipos, saveAsOwnData);
        }else{
          m_error = ERROR_NAME_EXPECTED;
          return nullptr;
        }
        
      }
      
    }
    
    if(ipos < m_pos){
      return oatpp::String((const char*)&m_data[ipos], m_pos - ipos, saveAsOwnData);
    }else{
      m_error = ERROR_NAME_EXPECTED;
      return nullptr;
    }
    
  }
  
  bool ParsingCaret::findText(p_char8 text, v_int32 textSize) {
    m_pos = (v_int32)(std::search(&m_data[m_pos], &m_data[m_size], text, text + textSize) - m_data);
    return m_pos != m_size;
  }
  
  oatpp::String ParsingCaret::findTextFromList(const std::shared_ptr<oatpp::collection::LinkedList<oatpp::String>>& list){
    
    while(m_pos < m_size){
      
      auto currNode = list->getFirstNode();
      while(currNode != nullptr){
        auto str = currNode->getData();
        if(proceedIfFollowsText(str->getData(), str->getSize())){
          return str;
        }
        currNode = currNode->getNext();
      }
      
      m_pos++;
    }
    
    return nullptr;
    
  }
  
  bool ParsingCaret::notAtCharFromSet(const char* set) const{
    return notAtCharFromSet((p_char8)set, (v_int32) std::strlen(set));
  }
  
  bool ParsingCaret::notAtCharFromSet(p_char8 set, v_int32 setSize) const{
    
    v_char8 a = m_data[m_pos];
    
    for(v_int32 i = 0; i < setSize; i++){
      if(a == set[i]){
        return false;
      }
    }
    
    return true;
    
  }
  
  bool ParsingCaret::isAtChar(v_char8 c) const{
    return m_data[m_pos] == c;
  }
  
  bool ParsingCaret::isAtBlankChar() const{
    v_char8 a = m_data[m_pos];
    return (a == ' ' || a == '\t' || a == '\n' || a == '\r' || a == '\b' || a == '\f');
  }
  
  bool ParsingCaret::isAtDigitChar() const{
    v_char8 a = m_data[m_pos];
    return (a >= '0' && a <= '9');
  }
  
  bool ParsingCaret::canContinueAtChar(v_char8 c) const{
    return m_pos < m_size && m_error == 0 && m_data[m_pos] == c;
  }
  
  bool ParsingCaret::canContinueAtChar(v_char8 c, v_int32 skipChars){
    
    if(m_pos < m_size && m_error == 0 && m_data[m_pos] == c){
      m_pos = m_pos + skipChars;
      return true;
    }
    return false;
  }
  
  bool ParsingCaret::canContinue() const{
    return m_pos < m_size && m_error == 0;
  }
  
  bool ParsingCaret::isEnd() const{
    return m_pos >= m_size;
  }
  
  
}}
