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

#include "LoggerTest.hpp"

namespace oatpp { namespace test { namespace base {

OATPP_LOG_CATEGORY(LoggerTest::TESTCATEGORY, "LogCategory", true);

void LoggerTest::onRun() {

  auto logger = std::static_pointer_cast<oatpp::base::DefaultLogger>(oatpp::base::Environment::getLogger());

  OATPP_LOGV("LoggerTest", "Verbose Log");
  OATPP_LOGD("LoggerTest", "Debug Log");
  OATPP_LOGI("LoggerTest", "Info Log");
  OATPP_LOGW("LoggerTest", "Warning Log");
  OATPP_LOGE("LoggerTest", "Error Log");

  OATPP_LOGI("LoggerTest", " --- Disabling Debug Log");
  logger->disablePriority(oatpp::base::DefaultLogger::PRIORITY_D);
  OATPP_ASSERT(!logger->isLogPriorityEnabled(oatpp::base::DefaultLogger::PRIORITY_D))

  OATPP_LOGV("LoggerTest", "Verbose Log");
  OATPP_LOGD("LoggerTest", "Debug Log");
  OATPP_LOGI("LoggerTest", "Info Log");
  OATPP_LOGW("LoggerTest", "Warning Log");
  OATPP_LOGE("LoggerTest", "Error Log");

  OATPP_LOGI("LoggerTest", " --- Enabling Debug Log again");
  logger->enablePriority(oatpp::base::DefaultLogger::PRIORITY_D);
  OATPP_ASSERT(logger->isLogPriorityEnabled(oatpp::base::DefaultLogger::PRIORITY_D))

  OATPP_LOGV("LoggerTest", "Verbose Log");
  OATPP_LOGD("LoggerTest", "Debug Log");
  OATPP_LOGI("LoggerTest", "Info Log");
  OATPP_LOGW("LoggerTest", "Warning Log");
  OATPP_LOGE("LoggerTest", "Error Log");

  OATPP_LOGI(TESTCATEGORY, " --- Log-Test with category");
  OATPP_LOGV(TESTCATEGORY, "Verbose Log");
  OATPP_LOGD(TESTCATEGORY, "Debug Log");
  OATPP_LOGI(TESTCATEGORY, "Info Log");
  OATPP_LOGW(TESTCATEGORY, "Warning Log");
  OATPP_LOGE(TESTCATEGORY, "Error Log");

  OATPP_LOGI(TESTCATEGORY, " --- Disabling Debug Log for category");
  TESTCATEGORY.disablePriority(oatpp::base::DefaultLogger::PRIORITY_D);
  OATPP_ASSERT(!TESTCATEGORY.isLogPriorityEnabled(oatpp::base::DefaultLogger::PRIORITY_D))
  OATPP_LOGV(TESTCATEGORY, "Verbose Log");
  OATPP_LOGD(TESTCATEGORY, "Debug Log");
  OATPP_LOGI(TESTCATEGORY, "Info Log");
  OATPP_LOGW(TESTCATEGORY, "Warning Log");
  OATPP_LOGE(TESTCATEGORY, "Error Log");

}

}}}
