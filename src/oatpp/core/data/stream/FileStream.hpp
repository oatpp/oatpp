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

#ifndef oatpp_data_stream_FileStream_hpp
#define oatpp_data_stream_FileStream_hpp

#include "Stream.hpp"

#include <cstdio>

namespace oatpp { namespace data{ namespace stream {

/**
 * Wrapper over `std::FILE`.
 */
class FileInputStream : public InputStream {
public:
  static oatpp::data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
private:
  std::FILE* m_file;
  bool m_ownsFile;
  IOMode m_ioMode;
private:
  std::shared_ptr<void> m_capturedData;
public:

  FileInputStream(const FileInputStream&) = delete;
  FileInputStream &operator=(const FileInputStream&) = delete;

  /**
   * Move constructor.
   * @param other
   */
  FileInputStream(FileInputStream&& other);

  /**
   * Constructor.
   * @param file - file.
   * @param ownsFile - if `true` then call close on `FileInputStream` destruction.
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  FileInputStream(std::FILE* file, bool ownsFile, const std::shared_ptr<void>& captureData = nullptr);

  /**
   * Constructor.
   * @param filename - name of the file.
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  FileInputStream(const char* filename, const std::shared_ptr<void>& captureData = nullptr);

  /**
   * Virtual destructor.
   */
  ~FileInputStream();

  /**
   * Get file.
   * @return
   */
  std::FILE* getFile();

  /**
   * Read data from stream up to count bytes, and return number of bytes actually read. <br>
   * It is a legal case if return result < count. Caller should handle this!
   * @param data - buffer to read data to.
   * @param count - size of the buffer.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes read.
   */
  v_io_size read(void *data, v_buff_size count, async::Action& action) override;

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
   * Get stream context.
   * @return
   */
  Context& getInputStreamContext() override;

  /**
   * Close file.
   */
  void close();

  FileInputStream& operator=(FileInputStream&& other);

};

/**
 * Wrapper over `std::FILE`.
 */
class FileOutputStream : public OutputStream {
public:
  static oatpp::data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
private:
  std::FILE* m_file;
  bool m_ownsFile;
  IOMode m_ioMode;
private:
  std::shared_ptr<void> m_capturedData;
public:

  FileOutputStream(const FileOutputStream&) = delete;
  FileOutputStream &operator=(const FileOutputStream&) = delete;

  /**
   * Move constructor.
   * @param other
   */
  FileOutputStream(FileOutputStream&& other);

  /**
   * Constructor.
   * @param file - file.
   * @param ownsFile - if `true` then call close on `FileInputStream` destruction.
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  FileOutputStream(std::FILE* file, bool ownsFile, const std::shared_ptr<void>& captureData = nullptr);

  /**
   * Constructor.
   * @param filename - name of the file.
   * @param mode - ("wb" - create new/override, "ab" - create new/append).
   * @param captureData - capture auxiliary data to not get deleted until it's done with the stream.
   */
  FileOutputStream(const char* filename, const char* mode = "wb", const std::shared_ptr<void>& captureData = nullptr);

  /**
   * Virtual destructor.
   */
  ~FileOutputStream();

  /**
   * Get file.
   * @return
   */
  std::FILE* getFile();

  /**
   * Write data to stream up to count bytes, and return number of bytes actually written. <br>
   * It is a legal case if return result < count. Caller should handle this!
   * @param data - data to write.
   * @param count - number of bytes to write.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written. &id:oatpp::v_io_size;.
   */
  v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

  /**
   * Set stream I/O mode.
   * @throws
   */
  void setOutputStreamIOMode(IOMode ioMode) override;

  /**
   * Get stream I/O mode.
   * @return
   */
  IOMode getOutputStreamIOMode() override;

  /**
   * Get stream context.
   * @return
   */
  Context& getOutputStreamContext() override;

  /**
   * Close file.
   */
  void close();

  FileOutputStream& operator=(FileOutputStream&& other);

};

}}}

#endif // oatpp_data_stream_FileStream_hpp
