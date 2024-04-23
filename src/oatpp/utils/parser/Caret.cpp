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

#include "Caret.hpp"

#include <cstdlib>
#include <algorithm>

namespace oatpp { namespace utils { namespace parser {
  
  const char* const Caret::ERROR_INVALID_INTEGER = "ERROR_INVALID_INTEGER";
  const char* const Caret::ERROR_INVALID_FLOAT = "ERROR_INVALID_FLOAT";
  const char* const Caret::ERROR_INVALID_BOOLEAN = "ERROR_INVALID_BOOLEAN";
  const char* const Caret::ERROR_NO_OPEN_TAG = "ERROR_NO_OPEN_TAG";
  const char* const Caret::ERROR_NO_CLOSE_TAG = "ERROR_NO_CLOSE_TAG";
  const char* const Caret::ERROR_NAME_EXPECTED = "ERROR_NAME_EXPECTED";

/////////////////////////////////////////////////////////////////////////////////
// Caret::Label

  Caret::Label::Label(Caret* caret)
    : m_caret(caret)
    , m_start((caret != nullptr) ? caret->m_pos : -1)
    , m_end(-1)
  {}

  void Caret::Label::start() {
    m_start = m_caret->m_pos;
    m_end = -1;
  }

  void Caret::Label::end() {
    m_end = m_caret->m_pos;
  }

  const char* Caret::Label::getData(){
    return &m_caret->m_data[m_start];
  }

  v_buff_size Caret::Label::getSize(){
    if(m_end == -1) {
      return m_caret->m_pos - m_start;
    }
    return m_end - m_start;
  }

  v_buff_size Caret::Label::getStartPosition() {
    return m_start;
  }

  v_buff_size Caret::Label::getEndPosition() {
    return m_end;
  }

  oatpp::String Caret::Label::toString(){
    v_buff_size end = m_end;
    if(end == -1){
      end = m_caret->m_pos;
    }
    return oatpp::String(&m_caret->m_data[m_start], end - m_start);
  }

  std::string Caret::Label::std_str(){
    v_buff_size end = m_end;
    if(end == -1){
      end = m_caret->m_pos;
    }
    return std::string(&m_caret->m_data[m_start], static_cast<size_t>(end - m_start));
  }

  Caret::Label::operator bool() const {
    return m_caret != nullptr;
  }

/////////////////////////////////////////////////////////////////////////////////
// Caret::StateSaveGuard

Caret::StateSaveGuard::StateSaveGuard(Caret& caret)
  : m_caret(caret)
  , m_savedPosition(caret.m_pos)
  , m_savedErrorMessage(caret.m_errorMessage)
  , m_savedErrorCode(caret.m_errorCode)
{}

Caret::StateSaveGuard::~StateSaveGuard() {
  m_caret.m_pos = m_savedPosition;
  m_caret.m_errorMessage = m_savedErrorMessage;
  m_caret.m_errorCode = m_savedErrorCode;
}

v_buff_size Caret::StateSaveGuard::getSavedPosition() {
  return m_savedPosition;
}

const char* Caret::StateSaveGuard::getSavedErrorMessage() {
  return m_savedErrorMessage;
}

v_int64 Caret::StateSaveGuard::getSavedErrorCode() {
  return m_savedErrorCode;
}

/////////////////////////////////////////////////////////////////////////////////
// Caret

  Caret::Caret(const char* text)
    : Caret(text, static_cast<v_buff_size>(std::strlen(text)))
  {}
  
  Caret::Caret(const char* parseData, v_buff_size dataSize)
    : m_data(parseData)
    , m_size(dataSize)
    , m_pos(0)
    , m_errorMessage(nullptr)
    , m_errorCode(0)
  {}
  
  Caret::Caret(const oatpp::String& str)
    : Caret(str->data(), static_cast<v_buff_size>(str->size()))
  {
    m_dataMemoryHandle = str.getPtr();
  }
  
  std::shared_ptr<Caret> Caret::createShared(const char* text){
    return std::make_shared<Caret>(text);
  }
  
  std::shared_ptr<Caret> Caret::createShared(const char* parseData, v_buff_size dataSize){
    return std::make_shared<Caret>(parseData, dataSize);
  }
  
  std::shared_ptr<Caret> Caret::createShared(const oatpp::String& str){
    return std::make_shared<Caret>(str->data(), str->size());
  }
  
  Caret::~Caret(){
  }
  
  const char* Caret::getData(){
    return m_data;
  }
  
  const char* Caret::getCurrData(){
    return &m_data[m_pos];
  }
  
  v_buff_size Caret::getDataSize(){
    return m_size;
  }

  std::shared_ptr<std::string> Caret::getDataMemoryHandle() {
    return m_dataMemoryHandle;
  }

  void Caret::setPosition(v_buff_size position){
    m_pos = position;
  }
  
  v_buff_size Caret::getPosition(){
    return m_pos;
  }
  
  void Caret::setError(const char* errorMessage, v_int64 errorCode){
    m_errorMessage = errorMessage;
    m_errorCode = errorCode;
  }
  
  const char* Caret::getErrorMessage() {
    return m_errorMessage;
  }

  v_int64 Caret::getErrorCode() {
    return m_errorCode;
  }
  
  bool Caret::hasError() {
    return m_errorMessage != nullptr;
  }
  
  void Caret::clearError() {
    m_errorMessage = nullptr;
    m_errorCode = 0;
  }

  Caret::Label Caret::putLabel() {
    return Label(this);
  }
  
  void Caret::inc(){
    m_pos ++;
  }
  
  void Caret::inc(v_buff_size amount){
    m_pos+= amount;
  }
  
  bool Caret::skipBlankChars(){
    
    while(m_pos < m_size){
      char a = m_data[m_pos];
      if(a != ' ' && a != '\t' && a != '\n' && a != '\r' && a != '\f')
        return true;
      m_pos ++;
    }
    
    return false;
  }
  
  bool Caret::skipChar(char c) {
    while(m_pos < m_size){
      if(m_data[m_pos] != c)
        return true;
      m_pos ++;
    }
    return false;
  }
  
  bool Caret::findChar(char c){
    
    while(m_pos < m_size){
      if(m_data[m_pos] == c)
        return true;
      m_pos ++;
    }
    
    return false;
  }
  
  bool Caret::skipCharsFromSet(const char* set){
    return skipCharsFromSet(set, static_cast<v_buff_size>(std::strlen(set)));
  }
  
  bool Caret::skipCharsFromSet(const char* set, v_buff_size setSize){
    
    while(m_pos < m_size){
      if(!isAtCharFromSet(set, setSize)){
        return true;
      }
      m_pos++;
    }
    
    return false;
    
  }
  
  v_buff_size Caret::findCharFromSet(const char* set){
    return findCharFromSet(set, static_cast<v_buff_size>(std::strlen(set)));
  }
  
  v_buff_size Caret::findCharFromSet(const char* set, v_buff_size setSize){
    
    while(m_pos < m_size){
      
      char a = m_data[m_pos];
      
      for(v_buff_size i = 0; i < setSize; i++){
        if(set[i] == a)
          return a;
      }
      
      m_pos ++;
    }
    
    return -1;
    
  }
  
  bool Caret::findRN() {
    
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
  
  bool Caret::skipRN() {
    if(m_pos + 1 < m_size && m_data[m_pos] == '\r' && m_data[m_pos + 1] == '\n'){
      m_pos += 2;
      return true;
    }
    return false;
  }
  
  bool Caret::isAtRN() {
    return (m_pos + 1 < m_size && m_data[m_pos] == '\r' && m_data[m_pos + 1] == '\n');
  }


  bool Caret::findROrN() {
    while(m_pos < m_size) {
      char a = m_data[m_pos];
      if(a == '\r' || a == '\n') {
        return true;
      }
      m_pos ++;
    }
    return false;
  }

  bool Caret::skipRNOrN() {
    if(m_pos < m_size - 1 && m_data[m_pos] == '\r' && m_data[m_pos + 1] == '\n') {
      m_pos += 2;
      return true;
    }
    if(m_pos < m_size && m_data[m_pos] == '\n') {
      m_pos ++;
      return true;
    }
    return false;
  }

  bool Caret::skipAllRsAndNs() {
    bool skipped = false;
    while(m_pos < m_size) {
      char a = m_data[m_pos];
      if(a == '\r' || a == '\n') {
        m_pos ++;
        skipped = true;
      } else {
        break;
      }
    }
    return skipped;
  }

  v_int64 Caret::parseInt(int base) {
    char* end;
    char* start = const_cast<char*>(&m_data[m_pos]);
    v_int64 result = static_cast<v_int64>(std::strtoll(start, &end, base));
    if(start == end){
      m_errorMessage = ERROR_INVALID_INTEGER;
    }
    m_pos = (reinterpret_cast<v_buff_size>(end) - reinterpret_cast<v_buff_size>(m_data));
    return result;
  }

  v_uint64 Caret::parseUnsignedInt(int base) {
    char* end;
    char* start = const_cast<char*>(&m_data[m_pos]);
    v_uint64 result = static_cast<v_uint64>(std::strtoull(start, &end, base));
    if(start == end){
      m_errorMessage = ERROR_INVALID_INTEGER;
    }
    m_pos = (reinterpret_cast<v_buff_size>(end) - reinterpret_cast<v_buff_size>(m_data));
    return result;
  }
  
  v_float32 Caret::parseFloat32(){
    char* end;
    char* start = const_cast<char*>(&m_data[m_pos]);
    v_float32 result = std::strtof(start , &end);
    if(start == end){
      m_errorMessage = ERROR_INVALID_FLOAT;
    }
    m_pos = (reinterpret_cast<v_buff_size>(end) - reinterpret_cast<v_buff_size>(m_data));
    return result;
  }
  
  v_float64 Caret::parseFloat64(){
    char* end;
    char* start = const_cast<char*>(&m_data[m_pos]);
    v_float64 result = std::strtod(start , &end);
    if(start == end){
      m_errorMessage = ERROR_INVALID_FLOAT;
    }
    m_pos = (reinterpret_cast<v_buff_size>(end) - reinterpret_cast<v_buff_size>(m_data));
    return result;
  }
  
  bool Caret::isAtText(const char* text, bool skipIfTrue){
    return isAtText(text, static_cast<v_buff_size>(std::strlen(text)), skipIfTrue);
  }
  
  bool Caret::isAtText(const char* text, v_buff_size textSize, bool skipIfTrue){
    
    if(textSize <= m_size - m_pos){
      
      for(v_buff_size i = 0; i < textSize; i++){
        
        if(text[i] != m_data[m_pos + i]){
          return false;
        }
        
      }

      if(skipIfTrue) {
        m_pos = m_pos + textSize;
      }
      
      return true;
      
    }else{
      return false;
    }
    
  }
  
  bool Caret::isAtTextNCS(const char* text, bool skipIfTrue){
    return isAtTextNCS(text, static_cast<v_buff_size>(std::strlen(text)), skipIfTrue);
  }
  
  bool Caret::isAtTextNCS(const char* text, v_buff_size textSize, bool skipIfTrue){
    
    if(textSize <= m_size - m_pos){
      
      for(v_buff_size i = 0; i < textSize; i++){
        
        char c1 = text[i];
        char c2 = m_data[m_pos + i];
        
        if(c1 >= 'a' && c1 <= 'z'){
          c1 = static_cast<char>(c1 - 'a' + 'A');
        }
        
        if(c2 >= 'a' && c2 <= 'z'){
          c2 = static_cast<char>(c2 - 'a' + 'A');
        }
        
        if(c1 != c2){
          return false;
        }
        
      }

      if(skipIfTrue) {
        m_pos = m_pos + textSize;
      }
      
      return true;
      
    }else{
      return false;
    }
    
  }
  
  Caret::Label Caret::parseStringEnclosed(char openChar, char closeChar, char escapeChar){
    
    if(canContinueAtChar(openChar, 1)){
      
      auto label = putLabel();
      while(canContinue()){

        char a = m_data[m_pos];

        if(a == escapeChar){
          m_pos++;
        } else if(a == closeChar){
          label.end();
          m_pos++;
          return label;
        }
        
        m_pos++;
      }

      m_errorMessage = ERROR_NO_CLOSE_TAG;

    }else{
      m_errorMessage = ERROR_NO_OPEN_TAG;
    }
    
    return Label(nullptr);
    
  }

  bool Caret::findText(const char* text) {
    return findText(text, static_cast<v_buff_size>(std::strlen(text)));
  }
  
  bool Caret::findText(const char* text, v_buff_size textSize) {
    m_pos = (std::search(&m_data[m_pos], &m_data[m_size], text, text + textSize) - m_data);
    return m_pos != m_size;
  }

  bool Caret::isAtCharFromSet(const char* set) const{
    return isAtCharFromSet(set, static_cast<v_buff_size>(std::strlen(set)));
  }
  
  bool Caret::isAtCharFromSet(const char* set, v_buff_size setSize) const{
    
    char a = m_data[m_pos];
    
    for(v_buff_size i = 0; i < setSize; i++){
      if(a == set[i]){
        return true;
      }
    }
    
    return false;
    
  }
  
  bool Caret::isAtChar(char c) const{
    return m_data[m_pos] == c;
  }
  
  bool Caret::isAtBlankChar() const{
    char a = m_data[m_pos];
    return (a == ' ' || a == '\t' || a == '\n' || a == '\r' || a == '\b' || a == '\f');
  }
  
  bool Caret::isAtDigitChar() const{
    char a = m_data[m_pos];
    return (a >= '0' && a <= '9');
  }
  
  bool Caret::canContinueAtChar(char c) const{
    return m_pos < m_size && m_errorMessage == nullptr && m_data[m_pos] == c;
  }
  
  bool Caret::canContinueAtChar(char c, v_buff_size skipChars){
    
    if(m_pos < m_size && m_errorMessage == nullptr && m_data[m_pos] == c){
      m_pos = m_pos + skipChars;
      return true;
    }
    return false;
  }
  
  bool Caret::canContinue() const{
    return m_pos < m_size && m_errorMessage == nullptr;
  }
  
}}}
