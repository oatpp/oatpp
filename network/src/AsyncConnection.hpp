//
//  AsyncConnection.hpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_network_AsyncConnection_hpp
#define oatpp_network_AsyncConnection_hpp

#include "../../../oatpp-lib/core/src/base/memory/ObjectPool.hpp"
#include "../../../oatpp-lib/core/src/data/stream/Stream.hpp"
#include "../../../oatpp-lib/core/src/async/Routine.hpp"
#include "../../../oatpp-lib/core/src/os/io/Library.hpp"

namespace oatpp { namespace network {
  
class AsyncConnection : public oatpp::base::Controllable, public oatpp::data::stream::IOStream {
public:
  typedef oatpp::os::io::Library Library;
public:
  OBJECT_POOL(AsyncConnection_Pool, AsyncConnection, 32);
  SHARED_OBJECT_POOL(Shared_AsyncConnection_Pool, AsyncConnection, 32);
private:
  Library::v_handle m_handle;
public:
  AsyncConnection(Library::v_handle handle);
public:
  
  static std::shared_ptr<AsyncConnection> createShared(Library::v_handle handle){
    return Shared_AsyncConnection_Pool::allocateShared(handle);
  }
  
  ~AsyncConnection();
  
  Library::v_size write(const void *buff, Library::v_size count) override;
  Library::v_size read(void *buff, Library::v_size count) override;
  
  void close();
  
  Library::v_handle getHandle(){
    return m_handle;
  }
  
};
  
}}

#endif /* oatpp_network_AsyncConnection_hpp */
