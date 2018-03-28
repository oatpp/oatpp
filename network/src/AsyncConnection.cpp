//
//  AsyncConnection.cpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "AsyncConnection.hpp"

namespace oatpp { namespace network {
  
AsyncConnection::AsyncConnection(Library::v_handle handle)
  : m_handle(handle)
{}

AsyncConnection::~AsyncConnection(){
  close();
}

AsyncConnection::Library::v_size AsyncConnection::write(const void *buff, Library::v_size count){
  auto result = Library::handle_write(m_handle, buff, count); // Socket should be non blocking!!!
  
  if(result < 0) {
    auto e = errno;
    //OATPP_LOGD("write", "errno=%d", e);
    if(e == EAGAIN || e == EWOULDBLOCK){
      return ERROR_TRY_AGAIN;
    }
  }
  
  return result;
}

AsyncConnection::Library::v_size AsyncConnection::read(void *buff, Library::v_size count){
  //OATPP_LOGD("AsyncConnection", "read. handler=%d", m_handle);
  auto result = Library::handle_read(m_handle, buff, count); // Socket should be non blocking!!!
  if(result < 0) {
    auto e = errno;
    //OATPP_LOGD("read", "errno=%d", e);
    if(e == EAGAIN || e == EWOULDBLOCK){
      return ERROR_TRY_AGAIN;
    }
  }
  return result;
}

void AsyncConnection::close(){
  //OATPP_LOGD("Connection", "close()");
  Library::handle_close(m_handle);
}
  
}}
