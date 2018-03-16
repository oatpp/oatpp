//
//  Connection.cpp
//  crud
//
//  Created by Leonid on 3/16/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "AsyncIOStream.hpp"

namespace oatpp { namespace network { namespace io {

const v_int32 AsyncIOStream::ERROR_TRY_AGAIN = -1001;
const v_int32 AsyncIOStream::ERROR_NOTHING_TO_READ = -1002;
const v_int32 AsyncIOStream::ERROR_CLOSED = -1003;
  
}}}
