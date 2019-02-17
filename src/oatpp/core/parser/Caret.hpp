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

#ifndef oatpp_parser_Caret_hpp
#define oatpp_parser_Caret_hpp

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace parser {

/**
 * Helper class to do parsing operations
 */
class Caret {
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
    Caret* m_caret;
    v_int32 m_start;
    v_int32 m_end;
  public:
    
    Label(Caret& caret);
    
    void start();
    void end();
    p_char8 getData();
    v_int32 getSize();
    oatpp::String toString(bool saveAsOwnData);
    oatpp::String toString();
    std::string std_str();
    
  };

  class StateSaveGuard {
  private:
    Caret& m_caret;
    v_int32 m_savedPosition;
    const char* m_savedErrorMessage;
    v_int32 m_savedErrorCode;
  public:

    StateSaveGuard(Caret& caret);
    ~StateSaveGuard();

    v_int32 getSavedPosition();
    const char* getSavedErrorMessage();
    v_int32 getSavedErrorCode();

  };

private:
  p_char8 m_data;
  v_int32 m_size;
  v_int32 m_pos;
  const char* m_errorMessage;
  v_int32 m_errorCode;
public:
  Caret(const char* text);
  Caret(p_char8 parseData, v_int32 dataSize);
  Caret(const oatpp::String& str);
public:
  
  static std::shared_ptr<Caret> createShared(const char* text);
  static std::shared_ptr<Caret> createShared(p_char8 parseData, v_int32 dataSize);
  static std::shared_ptr<Caret> createShared(const oatpp::String& str);

  virtual ~Caret();
  
  p_char8 getData();
  p_char8 getCurrData();
  v_int32 getSize();
  
  void setPosition(v_int32 position);
  v_int32 getPosition();

  void setError(const char* errorMessage, v_int32 errorCode = 0);

  const char* getErrorMessage();
  v_int32 getErrorCode();
  bool hasError();
  void clearError();
  
  void inc();
  void inc(v_int32 amount);
  
  bool skipBlankChars();

  bool skipChar(v_char8 c);
  bool findChar(v_char8 c);
  
  bool findCharNotFromSet(const char* set);
  bool findCharNotFromSet(p_char8 set, v_int32 setSize);
  
  v_int32 findCharFromSet(const char* set);
  v_int32 findCharFromSet(p_char8 set, v_int32 setSize);

  bool findRN();
  bool skipRN();
  bool isAtRN();

  /**
   * Find '\r' char of '\n' char
   * @return true if found '\r' or '\n'
   */
  bool findROrN();

  /**
   * if at "\r\n" - skip.
   * if at "\n" - skip.
   * @return true if position changed
   */
  bool skipRNOrN();

  /**
   * skip any sequence of '\r' and '\n'
   * @return true if position changed
   */
  bool skipAllRsAndNs();

  /**
   * parse integer value starting from the current position.
   * Using function std::strtol()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @param base - base is passed to std::strtol function
   * @return parsed value
   */
  long int parseInt(int base = 10);

  /**
   * parse integer value starting from the current position.
   * Using function std::strtoul()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @param base - base is passed to std::strtoul function
   * @return parsed value
   */
  unsigned long int parseUnsignedInt(int base = 10);

  /**
   * parse float value starting from the current position.
   * Using function std::strtof()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @return parsed value
   */
  v_float32 parseFloat32();

  /**
   * parse float value starting from the current position.
   * Using function std::strtod()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @return parsed value
   */
  v_float64 parseFloat64();

  /**
   * Check if follows text
   * @param text
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtText(const char* text, bool skipIfTrue = false);

  /**
   * Check if follows text
   * @param text
   * @param textSize
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtText(p_char8 text, v_int32 textSize, bool skipIfTrue = false);

  /**
   * Check if follows text (Not Case Sensitive)
   * @param text
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtTextNCS(const char* text, bool skipIfTrue = false);

  /**
   * Check if follows text (Not Case Sensitive)
   * @param text
   * @param textSize
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtTextNCS(p_char8 text, v_int32 textSize, bool skipIfTrue = false);
  
  oatpp::String parseStringEnclosed(char openChar, char closeChar, char escapeChar, bool saveAsOwnData);
  oatpp::String parseName(bool saveAsOwnData);
  
  bool findText(p_char8 text, v_int32 textSize);
  
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



#endif /* oatpp_parser_Caret_hpp */
