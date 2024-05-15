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

#ifndef oatpp_base_Log_hpp
#define oatpp_base_Log_hpp

#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/macro/basic.hpp"

namespace oatpp { namespace base {

class LogMessage {
private:

  struct Parameter {
    v_buff_size startPos;
    v_buff_size endPos;
  };

private:
  bool writeNextChunk();
private:
  oatpp::String m_msg;
  mutable data::stream::BufferOutputStream m_stream;
  v_uint64 m_currParam;
  std::vector<Parameter> m_params;
public:

  explicit LogMessage(const oatpp::String& msg);

  std::string toStdString() const;

  LogMessage& operator << (const char* str);
  LogMessage& operator << (bool value);

  LogMessage& operator << (char value);
  LogMessage& operator << (unsigned char value);
  LogMessage& operator << (short value);
  LogMessage& operator << (unsigned short value);
  LogMessage& operator << (int value);
  LogMessage& operator << (unsigned value);
  LogMessage& operator << (long value);
  LogMessage& operator << (unsigned long value);
  LogMessage& operator << (long long value);
  LogMessage& operator << (unsigned long long value);
  LogMessage& operator << (float value);
  LogMessage& operator << (double value);
  LogMessage& operator << (long double value);

  LogMessage& operator << (const oatpp::String& str);
  LogMessage& operator << (const Boolean& value);
  LogMessage& operator << (const Int8& value);
  LogMessage& operator << (const UInt8& value);
  LogMessage& operator << (const Int16& value);
  LogMessage& operator << (const UInt16& value);
  LogMessage& operator << (const Int32& value);
  LogMessage& operator << (const UInt32& value);
  LogMessage& operator << (const Int64& value);
  LogMessage& operator << (const UInt64& value);
  LogMessage& operator << (const Float32& value);
  LogMessage& operator << (const Float64& value);

};

struct Log {

  static void ignore(std::initializer_list<void*> list) {
    (void) list;
  }

  template<typename ... Types>
  static void stream(v_uint32 priority, const std::string& tag, const oatpp::String& message, Types... args) {
    oatpp::base::LogMessage msg(message);
    ignore({std::addressof(msg << args)...});
    log(priority, tag, msg);
  }

  template<typename ... Types>
  static void stream(v_uint32 priority, const LogCategory& category, const oatpp::String& message, Types... args) {
    oatpp::base::LogMessage msg(message);
    ignore({std::addressof(msg << args)...});
    log(priority, category, msg);
  }

  static void log(v_uint32 priority, const std::string& tag, const LogMessage& message);
  static void log(v_uint32 priority, const LogCategory& category, const LogMessage& message);

};

}}

////////////////////////////
////////////////////////////
////////////////////////////

#ifndef OATPP_DISABLE_LOGV

/**
 * Log message with &l:Logger::PRIORITY_V; <br>
 * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGV`*
 * @param TAG - message tag.
 * @param ...(1) - message.
 * @param ... - optional format parameter.
 */
#define OATPP_LOGv(TAG, ...) \
  oatpp::base::Log::stream(oatpp::Logger::PRIORITY_V, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGv(TAG, ...)
#endif

#ifndef OATPP_DISABLE_LOGD

/**
 * Log message with &l:Logger::PRIORITY_D; <br>
 * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGD`*
 * @param TAG - message tag.
 * @param ...(1) - message
 * @param ... - optional format parameter.
 */
#define OATPP_LOGd(TAG, ...) \
  oatpp::base::Log::stream(oatpp::Logger::PRIORITY_D, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGd(TAG, ...)
#endif

#ifndef OATPP_DISABLE_LOGI

/**
 * Log message with &l:Logger::PRIORITY_I; <br>
 * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGI`*
 * @param TAG - message tag.
 * @param ...(1) - message.
 * @param ... - optional format parameter.
 */
#define OATPP_LOGi(TAG, ...) \
  oatpp::base::Log::stream(oatpp::Logger::PRIORITY_I, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGi(TAG, ...)
#endif

#ifndef OATPP_DISABLE_LOGW

/**
 * Log message with &l:Logger::PRIORITY_W; <br>
 * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGW`*
 * @param TAG - message tag.
 * @param ...(1) - message.
 * @param ... - optional format parameter.
 */
#define OATPP_LOGw(TAG, ...) \
  oatpp::base::Log::stream(oatpp::Logger::PRIORITY_W, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGw(TAG, ...)
#endif

#ifndef OATPP_DISABLE_LOGE

/**
 * Log message with &l:Logger::PRIORITY_E; <br>
 * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGE`*
 * @param TAG - message tag.
 * @param ...(1) - message.
 * @param ... - optional format parameter.
 */
#define OATPP_LOGe(TAG, ...) \
  oatpp::base::Log::stream(oatpp::Logger::PRIORITY_E, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGe(TAG, ...)
#endif

//////////////////////
//////////////////////
//////////////////////


/**
 * Convenience macro to declare a logging category directly in a class header.
 * @param NAME - variable-name of the category which is later used to reference the category.
 */
#define OATPP_DECLARE_LOG_CATEGORY(NAME) \
  static oatpp::LogCategory NAME;

/**
 * Convenience macro to implement a logging category directly in a class header.
 * @param NAME - variable-name of the category which is later used to reference the category.
 * @param TAG - tag printed with each message printed usig this category.
 * @param ENABLED - enable or disable a category (bool).
 */
#define OATPP_LOG_CATEGORY(NAME, TAG, ENABLED) \
  oatpp::LogCategory NAME = oatpp::LogCategory(TAG, ENABLED);


//////////////////////
//////////////////////
//////////////////////

/**
 * Default oatpp assert method.
 * @param FMT - the format string used for the expression
 * @param EXP - expression that must be `true`.
 */
#define OATPP_ASSERT_FMT(FMT, EXP) \
if(!(EXP)) { \
  OATPP_LOGe("\033[1mASSERT\033[0m[\033[1;31mFAILED\033[0m]", FMT, #EXP) \
  exit(EXIT_FAILURE); \
}

/**
 * Default oatpp assert method.
 * @param EXP - expression that must be `true`.
 */
#define OATPP_ASSERT(EXP) \
if(!(EXP)) { \
  OATPP_LOGe("\033[1mASSERT\033[0m[\033[1;31mFAILED\033[0m]", #EXP) \
  exit(EXIT_FAILURE); \
}

#endif /* oatpp_base_Log_hpp */
