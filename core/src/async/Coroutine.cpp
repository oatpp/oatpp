//
//  Coroutine.cpp
//  crud
//
//  Created by Leonid on 3/22/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "Coroutine.hpp"

namespace oatpp { namespace async {
  
const Action2 Action2::_WAIT_RETRY(TYPE_WAIT_RETRY, nullptr, nullptr);
const Action2 Action2::_REPEAT(TYPE_REPEAT, nullptr, nullptr);
const Action2 Action2::_FINISH(TYPE_FINISH, nullptr, nullptr);
const Action2 Action2::_ABORT(TYPE_ABORT, nullptr, nullptr);
  
}}
