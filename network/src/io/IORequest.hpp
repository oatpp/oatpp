//
//  IORequest.hpp
//  crud
//
//  Created by Leonid on 3/15/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef oatpp_web_server_io_IORequest_hpp
#define oatpp_web_server_io_IORequest_hpp

#include "../../../core/src/data/stream/Stream.hpp"

#include <atomic>

namespace oatpp { namespace network { namespace io {
  
class IORequest {
public:
  static const v_int32 TYPE_DONE;
  static const v_int32 TYPE_READ;
  static const v_int32 TYPE_WRITE;
public:
  
  IORequest(const std::shared_ptr<oatpp::data::stream::IOStream>& pStream,
            void* pData,
            os::io::Library::v_size pSize,
            v_int32 pType)
    : stream(pStream)
    , data(pData)
    , size(pSize)
    , type(pType)
    , actualSize(0)
  {}
  
  const std::shared_ptr<oatpp::data::stream::IOStream>& stream;
  void* data;
  const os::io::Library::v_size size;
  v_int32 type;
  os::io::Library::v_size actualSize;
  
};
  
}}}

#endif /* oatpp_web_server_io_IORequest_hpp */
