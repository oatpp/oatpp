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

#ifndef oatpp_base_Environment_hpp
#define oatpp_base_Environment_hpp

#include <stdio.h>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <stdexcept>

#define OATPP_VERSION "0.18.6"

#define OATPP_ASSERT(EXP) \
if(!(EXP)) { \
  OATPP_LOGE("ASSERT[FAILED]", #EXP); \
  throw std::runtime_error("ASSERT[FAILED]:" #EXP); \
}

typedef unsigned char v_char8;
typedef v_char8 *p_char8;

typedef int64_t v_int64;
typedef v_int64* p_int64;
typedef uint64_t v_word64;
typedef v_word64* p_word64;

typedef int16_t v_int16;
typedef v_int16* p_int16;
typedef uint16_t v_word16;
typedef v_word16* p_word16;

typedef int32_t v_int32;
typedef v_int32* p_int32;
typedef uint32_t v_word32;
typedef v_word32* p_word32;

typedef double v_float64;
typedef v_float64 * p_float64;

typedef float v_float32;
typedef v_float32* p_float32;

typedef std::atomic_int_fast64_t v_atomicCounter;
typedef v_int64 v_counter;

namespace oatpp { namespace base{
  
class Logger {
public:
  virtual ~Logger(){};
  virtual void log(v_int32 priority, const std::string& tag, const std::string& message) = 0;
};
  
//#define OATPP_DISABLE_ENV_OBJECT_COUNTERS
  
class Environment{
private:
#ifndef OATPP_DISABLE_ENV_OBJECT_COUNTERS
  static v_atomicCounter m_objectsCount;
  static v_atomicCounter m_objectsCreated;
  static thread_local v_counter m_threadLocalObjectsCount;
  static thread_local v_counter m_threadLocalObjectsCreated;
#endif
private:
  static Logger* m_logger;
  static void checkTypes();
private:
  static std::unordered_map<std::string, std::unordered_map<std::string, void*>> m_components;
public:
  
  template <typename T>
  class Component {
  private:
    std::string m_type;
    std::string m_name;
    T m_object;
  public:
    
    Component(const std::string& name, const T& object)
      : m_type(typeid(T).name())
      , m_name(name)
      , m_object(object)
    {
      Environment::registerComponent(m_type, m_name, &m_object);
    }
    
    Component(const T& object)
      : Component("NoName", object)
    {}
    
    ~Component() {
      Environment::unregisterComponent(m_type, m_name);
    }
    
    T getObject() {
      return m_object;
    }
    
  };
  
private:
  static void registerComponent(const std::string& typeName, const std::string& componentName, void* component);
  static void unregisterComponent(const std::string& typeName, const std::string& componentName);
public:
  
  static void init();
  static void destroy();
  
  static void incObjects();
  static void decObjects();
  
  static v_counter getObjectsCount();
  static v_counter getObjectsCreated();
  
  static v_counter getThreadLocalObjectsCount();
  static v_counter getThreadLocalObjectsCreated();
  
  static void setLogger(Logger* logger);
  
  static void log(v_int32 priority, const std::string& tag, const std::string& message);
  static void logFormatted(v_int32 priority, const std::string& tag, const char* message, ...);
  
  static void* getComponent(const std::string& typeName);
  static void* getComponent(const std::string& typeName, const std::string& componentName);
  
};
  
#ifndef OATPP_DISABLE_LOGV
  #define OATPP_LOGV(TAG, ...) oatpp::base::Environment::logFormatted(0, TAG, __VA_ARGS__);
#else
  #define OATPP_LOGV(TAG, ...)
#endif
  
#ifndef OATPP_DISABLE_LOGD
  #define OATPP_LOGD(TAG, ...) oatpp::base::Environment::logFormatted(1, TAG, __VA_ARGS__);
#else
  #define OATPP_LOGD(TAG, ...)
#endif
  
#ifndef OATPP_DISABLE_LOGE
  #define OATPP_LOGE(TAG, ...) oatpp::base::Environment::logFormatted(2, TAG, __VA_ARGS__);
#else
  #define OATPP_LOGE(TAG, ...)
#endif
  
}}


#endif /* oatpp_base_Environment_hpp */
