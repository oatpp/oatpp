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

#include "TemporaryFile.hpp"

#include "./File.hpp"

#include "oatpp/data/stream/FileStream.hpp"
#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/encoding/Hex.hpp"
#include "oatpp/utils/Random.hpp"

namespace oatpp { namespace data { namespace resource {

TemporaryFile::FileHandle::~FileHandle() {
  if(fileName) {
    std::remove(fileName->c_str());
  }
}

oatpp::String TemporaryFile::constructRandomFilename(const oatpp::String &dir, v_int32 randomWordSizeBytes, const oatpp::String &extension) {

  std::unique_ptr<v_char8[]> buff(new v_char8[static_cast<unsigned long>(randomWordSizeBytes)]);
  utils::Random::randomBytes(buff.get(), randomWordSizeBytes);
  data::stream::BufferOutputStream s(randomWordSizeBytes * 2 + 4);
  encoding::Hex::encode(&s, buff.get(), randomWordSizeBytes, encoding::Hex::ALPHABET_LOWER);
  if (extension->at(0) != '.') {
    s << ".";
  }
  s << extension;

  return File::concatDirAndName(dir, s.toString());

}

TemporaryFile::TemporaryFile(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes)
  : m_handle(std::make_shared<FileHandle>(constructRandomFilename(tmpDirectory, randomWordSizeBytes, "tmp")))
{}

TemporaryFile::TemporaryFile(const oatpp::String& tmpDirectory, const oatpp::String& tmpFileName)
  : m_handle(std::make_shared<FileHandle>(File::concatDirAndName(tmpDirectory, tmpFileName)))
{}

TemporaryFile::TemporaryFile(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes, const oatpp::String& extension)
  : m_handle(std::make_shared<FileHandle>(constructRandomFilename(tmpDirectory, randomWordSizeBytes, extension)))
{}

std::shared_ptr<data::stream::OutputStream> TemporaryFile::openOutputStream() {
  if(m_handle) {
    return std::make_shared<data::stream::FileOutputStream>(m_handle->fileName->c_str(), "wb", m_handle);
  }
  throw std::runtime_error("[oatpp::data::resource::TemporaryFile::openOutputStream()]: Error. FileHandle is NOT initialized.");
}

std::shared_ptr<data::stream::InputStream> TemporaryFile::openInputStream() {
  if(m_handle) {
    return std::make_shared<data::stream::FileInputStream>(m_handle->fileName->c_str(), m_handle);
  }
  throw std::runtime_error("[oatpp::data::resource::TemporaryFile::openInputStream()]: Error. FileHandle is NOT initialized.");
}

oatpp::String TemporaryFile::getInMemoryData() {
  return nullptr;
}

v_int64 TemporaryFile::getKnownSize() {
  return -1;
}

oatpp::String TemporaryFile::getLocation() {
  if(m_handle) {
    return m_handle->fileName;
  }
  return nullptr;
}

bool TemporaryFile::moveFile(const oatpp::String& fullFileName) {
  if(m_handle) {
    return std::rename(m_handle->fileName->c_str(), fullFileName->c_str()) == 0;
  }
  return false;
}

}}}
