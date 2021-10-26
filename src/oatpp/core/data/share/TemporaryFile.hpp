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

#ifndef oatpp_data_share_TemporaryFile_hpp
#define oatpp_data_share_TemporaryFile_hpp

#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace data { namespace share {

/**
 * Temporary file - the file which gets deleted when the destructor is called
 * (more precisely when all copies of the same `TemporaryFile` object deleted). <br>
 * The `TemporaryFile` object internally stores a `shared_ptr` to a file handle.
 * When file handle deleted it also deletes the underlying file. <br>
 * Thus it's safe to copy `TemporaryFile` object and you may treat `TemporaryFile` object
 * as a shared_ptr to a temporary file.
 */
class TemporaryFile {
private:

  /*
   * Shared handle.
   * File is deleted on handle destroy.
   */
  struct FileHandle {

    oatpp::String fileName;

    FileHandle(const oatpp::String& fullFileName)
      : fileName(fullFileName)
    {}

    ~FileHandle();
  };

private:
  static oatpp::String concatDirAndName(const oatpp::String& dir, const oatpp::String& filename);
  static oatpp::String constructRandomFilename(const oatpp::String& dir, v_int32 randomWordSizeBytes);
private:
  std::shared_ptr<FileHandle> m_handle;
public:

  /**
   * Default constructor.
   */
  TemporaryFile() = default;

  /**
   * Constructor. <br>
   * Create temporary file with a random name in the `tmpDirectory`. <br>
   * The actual file will be created only after first write to that file. <br>
   * Example of the generated random file name: `f7c6ecd44024ff31.tmp`.
   * @param tmpDirectory - directory where to create a temporary file.
   * @param randomWordSizeBytes - number of random bytes to generate file name.
   */
  TemporaryFile(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes = 8);

  /**
   * Constructor.<br>
   * Create temporary file with the `tmpFileName` name in the `tmpDirectory`. <br>
   * @param tmpDirectory - directory where to create a temporary file.
   * @param tmpFileName - predefined name for the temporary file.
   */
  TemporaryFile(const oatpp::String& tmpDirectory, const oatpp::String& tmpFileName);

  /**
   * Get full name of a temporary file.
   * @return
   */
  oatpp::String getFullFileName();

  /**
   * Open output stream to a temporary file. <br>
   * *Note: stream also captures file-handle. The temporary file won't be deleted until the stream is deleted.*
   * @return - &id:oatpp::data::stream::FileOutputStream;.
   */
  data::stream::FileOutputStream openOutputStream();

  /**
   * Open input stream to a temporary file.
   * *Note: stream also captures file-handle. The temporary file won't be deleted until the stream is deleted.*
   * @return - &id:oatpp::data::stream::FileInputStream;.
   */
  data::stream::FileInputStream openInputStream();

  /**
   * Open output stream to a temporary file. <br>
   * *Note: stream also captures file-handle. The temporary file won't be deleted until the stream is deleted.*
   * @return - `std::shared_ptr` to &id:oatpp::data::stream::FileOutputStream;.
   */
  std::shared_ptr<data::stream::FileOutputStream> openOutputStreamShared();

  /**
   * Open input stream to a temporary file.
   * *Note: stream also captures file-handle. The temporary file won't be deleted until the stream is deleted.*
   * @return - `std::shared_ptr` &id:oatpp::data::stream::FileInputStream;.
   */
  std::shared_ptr<data::stream::FileInputStream> openInputStreamShared();

  /**
   * Move payload to a different file. <br>
   * @param fullFileName - full-file-name. Note: all the parent folders must exist.
   * @return - `true` - file was successfully moved, `false` - otherwise.
   */
  bool moveFile(const oatpp::String& fullFileName);

};

}}}

#endif //oatpp_data_share_TemporaryFile_hpp
