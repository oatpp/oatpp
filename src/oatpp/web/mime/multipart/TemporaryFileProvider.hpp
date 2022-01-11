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

#ifndef oatpp_web_mime_multipart_TemporaryFileProvider_hpp
#define oatpp_web_mime_multipart_TemporaryFileProvider_hpp

#include "PartReader.hpp"
#include "Reader.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

class TemporaryFileProvider : public PartReaderResourceProvider {
private:
  oatpp::String m_tmpDirectory;
  v_int32 m_randomWordSizeBytes;
public:

  TemporaryFileProvider(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes = 8);

  std::shared_ptr<data::resource::Resource> getResource(const std::shared_ptr<Part>& part) override;

  async::CoroutineStarter getResourceAsync(const std::shared_ptr<Part>& part,
                                           std::shared_ptr<data::resource::Resource>& resource) override;

};

/**
 * Create part reader to a temporary file.
 * @param tmpDirectory - directory for temporary files.
 * @param randomWordSizeBytes - number of random bytes to generate file name.
 * @param maxDataSize - max size of the received data. put `-1` for no-limit.
 * @return - `std::shared_ptr` to &id:oatpp::web::mime::multipart::PartReader;.
 */
std::shared_ptr<PartReader> createTemporaryFilePartReader(const oatpp::String& tmpDirectory,
                                                          v_int32 randomWordSizeBytes = 8,
                                                          v_io_size maxDataSize = -1);

/**
 * Create async part reader to a temporary file.
 * @param tmpDirectory - directory for temporary files.
 * @param randomWordSizeBytes - number of random bytes to generate file name.
 * @param maxDataSize - max size of the received data. put `-1` for no-limit.
 * @return - `std::shared_ptr` to &id:oatpp::web::mime::multipart::AsyncPartReader;.
 */
std::shared_ptr<AsyncPartReader> createAsyncTemporaryFilePartReader(const oatpp::String& tmpDirectory,
                                                                    v_int32 randomWordSizeBytes = 8,
                                                                    v_io_size maxDataSize = -1);

}}}}

#endif //oatpp_web_mime_multipart_TemporaryFileProvider_hpp
