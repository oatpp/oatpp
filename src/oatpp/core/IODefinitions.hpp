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

#ifndef oatpp_IODefinitions_hpp
#define oatpp_IODefinitions_hpp

#include "oatpp/core/async/Error.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp {

/**
 * Represents I/O handle (ex.: file descriptor).
 */
#if defined(WIN32) || defined(_WIN32)
  #if defined(_WIN64)
    typedef unsigned long long v_io_handle;
  #else
    typedef unsigned long v_io_handle;
  #endif
    constexpr const v_io_handle INVALID_IO_HANDLE = v_io_handle (-1);
#else
  typedef int v_io_handle;
  constexpr const v_io_handle INVALID_IO_HANDLE = v_io_handle (-1);
#endif

/**
 * Check if IO handle is valid.
 * @param handle - IO handle.
 * @return - `true` if valid.
 */
bool isValidIOHandle(v_io_handle handle);

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
   * In oatpp 0 is considered to be an Error as for I/O operation size. <br>
   *
   * As for argument value 0 should be handled separately of the main flow.<br>
   *
   * As for return value 0 should not be returned.<br>
   * I/O method should return an error describing a reason why I/O is empty instead of a zero itself.<br>
   * if zero is returned, client should treat it like a bad api implementation and as an error in the flow.<br>
   */
  ZERO_VALUE = 0,

  /**
   * I/O operation is not possible any more.
   * Client should give up trying and free all related resources.
   */
  BROKEN_PIPE = -1001,

  /**
   * I/O operation was interrupted because of some reason.
   * Client may retry read immediately.
   */
  RETRY_READ = -1002,

  /**
   * I/O operation was interrupted because of some reason.
   * Client may retry immediately.
   */
  RETRY_WRITE = -1003,

};

/**
 * Asynchronous I/O error. <br>
 * Extends &id:oatpp::async::Error;.
 */
class AsyncIOError : public oatpp::async::Error {
private:
  v_io_size m_code;
public:

  /**
   * Constructor.
   * @param what - description of error type.
   * @param code - I/O opersation error code. &l:IOError;.
   */
  AsyncIOError(const char* what, v_io_size code)
    : oatpp::async::Error(what)
    , m_code(code)
  {}

  /**
   * Constructor.
   * @param code - I/O opersation error code. &l:IOError;.
   */
  AsyncIOError(v_io_size code)
    : oatpp::async::Error("AsyncIOError")
    , m_code(code)
  {}

  /**
   * Get I/O opersation error code.
   * @return - I/O opersation error code. &l:IOError;.
   */
  v_io_size getCode() const {
    return m_code;
  }

};

}

#endif // oatpp_IODefinitions_hpp
