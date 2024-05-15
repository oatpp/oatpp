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

namespace oatpp { namespace test {

OATPP_LOG_CATEGORY(LoggerTest::TESTCATEGORY, "LogCategory", true)

void LoggerTest::onRun() {

  auto logger = std::static_pointer_cast<oatpp::DefaultLogger>(oatpp::Environment::getLogger());

  OATPP_LOGv("LoggerTest", "Verbose Log")
  OATPP_LOGd("LoggerTest", "Debug Log")
  OATPP_LOGi("LoggerTest", "Info Log")
  OATPP_LOGw("LoggerTest", "Warning Log")
  OATPP_LOGe("LoggerTest", "Error Log")

  OATPP_LOGi("LoggerTest", " --- Disabling Debug Log")
  logger->disablePriority(oatpp::DefaultLogger::PRIORITY_D);
  OATPP_ASSERT(!logger->isLogPriorityEnabled(oatpp::DefaultLogger::PRIORITY_D))

  OATPP_LOGv("LoggerTest", "Verbose Log")
  OATPP_LOGd("LoggerTest", "Debug Log")
  OATPP_LOGi("LoggerTest", "Info Log")
  OATPP_LOGw("LoggerTest", "Warning Log")
  OATPP_LOGe("LoggerTest", "Error Log")

  OATPP_LOGi("LoggerTest", " --- Enabling Debug Log again")
  logger->enablePriority(oatpp::DefaultLogger::PRIORITY_D);
  OATPP_ASSERT(logger->isLogPriorityEnabled(oatpp::DefaultLogger::PRIORITY_D))

  OATPP_LOGv("LoggerTest", "Verbose Log")
  OATPP_LOGd("LoggerTest", "Debug Log")
  OATPP_LOGi("LoggerTest", "Info Log")
  OATPP_LOGw("LoggerTest", "Warning Log")
  OATPP_LOGe("LoggerTest", "Error Log")

  OATPP_LOGi(TESTCATEGORY, " --- Log-Test with category")
  OATPP_LOGv(TESTCATEGORY, "Verbose Log")
  OATPP_LOGd(TESTCATEGORY, "Debug Log")
  OATPP_LOGi(TESTCATEGORY, "Info Log")
  OATPP_LOGw(TESTCATEGORY, "Warning Log")
  OATPP_LOGe(TESTCATEGORY, "Error Log")

  OATPP_LOGi(TESTCATEGORY, " --- Disabling Debug Log for category")
  TESTCATEGORY.disablePriority(oatpp::DefaultLogger::PRIORITY_D);
  OATPP_ASSERT(!TESTCATEGORY.isLogPriorityEnabled(oatpp::DefaultLogger::PRIORITY_D))
  OATPP_LOGv(TESTCATEGORY, "Verbose Log")
  OATPP_LOGd(TESTCATEGORY, "Debug Log")
  OATPP_LOGi(TESTCATEGORY, "Info Log")
  OATPP_LOGw(TESTCATEGORY, "Warning Log")
  OATPP_LOGe(TESTCATEGORY, "Error Log")

}

}}
