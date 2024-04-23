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

#ifndef oatpp_utils_parser_Caret_hpp
#define oatpp_utils_parser_Caret_hpp

#include "oatpp/Types.hpp"

namespace oatpp { namespace utils { namespace parser {

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

  /**
   * Class to label parsing data.
   */
  class Label {
  private:
    Caret* m_caret;
    v_buff_size m_start;
    v_buff_size m_end;
  public:

    /**
     * Constructor.
     * @param caret.
     */
    Label(Caret* caret);

    /**
     * Set current caret position as a starting point for label.
     */
    void start();

    /**
     * Fix current caret position as an end point for label.
     */
    void end();

    /**
     * Get pointer to a labeled data.
     * @return
     */
    const char* getData();

    /**
     * Get size of labeled data.
     * @return
     */
    v_buff_size getSize();

    /**
     * Get start position of the label.
     * @return
     */
    v_buff_size getStartPosition();

    /**
     * Get end position of the label.
     * @return - end position of the label or `-1` if end() wasn't called yet.
     */
    v_buff_size getEndPosition();

    /**
     * Same as`toString(true).`
     * @return - &id:oatpp::String;.
     */
    oatpp::String toString();

    /**
     * Create `std::string` from labeled data.
     * @return - `std::string`.
     */
    std::string std_str();

    explicit operator bool() const;
    
  };

  /**
   * Caret state saver guard.
   */
  class StateSaveGuard {
  private:
    Caret& m_caret;
    v_buff_size m_savedPosition;
    const char* m_savedErrorMessage;
    v_int64 m_savedErrorCode;
  public:

    /**
     * Constructor.
     * @param caret.
     */
    StateSaveGuard(Caret& caret);

    /**
     * Destructor. Restore saved state.
     */
    ~StateSaveGuard();

    /**
     * Get caret saved position.
     * @return
     */
    v_buff_size getSavedPosition();

    /**
     * Get caret saved error message.
     * @return
     */
    const char* getSavedErrorMessage();

    /**
     * Get caret saved error code.
     * @return
     */
    v_int64 getSavedErrorCode();

  };

private:
  const char* m_data;
  v_buff_size m_size;
  v_buff_size m_pos;
  const char* m_errorMessage;
  v_int64 m_errorCode;
  std::shared_ptr<std::string> m_dataMemoryHandle;
public:
  Caret(const char* text);
  Caret(const char* parseData, v_buff_size dataSize);
  Caret(const oatpp::String& str);
public:
  
  static std::shared_ptr<Caret> createShared(const char* text);
  static std::shared_ptr<Caret> createShared(const char* parseData, v_buff_size dataSize);
  static std::shared_ptr<Caret> createShared(const oatpp::String& str);

  virtual ~Caret();

  /**
   * Get pointer to a data, passed to Caret constructor
   * @return
   */
  const char* getData();

  /**
   * Same as &getData()[position]
   * @return
   */
  const char* getCurrData();

  /**
   * Get size of a data
   * @return
   */
  v_buff_size getDataSize();

  /**
   * Get data memoryHandle.
   * @return
   */
  std::shared_ptr<std::string> getDataMemoryHandle();

  /**
   * Set caret position relative to data
   * @param position
   */
  void setPosition(v_buff_size position);

  /**
   * Get caret position relative to data
   * @return
   */
  v_buff_size getPosition();

  /**
   * Set error message and error code.
   * Note that once error message is set, methods canContinue... will return false
   * @param errorMessage
   * @param errorCode
   */
  void setError(const char* errorMessage, v_int64 errorCode = 0);

  /**
   * Get error message
   * @return error message
   */
  const char* getErrorMessage();

  /**
   * Get error code
   * @return error code
   */
  v_int64 getErrorCode();

  /**
   * Check if error is set for the Caret
   * @return
   */
  bool hasError();

  /**
   * Clear error message and error code
   */
  void clearError();

  /**
   * Create Label(this);
   * @return Label
   */
  Label putLabel();

  /**
   * Increase caret position by one
   */
  void inc();

  /**
   * Increase caret position by amount
   * @param amount
   */
  void inc(v_buff_size amount);

  /**
   * Skip chars: [' ', '\t', '\n', '\r','\f']
   * @return true if other char found
   */
  bool skipBlankChars();

  /**
   * Skip char
   * @param c
   * @return true if other char found
   */
  bool skipChar(char c);

  /**
   * Find char. Position will be set to a found char. If
   * no such char found - position will be set to a dataSize;
   * @param c
   * @return true if found
   */
  bool findChar(char c);

  /**
   * Skip chars defined by set.
   * ex. skipCharsFromSet("abc") - will skip all 'a', 'b', 'c' chars
   * @param set
   * @return true if other char found
   */
  bool skipCharsFromSet(const char* set);

  /**
   * Skip chars defined by set.
   * ex. skipCharsFromSet("abc", 3) - will skip all 'a', 'b', 'c' chars
   * @param set
   * @param setSize
   * @return true if other char found
   */
  bool skipCharsFromSet(const char* set, v_buff_size setSize);

  /**
   * Find one of chars defined by set.
   * @param set
   * @return char found or -1 if no char found
   */
  v_buff_size findCharFromSet(const char* set);

  /**
   * Find one of chars defined by set.
   * @param set
   * @param setSize
   * @return char found or -1 if no char found
   */
  v_buff_size findCharFromSet(const char* set, v_buff_size setSize);

  /**
   * Find "\r\n" chars
   * @return true if found
   */
  bool findRN();

  /**
   * Skip "\r\n"
   * @return True if position changes. False if caret not at "\r\n"
   */
  bool skipRN();

  /**
   * Check if caret at "\r\n" chars
   * @return
   */
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
  v_int64 parseInt(int base = 10);

  /**
   * parse integer value starting from the current position.
   * Using function std::strtoul()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @param base - base is passed to std::strtoul function
   * @return parsed value
   */
  v_uint64 parseUnsignedInt(int base = 10);

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
  bool isAtText(const char* text, v_buff_size textSize, bool skipIfTrue = false);

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
  bool isAtTextNCS(const char* text, v_buff_size textSize, bool skipIfTrue = false);

  /**
   * Parse enclosed string.
   * ex. for data "'let\'s go'" parseStringEnclosed('\'', '\'', '\\')
   * will return Label to "let\'s go" without enclosing '\'' chars
   * @param openChar
   * @param closeChar
   * @param escapeChar
   * @return
   */
  Label parseStringEnclosed(char openChar, char closeChar, char escapeChar);

  /**
   * Find text and set position to found text
   * @param text
   * @return true if found
   */
  bool findText(const char* text);

  /**
   * Find text and set position to found text
   * @param text
   * @param textSize
   * @return true if found
   */
  bool findText(const char* text, v_buff_size textSize);

  /**
   * Check if caret is at char defined by set
   * ex. isAtCharFromSet("abc") - will return true for 'a', 'b', 'c' chars
   * @param set
   * @return
   */
  bool isAtCharFromSet(const char* set) const;

  /**
   * Check if caret is at char defined by set
   * ex. isAtCharFromSet("abc", 3) - will return true for 'a', 'b', 'c' chars
   * @param set
   * @param setSize
   * @return
   */
  bool isAtCharFromSet(const char* set, v_buff_size setSize) const;

  /**
   * Check if caret is at char
   * @param c
   * @return
   */
  bool isAtChar(char c) const;

  /**
   * Check if caret is at one of chars [' ', '\t', '\n', '\r','\f']
   * @return
   */
  bool isAtBlankChar() const;

  /**
   * Check if caret is at digit
   * @return
   */
  bool isAtDigitChar() const;

  /**
   * Check if caret is at char, and no error is set
   * @param c
   * @return
   */
  bool canContinueAtChar(char c) const;

  /**
   * Check if caret is at char, and no error is set.
   * If true inc position by skipChars
   * @param c
   * @param skipChars
   * @return
   */
  bool canContinueAtChar(char c, v_buff_size skipChars);

  /**
   * Check if caret position < dataSize and not error is set
   * @return
   */
  bool canContinue() const;

};
  
}}}

#endif /* oatpp_utils_parser_Caret_hpp */
