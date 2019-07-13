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

#ifndef oatpp_data_stream_BufferInputStream_hpp
#define oatpp_data_stream_BufferInputStream_hpp

#include "Stream.hpp"

namespace oatpp { namespace data{ namespace stream {

class BufferInputStream : public InputStream {
private:
  std::shared_ptr<base::StrBuffer> m_memoryHandle;
  p_char8 m_data;
  v_io_size m_size;
  v_io_size m_position;
  IOMode m_ioMode;
public:

  /**
   * Constructor.
   * @param memoryHandle - buffer memory handle. May be nullptr.
   * @param data - pointer to buffer data.
   * @param size - size of the buffer.
   */
  BufferInputStream(const std::shared_ptr<base::StrBuffer>& memoryHandle, p_char8 data, v_io_size size);

  /**
   * Read data from stream. <br>
   * It is a legal case if return result < count. Caller should handle this!
   * *Calls to this method are always NON-BLOCKING*
   * @param data - buffer to read data to.
   * @param count - size of the buffer.
   * @return - actual number of bytes read. 0 - designates end of the buffer.
   */
  data::v_io_size read(void *data, data::v_io_size count) override;

  /**
   * Not expected to be called because read method should always return correct amount or zero.
   * @throws - `std::runtime_error`.
   */
  oatpp::async::Action suggestInputStreamAction(data::v_io_size ioResult) override {
    const char* message =
      "Error. oatpp::data::stream::BufferInputStream::suggestOutputStreamAction() method is called.\n"
      "No suggestions for BufferInputStream async I/O operations are needed.\n "
      "BufferInputStream always satisfies call to read() method or returns 0 - as EOF.";
    throw std::runtime_error(message);
  }

  /**
   * Set stream I/O mode.
   * @throws
   */
  void setInputStreamIOMode(IOMode ioMode) override;

  /**
   * Get stream I/O mode.
   * @return
   */
  IOMode getInputStreamIOMode() override;

  /**
   * Get data memory handle.
   * @return - data memory handle.
   */
  std::shared_ptr<base::StrBuffer> getDataMemoryHandle();

  /**
   * Get pointer to data.
   * @return - pointer to data.
   */
  p_char8 getData();

  /**
   * Get data size.
   * @return - data size.
   */
  v_io_size getDataSize();

  /**
   * Get current data read position.
   * @return - current data read position.
   */
  v_io_size getCurrentPosition();

  /**
   * Reset current data read position to zero.
   */
  void resetPosition();


};

}}}

#endif // oatpp_data_stream_BufferInputStream_hpp
