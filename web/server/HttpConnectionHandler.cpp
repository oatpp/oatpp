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

#include "./HttpConnectionHandler.hpp"
#include "./HttpError.hpp"

#include "oatpp/web/protocol/http/outgoing/ChunkedBufferBody.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

namespace oatpp { namespace web { namespace server {
  
void HttpConnectionHandler::Task::run(){
  
  v_int32 bufferSize = oatpp::data::buffer::IOBuffer::BUFFER_SIZE;
  v_char8 buffer [bufferSize];
  
  auto outStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  auto inStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  
  bool keepAlive = true;
  do {
  
    auto response = HttpProcessor::processRequest(m_router, m_connection, m_bodyDecoder, m_errorHandler, m_requestInterceptors, buffer, bufferSize, inStream, keepAlive);
    
    if(response) {
      outStream->setBufferPosition(0, 0);
      response->send(outStream);
      outStream->flush();
    } else {
      return;
    }
    
  } while(keepAlive);
  
}
  
void HttpConnectionHandler::handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection){
  
  /* Create working thread */
  concurrency::Thread thread(Task::createShared(m_router.get(), connection, m_bodyDecoder, m_errorHandler, &m_requestInterceptors));
  
  /* Get hardware concurrency -1 in order to have 1cpu free of workers. */
  v_int32 concurrency = oatpp::concurrency::Thread::getHardwareConcurrency();
  if(concurrency > 1) {
    concurrency -= 1;
  }
  
  /* Set thread affinity group CPUs [0..cpu_count - 1]. Leave one cpu free of workers */
  oatpp::concurrency::Thread::setThreadAffinityToCpuRange(thread.getStdThread()->native_handle(), 0, concurrency - 1 /* -1 because 0-based index */);
  
  thread.detach();
}

}}}
