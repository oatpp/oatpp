/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "./AsyncHttpConnectionHandler.hpp"

#include "../protocol/http/outgoing/ChunkedBufferBody.hpp"

#include "../protocol/http/incoming/Request.hpp"
#include "../protocol/http/Http.hpp"

#include "./HttpError.hpp"

#include "../../../../oatpp-lib/network/src/AsyncConnection.hpp"

#include "../../../../oatpp-lib/core/test/Checker.hpp"

#include <errno.h>

namespace oatpp { namespace web { namespace server {
  
void AsyncHttpConnectionHandler::Task::consumeConnections(oatpp::async::Processor2& processor) {
  
  class ConnectionCoroutine : public oatpp::async::Coroutine<ConnectionCoroutine> {
  public:
    
    ConnectionCoroutine(const std::shared_ptr<HttpProcessor::ConnectionState>& pState)
      : state(pState)
    {}
    
    std::shared_ptr<HttpProcessor::ConnectionState> state;
    
    Action2 act() override {
      return finish();
    };
    
  };
  
  oatpp::concurrency::SpinLock lock(m_atom);
  
  auto curr = m_connections.getFirstNode();
  while (curr != nullptr) {
    processor.addCoroutine(ConnectionCoroutine::getBench().obtain(curr->getData()));
    curr = curr->getNext();
  }
  
}
  
void AsyncHttpConnectionHandler::Task::run(){
  
  oatpp::async::Processor2 processor;
  
  while(true) {
    consumeConnections(processor);
    while (processor.iterate(100)) {
      consumeConnections(processor);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
}

void AsyncHttpConnectionHandler::handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection){
  
  auto task = m_tasks[m_taskBalancer % m_threadCount];
  
  auto ioBuffer = oatpp::data::buffer::IOBuffer::createShared();
  auto state = HttpProcessor::ConnectionState::createShared();
  state->connection = connection;
  state->ioBuffer = ioBuffer;
  state->outStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(connection, ioBuffer);
  state->inStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(connection, ioBuffer);
  state->keepAlive = true;
  
  auto routine = oatpp::async::Routine::_do({
    [this, state]{
      return HttpProcessor::processRequestAsync(m_router.get(), m_errorHandler, state);
    }, [] (const oatpp::async::Error& error) {
      if(error.error == HttpProcessor::RETURN_KEEP_ALIVE) {
        return oatpp::async::Action::_repeat();
      }
      return oatpp::async::Action::_abort();
    }
  });
  
  task->addConnection(state);
  
  m_taskBalancer ++;
  
}
  
}}}

