//
//  AsyncConnection.cpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "AsyncConnection.hpp"

#include "io/AsyncIOStream.hpp"

namespace oatpp { namespace network {
  
AsyncConnection::AsyncConnection(Library::v_handle handle)
  : m_handle(handle)
{}

AsyncConnection::~AsyncConnection(){
  close();
}

AsyncConnection::Library::v_size AsyncConnection::write(const void *buff, Library::v_size count){
  auto result = Library::handle_write(m_handle, buff, count); // Socket should be non blocking!!!
  if(result == EAGAIN || result == EWOULDBLOCK){
    return io::AsyncIOStream::ERROR_TRY_AGAIN;
  } else if(result == -1) {
    return io::AsyncIOStream::ERROR_NOTHING_TO_READ;
  } else if(result == 0) {
    return io::AsyncIOStream::ERROR_CLOSED;
  }
  return result;
}

AsyncConnection::Library::v_size AsyncConnection::read(void *buff, Library::v_size count){
  auto result = Library::handle_read(m_handle, buff, count); // Socket should be non blocking!!!
  if(result == EAGAIN || result == EWOULDBLOCK){
    return io::AsyncIOStream::ERROR_TRY_AGAIN;
  } else if(result == -1) {
    return io::AsyncIOStream::ERROR_NOTHING_TO_READ;
  } else if(result == 0) {
    return io::AsyncIOStream::ERROR_CLOSED;
  }
  return result;
}

void AsyncConnection::close(){
  Library::handle_close(m_handle);
}
  
}}
