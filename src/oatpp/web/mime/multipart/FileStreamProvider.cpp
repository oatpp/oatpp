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

#include "FileStreamProvider.hpp"

#include "oatpp/core/data/stream/FileStream.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

FileStreamProvider::FileStreamProvider(const oatpp::String& filename)
  : m_filename(filename)
{}

std::shared_ptr<FileStreamProvider::OutputStream> FileStreamProvider::getOutputStream(const std::shared_ptr<Part>& part) {
  (void)part;
  return std::make_shared<data::stream::FileOutputStream>(m_filename->c_str());
}

std::shared_ptr<FileStreamProvider::InputStream> FileStreamProvider::getInputStream(const std::shared_ptr<Part>& part) {
  (void)part;
  return std::make_shared<data::stream::FileInputStream>(m_filename->c_str());
}


AsyncFileStreamProvider::AsyncFileStreamProvider(const oatpp::String& filename)
  : m_filename(filename)
{}

async::CoroutineStarter AsyncFileStreamProvider::getOutputStreamAsync(const std::shared_ptr<Part>& part,
                                                                      std::shared_ptr<data::stream::OutputStream>& stream)
{
  (void)part;
  stream = std::make_shared<data::stream::FileOutputStream>(m_filename->c_str());
  return nullptr;
}


async::CoroutineStarter AsyncFileStreamProvider::getInputStreamAsync(const std::shared_ptr<Part>& part,
                                                                     std::shared_ptr<data::stream::InputStream>& stream)
{
  (void)part;
  stream = std::make_shared<data::stream::FileInputStream>(m_filename->c_str());
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions

std::shared_ptr<PartReader> createFilePartReader(const oatpp::String& filename, v_io_size maxDataSize) {
  auto provider = std::make_shared<FileStreamProvider>(filename);
  auto reader = std::make_shared<StreamPartReader>(provider, maxDataSize);
  return reader;
}

std::shared_ptr<AsyncPartReader> createAsyncFilePartReader(const oatpp::String& filename, v_io_size maxDataSize) {
  auto provider = std::make_shared<AsyncFileStreamProvider>(filename);
  auto reader = std::make_shared<AsyncStreamPartReader>(provider, maxDataSize);
  return reader;
}

}}}}
