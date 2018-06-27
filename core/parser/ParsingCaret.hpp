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

#ifndef oatpp_parser_ParsingCaret_hpp
#define oatpp_parser_ParsingCaret_hpp

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/base/String.hpp"

namespace oatpp { namespace parser {
  
class ParsingCaret : public base::Controllable{
public:
  static const char* const ERROR_INVALID_INTEGER;
  static const char* const ERROR_INVALID_FLOAT;
  static const char* const ERROR_INVALID_BOOLEAN;
  static const char* const ERROR_NO_OPEN_TAG;
  static const char* const ERROR_NO_CLOSE_TAG;
  static const char* const ERROR_NAME_EXPECTED;
public:
  class Label {
  private:
    ParsingCaret& m_caret;
    v_int32 m_start;
    v_int32 m_end;
  public:
    
    Label(ParsingCaret& caret)
      : m_caret(caret)
      , m_start(caret.m_pos)
      , m_end(-1)
    {}
    
    void start() {
      m_start = m_caret.m_pos;
      m_end = -1;
    }
    
    void end() {
      m_end = m_caret.m_pos;
    }
    
    p_char8 getData(){
      return &m_caret.m_data[m_start];
    }
    
    v_int32 getSize(){
      if(m_end == -1) {
        return m_caret.m_pos - m_start;
      }
      return m_end - m_start;
    }
    
    std::shared_ptr<oatpp::base::String> toString(bool saveAsOwnData){
      v_int32 end = m_end;
      if(end == -1){
        end = m_caret.m_pos;
      }
      return oatpp::base::String::createShared(&m_caret.m_data[m_start], end - m_start, saveAsOwnData);
    }
    
    std::shared_ptr<oatpp::base::String> toString(){
      return toString(true);
    }
    
    std::string std_str(){
      v_int32 end = m_end;
      if(end == -1){
        end = m_caret.m_pos;
      }
      return std::string((const char*) (&m_caret.m_data[m_start]), end - m_start);
    }
    
  };
private:
  p_char8		m_data;
  v_int32		m_size;
  v_int32		m_pos;
  const char* m_error;
public:
  ParsingCaret(const char* text);
  ParsingCaret(p_char8 parseData, v_int32 dataSize);
  ParsingCaret(const std::shared_ptr<base::String>& str);
public:
  
  static std::shared_ptr<ParsingCaret> createShared(const char* text);
  static std::shared_ptr<ParsingCaret> createShared(p_char8 parseData, v_int32 dataSize);
  static std::shared_ptr<ParsingCaret> createShared(const std::shared_ptr<base::String>& str);

  virtual ~ParsingCaret();
  
  p_char8 getData();
  p_char8 getCurrData();
  v_int32 getSize();
  
  void setPosition(v_int32 position);
  v_int32 getPosition();

  void setError(const char* error);
  const char* getError();
  bool hasError();
  void clearError();
  
  void inc();
  void inc(v_int32 amount);
  
  bool findNotBlankChar(); //  findCharNotFromSet(" \n\r\t");
  bool skipChar(v_char8 c);
  
  bool findNotSpaceChar();
  bool findChar(v_char8 c);
  
  bool findCharNotFromSet(const char* set);
  bool findCharNotFromSet(p_char8 set, v_int32 setSize);
  
  v_int32 findCharFromSet(const char* set);
  v_int32 findCharFromSet(p_char8 set, v_int32 setSize);
  
  bool findNextLine();
  bool findRN();
  bool skipRN();
  bool isAtRN();
  
  v_int32 parseInteger(bool allowNegative); // Will setError if error
  
  v_int32 parseInt32();
  v_int64 parseInt64();
  
  v_float32 parseFloat32();
  v_float64 parseFloat64();
  
  bool parseBoolean(const char* trueText, const char* falseText); // will setError if error
  
  bool proceedIfFollowsText(const char* text); // not increases pos if false
  bool proceedIfFollowsText(p_char8 text, v_int32 textSize); // not increases pos if false
  
  bool proceedIfFollowsTextNCS(const char* text); // NotCaseSensative
  bool proceedIfFollowsTextNCS(p_char8 text, v_int32 textSize); // not increases pos if false
  
  bool proceedIfFollowsWord(const char* text); // not increases pos if false
  bool proceedIfFollowsWord(p_char8 text, v_int32 textSize); // not increases pos if false
  
  std::shared_ptr<base::String> parseStringEnclosed(char openChar, char closeChar, char escapeChar, bool saveAsOwnData);
  std::shared_ptr<base::String> parseName(bool saveAsOwnData);
  
  bool findText(p_char8 text, v_int32 textSize);
  
  std::shared_ptr<base::String> findTextFromList(const std::shared_ptr<oatpp::collection::LinkedList<std::shared_ptr<base::String>>>& list);
  
  bool notAtCharFromSet(const char* set) const;
  bool notAtCharFromSet(p_char8 set, v_int32 setSize) const;
  bool isAtChar(v_char8 c) const;
  bool isAtBlankChar() const;
  bool isAtDigitChar() const;
  bool canContinueAtChar(v_char8 c) const;
  bool canContinueAtChar(v_char8 c, v_int32 skipChars);
  
  bool canContinue() const;
  bool isEnd() const;

};
  
}}



#endif /* oatpp_parser_ParsingCaret_hpp */
