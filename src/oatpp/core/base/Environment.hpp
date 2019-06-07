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

#ifndef oatpp_base_Environment_hpp
#define oatpp_base_Environment_hpp

#include "./Config.hpp"

#include <stdio.h>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <stdlib.h>

#define OATPP_VERSION "0.19.4"

typedef unsigned char v_char8;
typedef v_char8 *p_char8;

typedef int8_t v_int8;
typedef v_int8* p_int8;
typedef uint8_t v_word8;
typedef v_word8* p_word8;

typedef int16_t v_int16;
typedef v_int16* p_int16;
typedef uint16_t v_word16;
typedef v_word16* p_word16;

typedef int32_t v_int32;
typedef v_int32* p_int32;
typedef uint32_t v_word32;
typedef v_word32* p_word32;

typedef int64_t v_int64;
typedef v_int64* p_int64;
typedef uint64_t v_word64;
typedef v_word64* p_word64;

typedef double v_float64;
typedef v_float64 * p_float64;

typedef float v_float32;
typedef v_float32* p_float32;

typedef std::atomic_int_fast64_t v_atomicCounter;
typedef v_int64 v_counter;

namespace oatpp { namespace base{

/**
 * Interface for system-wide Logger.<br>
 * All calls to `OATPP_DISABLE_LOGV`, `OATPP_DISABLE_LOGD`, `OATPP_DISABLE_LOGI`,
 * `OATPP_DISABLE_LOGW`, `OATPP_DISABLE_LOGE` will come here.
 */
class Logger {
public:
  /**
   * Log priority V-verbouse.
   */
  static constexpr v_int32 PRIORITY_V = 0;

  /**
   * Log priority D-debug.
   */
  static constexpr v_int32 PRIORITY_D = 1;

  /**
   * Log priority I-Info.
   */
  static constexpr v_int32 PRIORITY_I = 2;

  /**
   * Log priority W-Warning.
   */
  static constexpr v_int32 PRIORITY_W = 3;

  /**
   * Log priority E-error.
   */
  static constexpr v_int32 PRIORITY_E = 4;
public:
  /**
   * Virtual Destructor.
   */
  virtual ~Logger() = default;

  /**
   * Log message with priority, tag, message.
   * @param priority - priority channel of the message.
   * @param tag - tag of the log message.
   * @param message - message.
   */
  virtual void log(v_int32 priority, const std::string& tag, const std::string& message) = 0;
};

/**
 * Default Logger implementation.
 */
class DefaultLogger : public Logger {
public:
  /**
   * Default Logger Config.
   */
  struct Config {

    /**
     * Constructor.
     * @param tfmt - time format.
     * @param printMicroTicks - show ticks in microseconds.
     */
    Config(const char* tfmt, bool printMicroTicks)
      : timeFormat(tfmt)
      , printTicks(printMicroTicks)
    {}

    /**
     * Time format of the log message.
     * If nullptr then do not print time.
     */
    const char* timeFormat;

    /**
     * Print micro-ticks in the log message.
     */
    bool printTicks;
  };
private:
  Config m_config;
  std::mutex m_lock;
public:

  /**
   * Constructor.
   * @param config - Logger config.
   */
  DefaultLogger(const Config& config = Config("%Y-%m-%d %H:%M:%S", true));

  /**
   * Log message with priority, tag, message.
   * @param priority - log-priority channel of the message.
   * @param tag - tag of the log message.
   * @param message - message.
   */
  void log(v_int32 priority, const std::string& tag, const std::string& message) override;
};

/**
 * Class to manage application environment.<br>
 * Manage object counters, manage components, and do system health-checks.
 */
class Environment{
private:

  static v_atomicCounter m_objectsCount;
  static v_atomicCounter m_objectsCreated;

#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  static thread_local v_counter m_threadLocalObjectsCount;
  static thread_local v_counter m_threadLocalObjectsCreated;
#endif

private:
  static std::shared_ptr<Logger> m_logger;
  static void checkTypes();
private:
  static std::unordered_map<std::string, std::unordered_map<std::string, void*>> m_components;
public:

  /**
   * Class representing system component.
   * @tparam T - component type.
   */
  template <typename T>
  class Component {
  private:
    std::string m_type;
    std::string m_name;
    T m_object;
  public:

    /**
     * Constructor.
     * @param name - component name.
     * @param object - component object.
     */
    Component(const std::string& name, const T& object)
      : m_type(typeid(T).name())
      , m_name(name)
      , m_object(object)
    {
      Environment::registerComponent(m_type, m_name, &m_object);
    }

    /**
     * Constructor.
     * @param object - component object.
     */
    Component(const T& object)
      : Component("NoName", object)
    {}

    /**
     * Non-virtual Destructor.
     */
    ~Component() {
      Environment::unregisterComponent(m_type, m_name);
    }

    /**
     * Get object stored in the component.
     * @return - object.
     */
    T getObject() {
      return m_object;
    }
    
  };
  
private:
  static void registerComponent(const std::string& typeName, const std::string& componentName, void* component);
  static void unregisterComponent(const std::string& typeName, const std::string& componentName);
public:

  /**
   * Initialize environment and do basic health-checks.
   */
  static void init();

  /**
   * Initialize environment and do basic health-checks.
   * @param logger - system-wide logger.
   */
  static void init(const std::shared_ptr<Logger>& logger);

  /**
   * De-initialize environment and do basic health-checks.
   * Check for memory leaks.
   */
  static void destroy();

  /**
   * increment counter of objects.
   */
  static void incObjects();

  /**
   * decrement counter of objects.
   */
  static void decObjects();

  /**
   * Get count of objects currently allocated and stored in the memory.
   * @return
   */
  static v_counter getObjectsCount();

  /**
   * Get count of objects created for a whole system lifetime.
   * @return - count of objects.
   */
  static v_counter getObjectsCreated();

  /**
   * Same as `getObjectsCount()` but `thread_local`
   * @return - count of objects. <br>
   * *0 - if built with `-DOATPP_COMPAT_BUILD_NO_THREAD_LOCAL` flag*
   */
  static v_counter getThreadLocalObjectsCount();

  /**
   * Same as `getObjectsCreated()` but `thread_local`
   * @return - count of objects. <br>
   * *0 - if built with `-DOATPP_COMPAT_BUILD_NO_THREAD_LOCAL` flag*
   */
  static v_counter getThreadLocalObjectsCreated();

  /**
   * Set environment logger.
   * @param logger - system-wide logger.
   */
  static void setLogger(const std::shared_ptr<Logger>& logger);

  /**
   * Print debug information of compilation config.<br>
   * Print values for: <br>
   * - `OATPP_DISABLE_ENV_OBJECT_COUNTERS`<br>
   * - `OATPP_DISABLE_POOL_ALLOCATIONS`<br>
   * - `OATPP_THREAD_HARDWARE_CONCURRENCY`<br>
   * - `OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT`<br>
   * - `OATPP_ASYNC_EXECUTOR_THREAD_NUM_DEFAULT`
   */
  static void printCompilationConfig();

  /**
   * Call `Logger::log()`
   * @param priority - log-priority channel of the message.
   * @param tag - tag of the log message.
   * @param message - message.
   */
  static void log(v_int32 priority, const std::string& tag, const std::string& message);

  /**
   * Format message and call `Logger::log()`<br>
   * Message is formatted using `vsnprintf` method.
   * @param priority - log-priority channel of the message.
   * @param tag - tag of the log message.
   * @param message - message.
   * @param ... - format arguments.
   */
  static void logFormatted(v_int32 priority, const std::string& tag, const char* message, ...);

  /**
   * Get component object by typeName.
   * @param typeName - type name of the component.
   * @return - pointer to a component object.
   */
  static void* getComponent(const std::string& typeName);

  /**
   * Get component object by typeName and componentName.
   * @param typeName - type name of the component.
   * @param componentName - component qualifier name.
   * @return - pointer to a component object.
   */
  static void* getComponent(const std::string& typeName, const std::string& componentName);

  /**
   * Get ticks count in microseconds.
   * @return - ticks count in microseconds.
   */
  static v_int64 getMicroTickCount();
  
};

/**
 * Default oatpp assert method.
 * @param EXP - expression that must be `true`.
 */
#define OATPP_ASSERT(EXP) \
if(!(EXP)) { \
  OATPP_LOGE("\033[1mASSERT\033[0m[\033[1;31mFAILED\033[0m]", #EXP); \
  exit(EXIT_FAILURE); \
}
  
#ifndef OATPP_DISABLE_LOGV

  /**
   * Log message with &l:Logger::PRIORITY_V; <br>
   * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGV`*
   * @param TAG - message tag.
   * @param ...(1) - message.
   * @param ... - optional format parameter.
   */
  #define OATPP_LOGV(TAG, ...) \
  oatpp::base::Environment::logFormatted(oatpp::base::Logger::PRIORITY_V, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGV(TAG, ...)
#endif
  
#ifndef OATPP_DISABLE_LOGD

  /**
   * Log message with &l:Logger::PRIORITY_D; <br>
   * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGD`*
   * @param TAG - message tag.
   * @param ...(1) - message.
   * @param ... - optional format parameter.
   */
  #define OATPP_LOGD(TAG, ...) \
  oatpp::base::Environment::logFormatted(oatpp::base::Logger::PRIORITY_D, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGD(TAG, ...)
#endif

#ifndef OATPP_DISABLE_LOGI

  /**
   * Log message with &l:Logger::PRIORITY_I; <br>
   * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGI`*
   * @param TAG - message tag.
   * @param ...(1) - message.
   * @param ... - optional format parameter.
   */
  #define OATPP_LOGI(TAG, ...) \
  oatpp::base::Environment::logFormatted(oatpp::base::Logger::PRIORITY_I, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGI(TAG, ...)
#endif

#ifndef OATPP_DISABLE_LOGW

  /**
   * Log message with &l:Logger::PRIORITY_W; <br>
   * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGW`*
   * @param TAG - message tag.
   * @param ...(1) - message.
   * @param ... - optional format parameter.
   */
  #define OATPP_LOGW(TAG, ...) \
  oatpp::base::Environment::logFormatted(oatpp::base::Logger::PRIORITY_W, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGW(TAG, ...)
#endif

#ifndef OATPP_DISABLE_LOGE

  /**
   * Log message with &l:Logger::PRIORITY_E; <br>
   * *To disable this log compile oatpp with `#define OATPP_DISABLE_LOGE`*
   * @param TAG - message tag.
   * @param ...(1) - message.
   * @param ... - optional format parameter.
   */
  #define OATPP_LOGE(TAG, ...) \
  oatpp::base::Environment::logFormatted(oatpp::base::Logger::PRIORITY_E, TAG, __VA_ARGS__);

#else
  #define OATPP_LOGE(TAG, ...)
#endif
  
}}


#endif /* oatpp_base_Environment_hpp */
