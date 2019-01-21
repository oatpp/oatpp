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

#ifndef oatpp_os_io_Library_hpp
#define oatpp_os_io_Library_hpp

#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace os { namespace io {
  
class Library{
public:
  typedef v_int32 v_handle;
  typedef ssize_t v_size;
public:
  
  static v_int32 handle_close(v_handle handle);
  
  static v_size handle_read(v_handle handle, void *buf, v_size count);
  static v_size handle_write(v_handle handle, const void *buf, v_size count);
  
};
  
}}}

#endif /* oatpp_os_io_Library_hpp */
