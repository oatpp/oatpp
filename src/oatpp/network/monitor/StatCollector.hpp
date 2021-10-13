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

/**
 * ConnectionStats.
 */
struct ConnectionStats {

  /**
   * Timestamp created microseconds.
   * When connection was created.
   */
  v_int64 timestampCreated = 0;

  /**
   * Total bytes read from the connection.
   * Logs all bytes when the `read` method is called.
   */
  v_io_size totalRead = 0;

  /**
   * Total bytes written to the connection.
   * Logs all bytes when the `write` method is called.
   */
  v_io_size totalWrite = 0;

  /**
   * Timestamp microseconds when the last successful read was performed on the connection.
   */
  v_int64 timestampLastRead = 0;

  /**
   * Timestamp microseconds when the last successful write was performed on the connection.
   */
  v_int64 timestampLastWrite = 0;

  /**
   * Amount of bytes read during the last successful read.
   */
  v_io_size lastReadSize = 0;

  /**
   * Amount of bytes written during the last successful write.
   */
  v_io_size lastWriteSize = 0;

  /**
   * Data collected by stat-collectors - &l:StatCollector;
   */
  std::unordered_map<oatpp::String, void*> metricsData;

};

/**
 * StatCollector collects metrics data of the connection.
 */
class StatCollector : public oatpp::base::Countable {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~StatCollector() = default;

  /**
   * Unique metric name that is collected by this `StatCollector`.
   * @return - metricName. &id:oatpp::String;.
   */
  virtual oatpp::String metricName() = 0;

  /**
   * Metric data constructor.
   * @return
   */
  virtual void* createMetricData() = 0;

  /**
   * Metric data destructor.
   * @param metricData
   */
  virtual void deleteMetricData(void* metricData) = 0;

  /**
   * On connection read event.
   * @param metricData - metric data of the given connection.- the one created in the `createMetricData` method.
   * @param readResult - result of the connection read method.
   * @param timestamp - timestamp microseconds when the connection `read` method was called.
   */
  virtual void onRead(void* metricData, v_io_size readResult, v_int64 timestamp) = 0;

  /**
   * On connection write event.
   * @param metricData - metric data of the given connection.- the one created in the `createMetricData` method.
   * @param writeResult - result of the connection write method.
   * @param timestamp - timestamp microseconds when the connection `write` method was called.
   */
  virtual void onWrite(void* metricData, v_io_size writeResult, v_int64 timestamp) = 0;
};

}}}

#endif //oatpp_network_monitor_StatCollector_hpp
