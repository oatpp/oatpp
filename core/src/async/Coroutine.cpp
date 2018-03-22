//
//  Coroutine.cpp
//  crud
//
//  Created by Leonid on 3/22/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "Coroutine.hpp"

namespace oatpp { namespace async {
  
const v_int32 Action2::TYPE_COROUTINE = 0;
const v_int32 Action2::TYPE_YIELD_TO = 1;
const v_int32 Action2::TYPE_WAIT_RETRY = 2;
const v_int32 Action2::TYPE_REPEAT = 3;
const v_int32 Action2::TYPE_FINISH = 4;
const v_int32 Action2::TYPE_ABORT = 5;
const v_int32 Action2::TYPE_ERROR = 6;
  
}}
