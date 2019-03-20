/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
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

#ifndef oatpp_data_IODefinitions_hpp
#define oatpp_data_IODefinitions_hpp

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace data {

/**
 * Represents I/O handle (ex.: file descriptor).
 */
typedef int v_io_handle;

/**
 * All I/O buffer operations (like read/write(buffer, size)) should return v_io_size. <br>
 *
 * Possible return values:
 * <ul>
 *   <li>**On Success** - [1..max_int64].</li>
 *   <li>**On Error** - IOError values.</li>
 * </ul>
 *
 * All other values are considered to be a fatal error.
 * application should be terminated.
 */
typedef v_int64 v_io_size;

/**
 * Final set of possible I/O operation error values.
 * I/O operation should not return any other error values.
 */
enum IOError : v_io_size {

  /**
   * In oatpp 0 is considered to be an Error as for I/O operation size
   *
   * As for argument value 0 should be handled separately of the main flow.
   *
   * As for return value 0 should not be returned.
   * I/O method should return an error describing a reason why I/O is empty instead of a zero itself.
   * if zero is returned, client should treat it like a bad api implementation and as an error in the flow.
   */
  ZERO_VALUE = 0,

  /**
   * I/O operation is not possible any more
   * Client should give up trying and free all related resources
   */
  BROKEN_PIPE = -1001,

  /**
   * I/O operation was interrupted because of some reason
   * Client may retry immediately
   */
  RETRY = -1002,

  /**
   * I/O operation is not currently available due to some reason
   * Client should wait then retry
   */
  WAIT_RETRY = -1003

};

}}

#endif //oatpp_data_IODefinitions_hpp
