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

#ifndef oatpp_network_monitor_StatCollector_hpp
#define oatpp_network_monitor_StatCollector_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/IODefinitions.hpp"

namespace oatpp { namespace network { namespace monitor {

struct ConnectionStats {

  v_int64 timestampCreated = 0;

  v_io_size totalRead = 0;
  v_io_size totalWrite = 0;

  v_int64 timestampLastRead = 0;
  v_int64 timestampLastWrite = 0;

  v_io_size lastReadSize = 0;
  v_io_size lastWriteSize = 0;

  std::unordered_map<oatpp::String, void*> metricsData;

};

class StatCollector : public oatpp::base::Countable {
public:
  virtual ~StatCollector() = default;

  virtual oatpp::String metricName() = 0;
  virtual void* createMetricData() = 0;
  virtual void deleteMetricData(void* metricData) = 0;
  virtual void onRead(void* metricData, v_io_size readResult, v_int64 timestamp) = 0;
  virtual void onWrite(void* metricData, v_io_size writeResult, v_int64 timestamp) = 0;
};

}}}

#endif //oatpp_network_monitor_StatCollector_hpp
