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

#include "oatpp/test/Checker.hpp"

#include <errno.h>

//#include <sched.h>
//#include <pthread.h>

namespace oatpp { namespace web { namespace server {
  
void HttpConnectionHandler::Task::run(){
  
  //OATPP_LOGD("thread", "running on cpu %d", sched_getcpu());
  
  //oatpp::test::PerformanceChecker checker("task checker");
  
  v_int32 bufferSize = oatpp::data::buffer::IOBuffer::BUFFER_SIZE;
  v_char8 buffer [bufferSize];
  auto outStream = oatpp::data::stream::OutputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  auto inStream = oatpp::data::stream::InputStreamBufferedProxy::createShared(m_connection, buffer, bufferSize);
  
  bool keepAlive = true;
  do {
  
    std::shared_ptr<protocol::http::outgoing::Response> response;
    
    {
      PriorityController::PriorityLine priorityLine = m_priorityController->getLine();
      priorityLine.wait();
      
      response = HttpProcessor::processRequest(m_router, m_connection, m_errorHandler, m_requestInterceptors, buffer, bufferSize, inStream, keepAlive);
    }
    //auto response = HttpProcessor::processRequest(m_router, m_connection, m_errorHandler, m_requestInterceptors, buffer, bufferSize, inStream, keepAlive);
    
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
  concurrency::Thread thread(Task::createShared(m_router.get(), connection, m_errorHandler, &m_requestInterceptors, &m_priorityController));
  
  /*
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(oatpp::concurrency::Thread::getSuggestedCpuIndex(3), &cpuset);
  
  int rc = pthread_setaffinity_np(thread.getThread()->native_handle(), sizeof(cpu_set_t), &cpuset);
  
  if (rc != 0) {
    OATPP_LOGD("task", "setting cpu error %d", rc);
  }
  */
   
  thread.detach();
}

}}}
