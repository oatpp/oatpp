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

#include "ConnectionMonitor.hpp"

#include <chrono>
#include <thread>

namespace oatpp { namespace network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionMonitor::ConnectionProxy

ConnectionMonitor::ConnectionProxy::ConnectionProxy(const std::shared_ptr<Monitor>& monitor,
                                                    const std::shared_ptr<ConnectionProvider>& connectionProvider,
                                                    const std::shared_ptr<data::stream::IOStream>& connection)
  : m_monitor(monitor)
  , m_connectionProvider(connectionProvider)
  , m_connection(connection)
{
  m_stats.timestampCreated = base::Environment::getMicroTickCount();
}

ConnectionMonitor::ConnectionProxy::~ConnectionProxy() {

  std::lock_guard<std::mutex> lock(m_statsMutex);

  m_monitor->freeConnectionStats(m_stats);

  if(m_stats.metricData.size() > 0) {

    for(auto& pair : m_stats.metricData) {
      OATPP_LOGE("[oatpp::network::ConnectionMonitor::ConnectionProxy::~ConnectionProxy()]",
                 "Error. Memory leak. Metric data was not deleted: Metric name - '%s'", pair.first->c_str());
    }

  }

  m_monitor->removeConnection((v_uint64) this);

}

v_io_size ConnectionMonitor::ConnectionProxy::read(void *buffer, v_buff_size count, async::Action& action) {
  auto res = m_connection->read(buffer, count, action);
  std::lock_guard<std::mutex> lock(m_statsMutex);
  m_monitor->onConnectionRead(m_stats, res);
  return res;
}

v_io_size ConnectionMonitor::ConnectionProxy::write(const void *data, v_buff_size count, async::Action& action) {
  auto res = m_connection->write(data, count, action);
  std::lock_guard<std::mutex> lock(m_statsMutex);
  m_monitor->onConnectionWrite(m_stats, res);
  return res;
}

void ConnectionMonitor::ConnectionProxy::setInputStreamIOMode(data::stream::IOMode ioMode) {
  m_connection->setInputStreamIOMode(ioMode);
}
data::stream::IOMode ConnectionMonitor::ConnectionProxy::getInputStreamIOMode() {
  return m_connection->getInputStreamIOMode();
}
data::stream::Context& ConnectionMonitor::ConnectionProxy::getInputStreamContext() {
  return m_connection->getInputStreamContext();
}

void ConnectionMonitor::ConnectionProxy::setOutputStreamIOMode(data::stream::IOMode ioMode) {
  m_connection->setOutputStreamIOMode(ioMode);
}

data::stream::IOMode ConnectionMonitor::ConnectionProxy::getOutputStreamIOMode() {
  return m_connection->getOutputStreamIOMode();
}

data::stream::Context& ConnectionMonitor::ConnectionProxy::getOutputStreamContext() {
  return m_connection->getOutputStreamContext();
}

void ConnectionMonitor::ConnectionProxy::invalidate()  {
  m_connectionProvider->invalidate(m_connection);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Monitor

void ConnectionMonitor::Monitor::monitorTask(std::shared_ptr<Monitor> monitor) {

  while(monitor->m_running) {

    {

      std::lock_guard<std::mutex> lock(monitor->m_connectionsMutex);

      auto currMicroTime = oatpp::base::Environment::getMicroTickCount();

      for(auto& pair : monitor->m_connections) {

        auto connection = pair.second.lock();
        std::lock_guard<std::mutex> dataLock(connection->m_statsMutex);
        std::lock_guard<std::mutex> analysersLock(monitor->m_analysersMutex);

        for(auto& a : monitor->m_analysers) {
          bool res = a->analyse(connection->m_stats, currMicroTime);
          if(!res) {
            connection->invalidate();
            break;
          }
        }

      }

    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  }

  {
    std::lock_guard<std::mutex>(monitor->m_runMutex);
    monitor->m_stopped = true;
  }

  monitor->m_runCondition.notify_all();

}

void* ConnectionMonitor::Monitor::createOrGetMetricData(ConnectionStats& stats, const std::shared_ptr<StatCollector>& collector) {
  void* data;
  auto it = stats.metricData.find(collector->metricName());
  if(it == stats.metricData.end()) {
    data = collector->createMetricData();
    stats.metricData.insert({collector->metricName(), data});
  } else {
    data = it->second;
  }
  return data;
}

std::shared_ptr<ConnectionMonitor::Monitor> ConnectionMonitor::Monitor::createShared() {
  auto monitor = std::make_shared<Monitor>();
  std::thread t([monitor](){
    ConnectionMonitor::Monitor::monitorTask(monitor);
  });
  t.detach();
  return monitor;
}

void ConnectionMonitor::Monitor::addConnection(v_uint64 id, const std::weak_ptr<ConnectionProxy>& connection) {
  std::lock_guard<std::mutex> lock(m_connectionsMutex);
  m_connections.insert({id, connection});
}

void ConnectionMonitor::Monitor::freeConnectionStats(ConnectionStats& stats) {

  std::lock_guard<std::mutex> lock(m_analysersMutex);

  for(auto& metric : stats.metricData) {
    auto it = m_statCollectors.find(metric.first);
    if(it != m_statCollectors.end()) {
      it->second->deleteMetricData(metric.second);
    } else {
      OATPP_LOGE("[oatpp::network::ConnectionMonitor::Monitor::freeConnectionStats]",
                 "Error. Can't free Metric data. Unknown Metric: name - '%s'", it->first->c_str());
    }
  }

}

void ConnectionMonitor::Monitor::removeConnection(v_uint64 id) {
  std::lock_guard<std::mutex> lock(m_connectionsMutex);
  m_connections.erase(id);
}

void ConnectionMonitor::Monitor::addStatCollector(const std::shared_ptr<StatCollector>& collector) {
  std::lock_guard<std::mutex> lock(m_analysersMutex);
  m_statCollectors.insert({collector->metricName(), collector});
}

void ConnectionMonitor::Monitor::removeStatCollector(const oatpp::String& metricName) {
  std::lock_guard<std::mutex> lock(m_analysersMutex);
  m_statCollectors.erase(metricName);
}

void ConnectionMonitor::Monitor::addAnalyser(const std::shared_ptr<MetricAnalyser>& analyser) {
  std::lock_guard<std::mutex> lock(m_analysersMutex);
  m_analysers.push_back(analyser);
  auto metrics = analyser->getMetricsList();
  for(auto& m : metrics) {
    auto it = m_statCollectors.find(m);
    if(it == m_statCollectors.end()) {
      m_statCollectors.insert({m, analyser->createStatCollector(m)});
    }
  }
}

void ConnectionMonitor::Monitor::onConnectionRead(ConnectionStats& stats, v_io_size readResult) {

  v_int64 currTimestamp = base::Environment::getMicroTickCount();

  if(readResult > 0) {
    stats.totalRead += readResult;
    stats.lastReadSize = readResult;
    stats.timestampLastRead = currTimestamp;
  }

  {
    std::lock_guard<std::mutex> lock(m_analysersMutex);
    for(auto& pair : m_statCollectors) {
      pair.second->onRead(createOrGetMetricData(stats, pair.second), readResult, currTimestamp);
    }
  }

}

void ConnectionMonitor::Monitor::onConnectionWrite(ConnectionStats& stats, v_io_size writeResult) {

  v_int64 currTimestamp = base::Environment::getMicroTickCount();

  if(writeResult > 0) {
    stats.totalWrite += writeResult;
    stats.lastWriteSize = writeResult;
    stats.timestampLastWrite = currTimestamp;
  }

  {
    std::lock_guard<std::mutex> lock(m_analysersMutex);
    for(auto& pair : m_statCollectors) {
      pair.second->onWrite(createOrGetMetricData(stats, pair.second), writeResult, currTimestamp);
    }
  }

}

void ConnectionMonitor::Monitor::stop() {
  m_running = false;
  std::unique_lock<std::mutex> runLock(m_runMutex);
  while(!m_stopped) {
    m_runCondition.wait(runLock);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionMonitor

ConnectionMonitor::ConnectionMonitor(const std::shared_ptr<ConnectionProvider>& connectionProvider)
  : m_monitor(Monitor::createShared())
  , m_connectionProvider(connectionProvider)
{
}

std::shared_ptr<data::stream::IOStream> ConnectionMonitor::get() {
  auto connection = m_connectionProvider->get();
  auto proxy = std::make_shared<ConnectionProxy>(m_monitor, m_connectionProvider, connection);
  m_monitor->addConnection((v_uint64) proxy.get(), proxy);
  return proxy;
}

void ConnectionMonitor::addStatCollector(const std::shared_ptr<StatCollector>& collector) {
  m_monitor->addStatCollector(collector);
}

void ConnectionMonitor::addAnalyser(const std::shared_ptr<MetricAnalyser>& analyser) {
  m_monitor->addAnalyser(analyser);
}

void ConnectionMonitor::stop() {
  m_monitor->stop();
}

}}