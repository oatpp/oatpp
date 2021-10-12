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

#ifndef oatpp_network_monitor_MetricsChecker_hpp
#define oatpp_network_monitor_MetricsChecker_hpp

#include "StatCollector.hpp"

namespace oatpp { namespace network { namespace monitor {

/**
 * MetricsChecker checks &id:oatpp::network::monitor::ConnectionStats; if those are satisfy the rule.
 */
class MetricsChecker : public oatpp::base::Countable  {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~MetricsChecker() = default;

  /**
   * Get list of metrics names that are checked by this MetricsChecker.
   * @return
   */
  virtual std::vector<oatpp::String> getMetricsList() = 0;

  /**
   * Create &id:oatpp::network::monitor::StatCollector; for given `metricName`.
   * This method will be called by &id:oatpp::network::monitor::ConnectionMonitor; only if there is
   * no such `StatCollector` registered in the `ConnectionMonitor` yet.
   * @param metricName - name of the metric.
   * @return - &id:oatpp::network::monitor::StatCollector;.
   */
  virtual std::shared_ptr<StatCollector> createStatCollector(const oatpp::String& metricName) = 0;

  /**
   * Called by &id:oatpp::network::monitor::ConnectionMonitor; on each
   * time interval to check if connection satisfies the rule.
   * @param stats - &id:oatpp::network::monitor::ConnectionStats;.
   * @param currMicroTime - current time microseconds.
   * @return - `true` if connection satisfies the rule. `false` if connection should be closed.
   */
  virtual bool check(const ConnectionStats& stats, v_int64 currMicroTime) = 0;

};

}}}

#endif //oatpp_network_monitor_MetricsChecker_hpp
