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

#include "./HttpProcessor.hpp"

#include "../protocol/http/outgoing/ChunkedBufferBody.hpp"

#include "../protocol/http/incoming/Request.hpp"
#include "../protocol/http/Http.hpp"

#include "./HttpError.hpp"

#include "../../../../oatpp-lib/network/src/io/AsyncIOStream.hpp"
#include "../../../../oatpp-lib/network/src/AsyncConnection.hpp"

#include "../../../../oatpp-lib/core/test/Checker.hpp"

#include <errno.h>

namespace oatpp { namespace web { namespace server {
  
void AsyncHttpConnectionHandler::Task::run(){
  
  //Backlog& backlog = Backlog::getInstance();
  
  while(true) {
    
    Backlog::Entry* entry = m_backLog.popFront();
    if(entry != nullptr) {
      auto& state = entry->connectionState;
      auto response = HttpProcessor::processRequest(m_router, state->connection, m_errorHandler,
                                                    state->ioBuffer->getData(),
                                                    state->ioBuffer->getSize(),
                                                    state->inStream,
                                                    state->keepAlive);
      if(response) {
        state->outStream->setBufferPosition(0, 0);
        response->send(state->outStream);
        state->outStream->flush();
      }
      
      if(state->keepAlive){
        m_backLog.pushBack(entry);
      } else {
        delete entry;
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
  }
  
}

void AsyncHttpConnectionHandler::handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection){
  auto task = m_tasks[m_taskBalancer % m_threadCount];
  
  auto asyncConnection = oatpp::network::io::AsyncIOStream::createShared(connection, task->getIOQueue());
  
  auto ioBuffer = oatpp::data::buffer::IOBuffer::createShared();
  auto state = ConnectionState::createShared();
  state->connection = asyncConnection;
  state->ioBuffer = ioBuffer;
  state->outStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(asyncConnection, ioBuffer);
  state->inStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(asyncConnection, ioBuffer);
  state->keepAlive = true;
  
  //Backlog::getInstance().pushBack(state);
  task->getBacklog().pushBack(state);
  
  m_taskBalancer ++;
}
  
}}}

