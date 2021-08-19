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

#ifndef oatpp_network_ConnectionMonitor_hpp
#define oatpp_network_ConnectionMonitor_hpp

#include "./ConnectionProvider.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace network {

class ConnectionMonitor {
private:

  class ConnectionProxy : public data::stream::IOStream {
  private:
    /* provider which created this connection */
    std::shared_ptr<ConnectionProvider> m_connectionProvider;
    std::shared_ptr<data::stream::IOStream> m_connection;
  public:

    ConnectionProxy(const std::shared_ptr<ConnectionProvider>& connectionProvider,
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

  };

private:

  static void monitorTask(std::shared_ptr<ConnectionMonitor> monitor);

private:
  std::atomic<bool> m_running {true};
public:



};

}}

#endif //oatpp_network_ConnectionMonitor_hpp
