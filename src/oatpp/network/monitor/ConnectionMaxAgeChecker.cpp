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

#include "ConnectionMaxAgeChecker.hpp"

namespace oatpp { namespace network { namespace monitor {

ConnectionMaxAgeChecker::ConnectionMaxAgeChecker(const std::chrono::duration<v_int64, std::micro>& maxAge)
  : m_maxAge(maxAge)
{}

std::vector<oatpp::String> ConnectionMaxAgeChecker::getMetricsList() {
  return {};
}

std::shared_ptr<StatCollector> ConnectionMaxAgeChecker::createStatCollector(const oatpp::String& metricName) {
  throw std::runtime_error("[oatpp::network::monitor::ConnectionMaxAgeChecker::createStatCollector()]: "
                           "Error. ConnectionMaxAgeChecker doesn't use any stat collectors.");
}

bool ConnectionMaxAgeChecker::check(const ConnectionStats& stats, v_int64 currMicroTime) {
  return currMicroTime - stats.timestampCreated < m_maxAge.count();
}

}}}
