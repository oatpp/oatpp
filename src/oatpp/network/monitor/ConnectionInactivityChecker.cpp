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

#include "ConnectionInactivityChecker.hpp"

namespace oatpp { namespace network { namespace monitor {

ConnectionInactivityChecker::ConnectionInactivityChecker(const std::chrono::duration<v_int64, std::micro>& lastReadTimeout,
                                                         const std::chrono::duration<v_int64, std::micro>& lastWriteTimeout)
  : m_lastReadTimeout(lastReadTimeout)
  , m_lastWriteTimeout(lastWriteTimeout)
{}

std::vector<oatpp::String> ConnectionInactivityChecker::getMetricsList() {
  return {};
}

std::shared_ptr<StatCollector> ConnectionInactivityChecker::createStatCollector(const oatpp::String& metricName) {
  throw std::runtime_error("[oatpp::network::monitor::ConnectionInactivityChecker::createStatCollector()]: "
                           "Error. ConnectionInactivityChecker doesn't use any stat collectors.");
}

bool ConnectionInactivityChecker::check(const ConnectionStats& stats, v_int64 currMicroTime) {

  bool goodRead;
  if(stats.timestampLastRead == 0) {
    goodRead = currMicroTime - stats.timestampCreated < m_lastReadTimeout.count();
  } else {
    goodRead = currMicroTime - stats.timestampLastRead < m_lastReadTimeout.count();
  }

  bool goodWrite;
  if(stats.timestampLastWrite == 0) {
    goodWrite = currMicroTime - stats.timestampCreated < m_lastWriteTimeout.count();
  } else {
    goodWrite = currMicroTime - stats.timestampLastWrite < m_lastWriteTimeout.count();
  }

  return  goodRead && goodWrite;
}

}}}
