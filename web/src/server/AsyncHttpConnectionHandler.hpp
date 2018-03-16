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

#ifndef oatpp_web_server_AsyncHttpConnectionHandler_hpp
#define oatpp_web_server_AsyncHttpConnectionHandler_hpp

#include "./handler/ErrorHandler.hpp"

#include "./HttpRouter.hpp"

#include "../protocol/http/incoming/Request.hpp"
#include "../protocol/http/outgoing/Response.hpp"

#include "../../../../oatpp-lib/network/src/io/Queue.hpp"

#include "../../../../oatpp-lib/network/src/server/Server.hpp"
#include "../../../../oatpp-lib/network/src/Connection.hpp"

#include "../../../../oatpp-lib/core/src/concurrency/Thread.hpp"
#include "../../../../oatpp-lib/core/src/concurrency/Runnable.hpp"

#include "../../../../oatpp-lib/core/src/data/stream/StreamBufferedProxy.hpp"
#include "../../../../oatpp-lib/core/src/data/buffer/IOBuffer.hpp"

namespace oatpp { namespace web { namespace server {
  
class AsyncHttpConnectionHandler : public base::Controllable, public network::server::ConnectionHandler {
private:
  
  class ConnectionState {
    SHARED_OBJECT_POOL(ConnectionState_Pool, ConnectionState, 32)
  public:
    
    static std::shared_ptr<ConnectionState> createShared(){
      return ConnectionState_Pool::allocateShared();
    }
    
    std::shared_ptr<oatpp::data::stream::IOStream> connection;
    std::shared_ptr<oatpp::data::buffer::IOBuffer> ioBuffer;
    std::shared_ptr<oatpp::data::stream::OutputStreamBufferedProxy> outStream;
    std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> inStream;
    bool keepAlive = true;
    
  };
  
  class Backlog {
  public:
    /*static Backlog& getInstance(){
      static Backlog backlog;
      return backlog;
    }*/
  public:
    class Entry {
    public:
      OBJECT_POOL(AsyncHttpConnectionHandler_Backlog_Entry_Pool, Entry, 32)
    public:
      Entry(const std::shared_ptr<ConnectionState>& pConnectionState, Entry* pNext)
      : connectionState(pConnectionState)
      , next(pNext)
      {}
      std::shared_ptr<ConnectionState> connectionState;
      Entry* next;
    };
  private:
    oatpp::concurrency::SpinLock::Atom m_atom;
    Entry* m_first;
    Entry* m_last;
  public:
    
    Backlog()
    : m_atom(false)
    , m_first(nullptr)
    , m_last(nullptr)
    {}
    
    Entry* popFront() {
      oatpp::concurrency::SpinLock lock(m_atom);
      auto result = m_first;
      if(m_first != nullptr) {
        m_first = m_first->next;
        if(m_first == nullptr){
          m_last = nullptr;
        }
      }
      return result;
    }
    
    void pushBack(const std::shared_ptr<ConnectionState>& connectionState) {
      pushBack(new Entry(connectionState, nullptr));
    }
    
    void pushBack(Entry* entry) {
      oatpp::concurrency::SpinLock lock(m_atom);
      entry->next = nullptr;
      if(m_last != nullptr) {
        m_last->next = entry;
        m_last = entry;
      } else {
        m_first = entry;
        m_last = entry;
      }
    }
    
  };
  
private:
  
  class Task : public base::Controllable, public concurrency::Runnable{
  private:
    HttpRouter* m_router;
    std::shared_ptr<handler::ErrorHandler> m_errorHandler;
    oatpp::network::io::Queue m_ioQueue;
    Backlog m_backLog;
  public:
    Task(HttpRouter* router,
         const std::shared_ptr<handler::ErrorHandler>& errorHandler)
      : m_router(router)
      , m_errorHandler(errorHandler)
    {}
  public:
    
    static std::shared_ptr<Task> createShared(HttpRouter* router,
                                              const std::shared_ptr<handler::ErrorHandler>& errorHandler){
      return std::shared_ptr<Task>(new Task(router, errorHandler));
    }
    
    void run() override;
    
    oatpp::network::io::Queue& getIOQueue(){
      return m_ioQueue;
    }
    
    Backlog& getBacklog(){
      return m_backLog;
    }
    
  };
  
private:
  std::shared_ptr<HttpRouter> m_router;
  std::shared_ptr<handler::ErrorHandler> m_errorHandler;
  v_int32 m_taskBalancer;
  v_int32 m_threadCount;
  std::shared_ptr<Task>* m_tasks;
public:
  AsyncHttpConnectionHandler(const std::shared_ptr<HttpRouter>& router)
    : m_router(router)
    , m_errorHandler(handler::DefaultErrorHandler::createShared())
    , m_taskBalancer(0)
    , m_threadCount(2)
  {
    m_tasks = new std::shared_ptr<Task>[m_threadCount];
    for(v_int32 i = 0; i < m_threadCount; i++) {
      auto task = Task::createShared(m_router.get(), m_errorHandler);
      m_tasks[i] = task;
      concurrency::Thread thread(task);
      thread.detach();
    }
  }
public:
  
  static std::shared_ptr<AsyncHttpConnectionHandler> createShared(const std::shared_ptr<HttpRouter>& router){
    return std::shared_ptr<AsyncHttpConnectionHandler>(new AsyncHttpConnectionHandler(router));
  }
  
  ~AsyncHttpConnectionHandler(){
    delete [] m_tasks;
  }
  
  void setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
    m_errorHandler = errorHandler;
    if(!m_errorHandler) {
      m_errorHandler = handler::DefaultErrorHandler::createShared();
    }
  }
  
  void handleConnection(const std::shared_ptr<oatpp::data::stream::IOStream>& connection) override;
  
};
  
}}}

#endif /* oatpp_web_server_AsyncHttpConnectionHandler_hpp */

