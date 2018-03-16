//
//  AsyncIOStream.hpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_web_server_io_AsyncIOStream_hpp
#define oatpp_web_server_io_AsyncIOStream_hpp

#include "./Queue.hpp"

#include "../../../core/src/base/memory/ObjectPool.hpp"
#include "../../../core/src/data/stream/Stream.hpp"
#include "../../../core/src/base/Controllable.hpp"

namespace oatpp { namespace network { namespace io {
  
class AsyncIOStream : public oatpp::base::Controllable, public oatpp::data::stream::IOStream {
public:
  typedef oatpp::os::io::Library Library;
public:
  static const v_int32 ERROR_TRY_AGAIN;
  static const v_int32 ERROR_NOTHING_TO_READ;
  static const v_int32 ERROR_CLOSED;
public:
  OBJECT_POOL(AsyncIOStream_Pool, AsyncIOStream, 32);
  SHARED_OBJECT_POOL(Shared_AsyncIOStream_Pool, AsyncIOStream, 32);
private:
  std::shared_ptr<oatpp::data::stream::IOStream> m_stream;
  Queue& m_queue;
private:
  void processRequest(IORequest& request) {
    m_queue.pushBack(request);
    while (request.type != IORequest::TYPE_DONE) {
      Queue::Entry* entry = m_queue.popFront();
      if(entry != nullptr) {
        IORequest& request = entry->request;
        if(request.type == IORequest::TYPE_READ) {
          request.actualSize = request.stream->read(request.data, request.size);
        } else if(request.type == IORequest::TYPE_WRITE) {
          request.actualSize = request.stream->write(request.data, request.size);
        } else {
          throw std::runtime_error("Invalid state of IORequest");
        }
        if(request.actualSize != ERROR_TRY_AGAIN) {
          request.type = IORequest::TYPE_DONE;
          delete entry;
        } else {
          m_queue.pushBack(entry);
        }
      } else if(request.type == IORequest::TYPE_DONE) {
        return;
      } else {
        throw std::runtime_error("Invalid state of Queue");
      }
      std::this_thread::yield();
    }
  }
public:
  AsyncIOStream(const std::shared_ptr<oatpp::data::stream::IOStream>& stream,
                Queue& queue = Queue::getInstance())
    : m_stream(stream)
    , m_queue(queue)
  {}
public:
  
  static std::shared_ptr<AsyncIOStream> createShared(const std::shared_ptr<oatpp::data::stream::IOStream>& stream,
                                                     Queue& queue = Queue::getInstance()){
    return Shared_AsyncIOStream_Pool::allocateShared(stream, std::ref(queue));
  }
  
  Library::v_size write(const void *buff, Library::v_size count) override {
    //auto result = m_stream->write(buff, count);
    //if(result == ERROR_TRY_AGAIN) {
      IORequest request(m_stream, const_cast<void*>(buff), count, IORequest::TYPE_WRITE);
      processRequest(request);
      return request.actualSize;
    //}
    //return result;
  }
  
  Library::v_size read(void *buff, Library::v_size count) override {
    //auto result = m_stream->read(buff, count);
    //if(result == ERROR_TRY_AGAIN) {
      IORequest request(m_stream, buff, count, IORequest::TYPE_READ);
      processRequest(request);
      return request.actualSize;
    //}
    //return result;
  }

};
  
}}}

#endif /* oatpp_web_server_io_AsyncIOStream_hpp */
