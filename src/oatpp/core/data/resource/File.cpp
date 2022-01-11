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

#include "File.hpp"

#include "oatpp/core/data/stream/FileStream.hpp"

namespace oatpp { namespace data { namespace resource {

oatpp::String File::concatDirAndName(const oatpp::String& dir, const oatpp::String& filename) {
  if(dir && dir->size() > 0) {
    auto lastChar = dir->data()[dir->size() - 1];
    if(lastChar != '/' && lastChar != '\\') {
      return dir + "/" + filename;
    }
    return dir + filename;
  }
  return filename;
}

File::File(const oatpp::String& fullFileName)
  : m_handle(std::make_shared<FileHandle>(fullFileName))
{}

File::File(const oatpp::String& tmpDirectory, const oatpp::String& tmpFileName)
  : m_handle(std::make_shared<FileHandle>(concatDirAndName(tmpDirectory, tmpFileName)))
{}

std::shared_ptr<data::stream::OutputStream> File::openOutputStream() {
  if(m_handle) {
    return std::make_shared<data::stream::FileOutputStream>(m_handle->fileName->c_str(), "wb", m_handle);
  }
  throw std::runtime_error("[oatpp::data::resource::File::openOutputStream()]: Error. FileHandle is NOT initialized.");
}

std::shared_ptr<data::stream::InputStream> File::openInputStream() {
  if(m_handle) {
    return std::make_shared<data::stream::FileInputStream>(m_handle->fileName->c_str(), m_handle);
  }
  throw std::runtime_error("[oatpp::data::resource::File::openInputStream()]: Error. FileHandle is NOT initialized.");
}

oatpp::String File::getInMemoryData() {
  return nullptr;
}

v_int64 File::getKnownSize() {
  return -1;
}

oatpp::String File::getLocation() {
  if(m_handle) {
    return m_handle->fileName;
  }
  return nullptr;
}

}}}
