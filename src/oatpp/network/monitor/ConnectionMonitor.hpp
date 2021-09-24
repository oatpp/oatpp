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

#ifndef oatpp_network_monitor_ConnectionMonitor_hpp
#define oatpp_network_monitor_ConnectionMonitor_hpp

#include "MetricsChecker.hpp"

#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

#include <unordered_map>
#include <condition_variable>

namespace oatpp { namespace network { namespace monitor {

class ConnectionMonitor : public ConnectionProvider {
private:

  class Monitor; // FWD

  class ConnectionProxy : public data::stream::IOStream {
    friend Monitor;
  private:
    std::shared_ptr<Monitor> m_monitor;
    /* provider which created this connection */
    std::shared_ptr<ConnectionProvider> m_connectionProvider;
    std::shared_ptr<data::stream::IOStream> m_connection;
    std::mutex m_statsMutex;
    ConnectionStats m_stats;
  public:

    ConnectionProxy(const std::shared_ptr<Monitor>& monitor,
                    const std::shared_ptr<ConnectionProvider>& connectionProvider,
                    const std::shared_ptr<data::stream::IOStream>& connection);

    ~ConnectionProxy() override;

    v_io_size read(void *buffer, v_buff_size count, async::Action& action) override;
    v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

    void setInputStreamIOMode(data::stream::IOMode ioMode) override;
    data::stream::IOMode getInputStreamIOMode() override;
    data::stream::Context& getInputStreamContext() override;

    void setOutputStreamIOMode(data::stream::IOMode ioMode) override;
    data::stream::IOMode getOutputStreamIOMode() override;
    data::stream::Context& getOutputStreamContext() override;

    void invalidate();

  };

private:

  class Monitor : public oatpp::base::Countable {
  private:

    std::mutex m_runMutex;
    std::condition_variable m_runCondition;
    std::atomic<bool> m_running {true};
    bool m_stopped {false};

    std::mutex m_connectionsMutex;
    std::unordered_map<v_uint64, std::weak_ptr<ConnectionProxy>> m_connections;

    std::mutex m_checkMutex;
    std::vector<std::shared_ptr<MetricsChecker>> m_metricsCheckers;
    std::unordered_map<oatpp::String, std::shared_ptr<StatCollector>> m_statCollectors;

  private:
    static void monitorTask(std::shared_ptr<Monitor> monitor);
  private:
    static void* createOrGetMetricData(ConnectionStats& stats, const std::shared_ptr<StatCollector>& collector);
  public:

    static std::shared_ptr<Monitor> createShared();

    void addConnection(v_uint64 id, const std::weak_ptr<ConnectionProxy>& connection);
    void freeConnectionStats(ConnectionStats& stats);
    void removeConnection(v_uint64 id);

    void addStatCollector(const std::shared_ptr<StatCollector>& collector);
    void removeStatCollector(const oatpp::String& metricName);

    void addMetricsChecker(const std::shared_ptr<MetricsChecker>& checker);

    void onConnectionRead(ConnectionStats& stats, v_io_size readResult);
    void onConnectionWrite(ConnectionStats& stats, v_io_size writeResult);

    void stop();

  };

private:
  std::shared_ptr<Monitor> m_monitor;
  std::shared_ptr<ConnectionProvider> m_connectionProvider;
public:

  ConnectionMonitor(const std::shared_ptr<ConnectionProvider>& connectionProvider);

  std::shared_ptr<data::stream::IOStream> get() override;

  async::CoroutineStarterForResult<const std::shared_ptr<data::stream::IOStream>&> getAsync() override;

  void addStatCollector(const std::shared_ptr<StatCollector>& collector);

  void addMetricsChecker(const std::shared_ptr<MetricsChecker>& checker);

  void invalidate(const std::shared_ptr<data::stream::IOStream>& connection) override;

  void stop() override;

};

}}}

#endif //oatpp_network_monitor_ConnectionMonitor_hpp
