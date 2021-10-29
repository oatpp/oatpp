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

namespace oatpp { namespace network { namespace monitor {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionMonitor::ConnectionInvalidator

void ConnectionMonitor::ConnectionInvalidator::invalidate(const std::shared_ptr<data::stream::IOStream> &connection) {
  auto proxy = std::static_pointer_cast<ConnectionProxy>(connection);
  proxy->invalidate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionMonitor::ConnectionProxy

ConnectionMonitor::ConnectionProxy::ConnectionProxy(const std::shared_ptr<Monitor>& monitor,
                                                    const provider::ResourceHandle<data::stream::IOStream>& connectionHandle)
  : m_monitor(monitor)
  , m_connectionHandle(connectionHandle)
{
  m_stats.timestampCreated = base::Environment::getMicroTickCount();
}

ConnectionMonitor::ConnectionProxy::~ConnectionProxy() {

  m_monitor->removeConnection((v_uint64) this);

  std::lock_guard<std::mutex> lock(m_statsMutex);

  m_monitor->freeConnectionStats(m_stats);

  if(m_stats.metricsData.size() > 0) {

    for(auto& pair : m_stats.metricsData) {
      OATPP_LOGE("[oatpp::network::ConnectionMonitor::ConnectionProxy::~ConnectionProxy()]",
                 "Error. Memory leak. Metric data was not deleted: Metric name - '%s'", pair.first->c_str());
    }

  }

}

v_io_size ConnectionMonitor::ConnectionProxy::read(void *buffer, v_buff_size count, async::Action& action) {
  auto res = m_connectionHandle.object->read(buffer, count, action);
  std::lock_guard<std::mutex> lock(m_statsMutex);
  m_monitor->onConnectionRead(m_stats, res);
  return res;
}

v_io_size ConnectionMonitor::ConnectionProxy::write(const void *data, v_buff_size count, async::Action& action) {
  auto res = m_connectionHandle.object->write(data, count, action);
  std::lock_guard<std::mutex> lock(m_statsMutex);
  m_monitor->onConnectionWrite(m_stats, res);
  return res;
}

void ConnectionMonitor::ConnectionProxy::setInputStreamIOMode(data::stream::IOMode ioMode) {
  m_connectionHandle.object->setInputStreamIOMode(ioMode);
}
data::stream::IOMode ConnectionMonitor::ConnectionProxy::getInputStreamIOMode() {
  return m_connectionHandle.object->getInputStreamIOMode();
}
data::stream::Context& ConnectionMonitor::ConnectionProxy::getInputStreamContext() {
  return m_connectionHandle.object->getInputStreamContext();
}

void ConnectionMonitor::ConnectionProxy::setOutputStreamIOMode(data::stream::IOMode ioMode) {
  m_connectionHandle.object->setOutputStreamIOMode(ioMode);
}

data::stream::IOMode ConnectionMonitor::ConnectionProxy::getOutputStreamIOMode() {
  return m_connectionHandle.object->getOutputStreamIOMode();
}

data::stream::Context& ConnectionMonitor::ConnectionProxy::getOutputStreamContext() {
  return m_connectionHandle.object->getOutputStreamContext();
}

void ConnectionMonitor::ConnectionProxy::invalidate()  {
  m_connectionHandle.invalidator->invalidate(m_connectionHandle.object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Monitor

void ConnectionMonitor::Monitor::monitorTask(std::shared_ptr<Monitor> monitor) {

  while(monitor->m_running) {

    {

      std::lock_guard<std::mutex> lock(monitor->m_connectionsMutex);

      auto currMicroTime = oatpp::base::Environment::getMicroTickCount();

      for(auto& caddr : monitor->m_connections) {

        auto connection = (ConnectionProxy*) caddr;
        std::lock_guard<std::mutex> dataLock(connection->m_statsMutex);
        std::lock_guard<std::mutex> analysersLock(monitor->m_checkMutex);

        for(auto& a : monitor->m_metricsCheckers) {
          bool res = a->check(connection->m_stats, currMicroTime);
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
  auto it = stats.metricsData.find(collector->metricName());
  if(it == stats.metricsData.end()) {
    data = collector->createMetricData();
    stats.metricsData.insert({collector->metricName(), data});
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

void ConnectionMonitor::Monitor::addConnection(ConnectionProxy* connection) {
  std::lock_guard<std::mutex> lock(m_connectionsMutex);
  m_connections.insert((v_uint64) connection);
}

void ConnectionMonitor::Monitor::freeConnectionStats(ConnectionStats& stats) {

  std::lock_guard<std::mutex> lock(m_checkMutex);

  for(auto& metric : stats.metricsData) {
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
  std::lock_guard<std::mutex> lock(m_checkMutex);
  m_statCollectors.insert({collector->metricName(), collector});
}

void ConnectionMonitor::Monitor::removeStatCollector(const oatpp::String& metricName) {
  std::lock_guard<std::mutex> lock(m_checkMutex);
  m_statCollectors.erase(metricName);
}

void ConnectionMonitor::Monitor::addMetricsChecker(const std::shared_ptr<MetricsChecker>& checker) {
  std::lock_guard<std::mutex> lock(m_checkMutex);
  m_metricsCheckers.push_back(checker);
  auto metrics = checker->getMetricsList();
  for(auto& m : metrics) {
    auto it = m_statCollectors.find(m);
    if(it == m_statCollectors.end()) {
      m_statCollectors.insert({m, checker->createStatCollector(m)});
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
    std::lock_guard<std::mutex> lock(m_checkMutex);
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
    std::lock_guard<std::mutex> lock(m_checkMutex);
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
  : m_invalidator(std::make_shared<ConnectionInvalidator>())
  , m_monitor(Monitor::createShared())
  , m_connectionProvider(connectionProvider)
{
}

provider::ResourceHandle<data::stream::IOStream> ConnectionMonitor::get() {
  auto connection = m_connectionProvider->get();
  if(!connection) {
    return nullptr;
  }
  auto proxy = std::make_shared<ConnectionProxy>(m_monitor, connection);
  m_monitor->addConnection(proxy.get());
  return provider::ResourceHandle<data::stream::IOStream>(proxy, m_invalidator);
}

async::CoroutineStarterForResult<const provider::ResourceHandle<data::stream::IOStream>&>
ConnectionMonitor::getAsync() {

  class GetConnectionCoroutine : public async::CoroutineWithResult<GetConnectionCoroutine, const provider::ResourceHandle<data::stream::IOStream>&> {
  private:
    std::shared_ptr<Monitor> m_monitor;
    std::shared_ptr<ConnectionProvider> m_connectionProvider;
    std::shared_ptr<ConnectionInvalidator> m_invalidator;
  public:

    GetConnectionCoroutine(const std::shared_ptr<Monitor>& monitor,
                           const std::shared_ptr<ConnectionProvider>& connectionProvider,
                           const std::shared_ptr<ConnectionInvalidator>& invalidator)
      : m_monitor(monitor)
      , m_connectionProvider(connectionProvider)
      , m_invalidator(invalidator)
    {}

    Action act() override {
      return m_connectionProvider->getAsync().callbackTo(&GetConnectionCoroutine::onConnection);
    }

    Action onConnection(const provider::ResourceHandle<data::stream::IOStream>& connection) {
      if(!connection) {
        return _return(nullptr);
      }
      auto proxy = std::make_shared<ConnectionProxy>(m_monitor, connection);
      m_monitor->addConnection(proxy.get());
      return _return(provider::ResourceHandle<data::stream::IOStream>(proxy, m_invalidator));
    }

  };

  return GetConnectionCoroutine::startForResult(m_monitor, m_connectionProvider, m_invalidator);

}

void ConnectionMonitor::addStatCollector(const std::shared_ptr<StatCollector>& collector) {
  m_monitor->addStatCollector(collector);
}

void ConnectionMonitor::addMetricsChecker(const std::shared_ptr<MetricsChecker>& checker) {
  m_monitor->addMetricsChecker(checker);
}

void ConnectionMonitor::stop() {
  m_monitor->stop();
}

}}}
