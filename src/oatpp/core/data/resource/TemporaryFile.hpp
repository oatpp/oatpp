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

#ifndef oatpp_data_resource_TemporaryFile_hpp
#define oatpp_data_resource_TemporaryFile_hpp

#include "./Resource.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace data { namespace resource {

/**
 * Temporary file - the file which gets deleted when the destructor is called
 * (more precisely when all copies of the same `TemporaryFile` object deleted). <br>
 * The `TemporaryFile` object internally stores a `shared_ptr` to a file handle.
 * When file handle deleted it also deletes the underlying file. <br>
 * Thus it's safe to copy `TemporaryFile` object and you may treat `TemporaryFile` object
 * as a shared_ptr to a temporary file. <br>
 * @extends - &id:oatpp::data::Resource;.
 */
class TemporaryFile : public Resource {
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
  static oatpp::String constructRandomFilename(const oatpp::String &dir, v_int32 randomWordSizeBytes, const oatpp::String &extension);
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
   * Constructor. <br>
   * Create temporary file with a random name and specified extension in the `tmpDirectory`. <br>
   * The actual file will be created only after first write to that file. <br>
   * Example of the generated random file name: `f7c6ecd44024ff31.txt`.
   * @param tmpDirectory - directory where to create a temporary file.
   * @param randomWordSizeBytes - number of random bytes to generate file name.
   * @param extension - extension of the temporary file, e.g. txt or .txt
   */
  TemporaryFile(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes, const oatpp::String& extension);

  /**
   * Open output stream to a temporary file. <br>
   * *Note: stream also captures file-handle. The temporary file won't be deleted until the stream is deleted.*
   * @return - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   */
  std::shared_ptr<data::stream::OutputStream> openOutputStream() override;

  /**
   * Open input stream to a temporary file. <br>
   * *Note: stream also captures file-handle. The temporary file won't be deleted until the stream is deleted.*
   * @return - `std::shared_ptr` &id:oatpp::data::stream::InputStream;.
   */
  std::shared_ptr<data::stream::InputStream> openInputStream() override;

  /**
   * Not applicable.
   * @return - always returns `nullptr`.
   */
  oatpp::String getInMemoryData() override;

  /**
   * Not applicable.
   * @return - always returns `-1`.
   */
  v_int64 getKnownSize() override;

  /**
   * Get location where temporary data is stored.
   * @return - `&id:oatpp::String;`.
   */
  oatpp::String getLocation() override;

  /**
   * Move payload to a different file. <br>
   * @param fullFileName - full-file-name. Note: all the parent folders must exist.
   * @return - `true` - file was successfully moved, `false` - otherwise.
   */
  bool moveFile(const oatpp::String& fullFileName);

};

}}}

#endif //oatpp_data_resource_TemporaryFile_hpp
