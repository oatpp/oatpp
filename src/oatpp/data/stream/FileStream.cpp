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

#include "FileStream.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace data{ namespace stream {


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FileInputStream

oatpp::data::stream::DefaultInitializedContext FileInputStream::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_FINITE);

FileInputStream::FileInputStream(FileInputStream&& other)
  : m_file(other.m_file)
  , m_ownsFile(other.m_ownsFile)
  , m_ioMode(other.m_ioMode)
{
  other.m_file = nullptr;
  other.m_ownsFile = false;
}

FileInputStream::FileInputStream(std::FILE* file, bool ownsFile, const std::shared_ptr<void>& captureData)
  : m_file(file)
  , m_ownsFile(ownsFile)
  , m_ioMode(IOMode::ASYNCHRONOUS)
  , m_capturedData(captureData)
{}

FileInputStream::FileInputStream(const char* filename, const std::shared_ptr<void>& captureData)
  : FileInputStream(std::fopen(filename, "rb"), true, captureData)
{
  if(!m_file) {
    OATPP_LOGe("[oatpp::data::stream::FileInputStream::FileInputStream(filename)]", "Error. Can't open file '{}'.", filename)
    throw std::runtime_error("[oatpp::data::stream::FileInputStream::FileInputStream(filename)]: Error. Can't open file.");
  }
}

FileInputStream::~FileInputStream() {
  this->close();
}

std::FILE* FileInputStream::getFile() {
  return m_file;
}

v_io_size FileInputStream::read(void *data, v_buff_size count, async::Action& action) {
  (void) action;
  if(m_file != nullptr) {
    return static_cast<v_io_size>(std::fread(data, 1, static_cast<size_t>(count), m_file));
  }
  return oatpp::IOError::BROKEN_PIPE;
}

void FileInputStream::setInputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode FileInputStream::getInputStreamIOMode() {
  return m_ioMode;
}

Context& FileInputStream::getInputStreamContext() {
  return DEFAULT_CONTEXT;
}

void FileInputStream::close() {
  if(m_ownsFile && m_file) {
    std::fclose(m_file);
  }
}

FileInputStream& FileInputStream::operator=(FileInputStream&& other) {

  if(this != &other) {
    close();
  }

  m_file = other.m_file;
  m_ownsFile = other.m_ownsFile;
  m_ioMode = other.m_ioMode;

  other.m_file = nullptr;
  other.m_ownsFile = false;

  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FileOutputStream

oatpp::data::stream::DefaultInitializedContext FileOutputStream::DEFAULT_CONTEXT(data::stream::StreamType::STREAM_FINITE);

FileOutputStream::FileOutputStream(FileOutputStream&& other)
  : m_file(other.m_file)
  , m_ownsFile(other.m_ownsFile)
  , m_ioMode(other.m_ioMode)
{
  other.m_file = nullptr;
  other.m_ownsFile = false;
}

FileOutputStream::FileOutputStream(std::FILE* file, bool ownsFile, const std::shared_ptr<void>& captureData)
  : m_file(file)
  , m_ownsFile(ownsFile)
  , m_ioMode(IOMode::ASYNCHRONOUS)
  , m_capturedData(captureData)
{}

FileOutputStream::FileOutputStream(const char* filename, const char* mode, const std::shared_ptr<void>& captureData)
  : FileOutputStream(std::fopen(filename, mode), true, captureData)
{
  if(!m_file) {
    OATPP_LOGe("[oatpp::data::stream::FileOutputStream::FileOutputStream(filename, mode)]", "Error. Can't open file '{}'.", filename)
    throw std::runtime_error("[oatpp::data::stream::FileOutputStream::FileOutputStream(filename, mode)]: Error. Can't open file.");
  }
}

FileOutputStream::~FileOutputStream() {
  this->close();
}

std::FILE* FileOutputStream::getFile() {
  return m_file;
}

v_io_size FileOutputStream::write(const void *data, v_buff_size count, async::Action& action) {
  (void) action;
  return static_cast<v_io_size>(std::fwrite(data, 1, static_cast<size_t>(count), m_file));
}

void FileOutputStream::setOutputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode FileOutputStream::getOutputStreamIOMode() {
  return m_ioMode;
}

Context& FileOutputStream::getOutputStreamContext() {
  return DEFAULT_CONTEXT;
}

void FileOutputStream::close() {
  if(m_ownsFile && m_file) {
    std::fclose(m_file);
  }
}

FileOutputStream& FileOutputStream::operator=(FileOutputStream&& other) {

  if(this != &other) {
    close();
  }

  m_file = other.m_file;
  m_ownsFile = other.m_ownsFile;
  m_ioMode = other.m_ioMode;

  other.m_file = nullptr;
  other.m_ownsFile = false;

  return *this;
}

}}}
