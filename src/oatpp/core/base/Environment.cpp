/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <oatpp@bamkrs.de>
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

#include "Environment.hpp"

#include <iomanip>
#include <chrono>
#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdarg>

#if defined(WIN32) || defined(_WIN32)
	#include <winsock2.h>

  struct tm* localtime_r(time_t *_clock, struct tm *_result) {
      localtime_s(_result, _clock);
      return _result;
  }
#endif

namespace oatpp { namespace base {

v_atomicCounter Environment::m_objectsCount(0);
v_atomicCounter Environment::m_objectsCreated(0);

#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
thread_local v_counter Environment::m_threadLocalObjectsCount = 0;
thread_local v_counter Environment::m_threadLocalObjectsCreated = 0;
#endif

std::mutex& Environment::getComponentsMutex() {
  static std::mutex componentsMutex;
  return componentsMutex;
}

std::unordered_map<std::string, std::unordered_map<std::string, void*>>& Environment::getComponents() {
  static std::unordered_map<std::string, std::unordered_map<std::string, void*>> components;
  return components;
}

std::shared_ptr<Logger> Environment::m_logger;

DefaultLogger::DefaultLogger(const Config& config)
  : m_config(config)
{}

void DefaultLogger::log(v_uint32 priority, const std::string& tag, const std::string& message) {

  bool indent = false;
  auto time = std::chrono::system_clock::now().time_since_epoch();

  std::lock_guard<std::mutex> lock(m_lock);

  switch (priority) {
    case PRIORITY_V:
      std::cout << "\033[0m V \033[0m|";
      break;

    case PRIORITY_D:
      std::cout << "\033[34m D \033[0m|";
      break;

    case PRIORITY_I:
      std::cout << "\033[32m I \033[0m|";
      break;

    case PRIORITY_W:
      std::cout << "\033[45m W \033[0m|";
      break;

    case PRIORITY_E:
      std::cout << "\033[41m E \033[0m|";
      break;

    default:
      std::cout << " " << priority << " |";
  }

  if (m_config.timeFormat) {
	time_t seconds = std::chrono::duration_cast<std::chrono::seconds>(time).count();
    struct tm now;
    localtime_r(&seconds, &now);
#ifdef OATPP_DISABLE_STD_PUT_TIME
	  char timeBuffer[50];
      strftime(timeBuffer, sizeof(timeBuffer), m_config.timeFormat, &now);
      std::cout << timeBuffer;
#else
      std::cout << std::put_time(&now, m_config.timeFormat);
#endif
    indent = true;
  }

  if (m_config.printTicks) {
    auto ticks = std::chrono::duration_cast<std::chrono::microseconds>(time).count();
    if(indent) {
      std::cout << " ";
    }
    std::cout << ticks;
    indent = true;
  }

  if (indent) {
    std::cout << "|";
  }

  if (message.empty()) {
    std::cout << " " << tag << std::endl;
  } else {
    std::cout << " " << tag << ":" << message << std::endl;
  }

}

void DefaultLogger::enablePriority(v_uint32 priority) {
  if (priority > PRIORITY_E) {
    return;
  }
  m_config.logMask |= (1 << priority);
}

void DefaultLogger::disablePriority(v_uint32 priority) {
  if (priority > PRIORITY_E) {
    return;
  }
  m_config.logMask &= ~(1 << priority);
}

bool DefaultLogger::isLogPriorityEnabled(v_uint32 priority) {
  if (priority > PRIORITY_E) {
    return true;
  }
  return m_config.logMask & (1 << priority);
}

void LogCategory::enablePriority(v_uint32 priority) {
  if (priority > Logger::PRIORITY_E) {
    return;
  }
  enabledPriorities |= (1 << priority);
}

void LogCategory::disablePriority(v_uint32 priority) {
  if (priority > Logger::PRIORITY_E) {
    return;
  }
  enabledPriorities &= ~(1 << priority);
}

bool LogCategory::isLogPriorityEnabled(v_uint32 priority) {
  if (priority > Logger::PRIORITY_E) {
    return true;
  }
  return enabledPriorities & (1 << priority);
}

void Environment::init() {
  init(std::make_shared<DefaultLogger>());
}

void Environment::init(const std::shared_ptr<Logger>& logger) {

  m_logger = logger;

#if defined(WIN32) || defined(_WIN32)
    // Initialize Winsock
    WSADATA wsaData;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {

        throw std::runtime_error("[oatpp::base::Environment::init()]: Error. WSAStartup failed");
    }
#endif

  checkTypes();

  m_objectsCount = 0;
  m_objectsCreated = 0;

#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  m_threadLocalObjectsCount = 0;
  m_threadLocalObjectsCreated = 0;
#endif

  {
    std::lock_guard<std::mutex> lock(getComponentsMutex());
    if (getComponents().size() > 0) {
      throw std::runtime_error("[oatpp::base::Environment::init()]: Error. "
                               "Invalid state. Components were created before call to Environment::init()");
    }
  }

}

void Environment::destroy(){
  if(getComponents().size() > 0) {
    std::lock_guard<std::mutex> lock(getComponentsMutex());
    throw std::runtime_error("[oatpp::base::Environment::destroy()]: Error. "
                             "Invalid state. Leaking components");
  }
  m_logger.reset();

#if defined(WIN32) || defined(_WIN32)
    WSACleanup();
#endif
}

void Environment::checkTypes(){

  static_assert(sizeof(v_char8) == 1, "");
  static_assert(sizeof(v_int16) == 2, "");
  static_assert(sizeof(v_uint16) == 2, "");
  static_assert(sizeof(v_int32) == 4, "");
  static_assert(sizeof(v_int64) == 8, "");
  static_assert(sizeof(v_uint32) == 4, "");
  static_assert(sizeof(v_uint64) == 8, "");
  static_assert(sizeof(v_float64) == 8, "");

  v_int32 vInt32 = ~v_int32(1);
  v_int64 vInt64 = ~v_int64(1);
  v_uint32 vUInt32 = ~v_uint32(1);
  v_uint64 vUInt64 = ~v_uint64(1);

  OATPP_ASSERT(vInt32 < 0);
  OATPP_ASSERT(vInt64 < 0);
  OATPP_ASSERT(vUInt32 > 0);
  OATPP_ASSERT(vUInt64 > 0);

}

void Environment::incObjects(){

  m_objectsCount ++;
  m_objectsCreated ++;

#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  m_threadLocalObjectsCount ++;
  m_threadLocalObjectsCreated ++;
#endif

}

void Environment::decObjects(){

  m_objectsCount --;

#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  m_threadLocalObjectsCount --;
#endif

}

v_counter Environment::getObjectsCount(){
  return m_objectsCount;
}

v_counter Environment::getObjectsCreated(){
  return m_objectsCreated;
}

v_counter Environment::getThreadLocalObjectsCount(){
#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  return m_threadLocalObjectsCount;
#else
  return 0;
#endif
}

v_counter Environment::getThreadLocalObjectsCreated(){
#ifndef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  return m_threadLocalObjectsCreated;
#else
  return 0;
#endif
}

void Environment::setLogger(const std::shared_ptr<Logger>& logger){
  m_logger = logger;
}

std::shared_ptr<Logger> Environment::getLogger() {
  return m_logger;
}

void Environment::printCompilationConfig() {

  OATPP_LOGD("oatpp-version", OATPP_VERSION);

#ifdef OATPP_DISABLE_ENV_OBJECT_COUNTERS
  OATPP_LOGD("oatpp/Config", "OATPP_DISABLE_ENV_OBJECT_COUNTERS");
#endif

#ifdef OATPP_COMPAT_BUILD_NO_THREAD_LOCAL
  OATPP_LOGD("oatpp/Config", "OATPP_COMPAT_BUILD_NO_THREAD_LOCAL");
#endif

#ifdef OATPP_THREAD_HARDWARE_CONCURRENCY
  OATPP_LOGD("oatpp/Config", "OATPP_THREAD_HARDWARE_CONCURRENCY=%d", OATPP_THREAD_HARDWARE_CONCURRENCY);
#endif

}

void Environment::log(v_uint32 priority, const std::string& tag, const std::string& message) {
  if(m_logger != nullptr) {
    m_logger->log(priority, tag, message);
  }
}


void Environment::logFormatted(v_uint32 priority, const LogCategory& category, const char* message, ...) {
  if (category.categoryEnabled && (category.enabledPriorities & (1 << priority))) {
    va_list args;
    va_start(args, message);
    vlogFormatted(priority, category.tag, message, args);
    va_end(args);
  }
}

void Environment::logFormatted(v_uint32 priority, const std::string& tag, const char* message, ...) {
    va_list args;
    va_start(args, message);
    vlogFormatted(priority, tag, message, args);
    va_end(args);
}

void Environment::vlogFormatted(v_uint32 priority, const std::string& tag, const char* message, va_list args) {
  // do we have a logger and the priority is enabled?
  if (m_logger == nullptr || !m_logger->isLogPriorityEnabled(priority)) {
    return;
  }
  // if we dont need to format anything, just print the message
  if(message == nullptr) {
    log(priority, tag, std::string());
    return;
  }
  // check how big our buffer has to be
  va_list argscpy;
  va_copy(argscpy, args);
  v_buff_size allocsize = vsnprintf(nullptr, 0, message, argscpy) + 1;
  // alloc the buffer (or the max size)
  if (allocsize > m_logger->getMaxFormattingBufferSize()) {
    allocsize = m_logger->getMaxFormattingBufferSize();
  }
  auto buffer = std::unique_ptr<char[]>(new char[allocsize]);
  memset(buffer.get(), 0, allocsize);
  // actually format
  vsnprintf(buffer.get(), allocsize, message, args);
  // call (user) providen log function
  log(priority, tag, buffer.get());
}

void Environment::registerComponent(const std::string& typeName, const std::string& componentName, void* component) {
  std::lock_guard<std::mutex> lock(getComponentsMutex());
  auto& bucket = getComponents()[typeName];
  auto it = bucket.find(componentName);
  if(it != bucket.end()){
    throw std::runtime_error("[oatpp::base::Environment::registerComponent()]: Error. Component with given name already exists: name='" + componentName + "'");
  }
  bucket[componentName] = component;
}

void Environment::unregisterComponent(const std::string& typeName, const std::string& componentName) {
  std::lock_guard<std::mutex> lock(getComponentsMutex());
  auto& components = getComponents();
  auto bucketIt = getComponents().find(typeName);
  if(bucketIt == components.end() || bucketIt->second.size() == 0) {
    throw std::runtime_error("[oatpp::base::Environment::unregisterComponent()]: Error. Component of given type doesn't exist: type='" + typeName + "'");
  }
  auto& bucket = bucketIt->second;
  auto componentIt = bucket.find(componentName);
  if(componentIt == bucket.end()) {
    throw std::runtime_error("[oatpp::base::Environment::unregisterComponent()]: Error. Component with given name doesn't exist: name='" + componentName + "'");
  }
  bucket.erase(componentIt);
  if(bucket.size() == 0) {
    components.erase(bucketIt);
  }
}

void* Environment::getComponent(const std::string& typeName) {
  std::lock_guard<std::mutex> lock(getComponentsMutex());
  auto& components = getComponents();
  auto bucketIt = components.find(typeName);
  if(bucketIt == components.end() || bucketIt->second.size() == 0) {
    throw std::runtime_error("[oatpp::base::Environment::getComponent()]: Error. Component of given type doesn't exist: type='" + typeName + "'");
  }
  auto bucket = bucketIt->second;
  if(bucket.size() > 1){
    throw std::runtime_error("[oatpp::base::Environment::getComponent()]: Error. Ambiguous component reference. Multiple components exist for a given type: type='" + typeName + "'");
  }
  return bucket.begin()->second;
}

void* Environment::getComponent(const std::string& typeName, const std::string& componentName) {
  std::lock_guard<std::mutex> lock(getComponentsMutex());
  auto& components = getComponents();
  auto bucketIt = components.find(typeName);
  if(bucketIt == components.end() || bucketIt->second.size() == 0) {
    throw std::runtime_error("[oatpp::base::Environment::getComponent()]: Error. Component of given type doesn't exist: type='" + typeName + "'");
  }
  auto bucket = bucketIt->second;
  auto componentIt = bucket.find(componentName);
  if(componentIt == bucket.end()) {
    throw std::runtime_error("[oatpp::base::Environment::getComponent()]: Error. Component with given name doesn't exist: name='" + componentName + "'");
  }
  return componentIt->second;
}

v_int64 Environment::getMicroTickCount(){
  std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>
  (std::chrono::system_clock::now().time_since_epoch());
  return ms.count();
}

}}
