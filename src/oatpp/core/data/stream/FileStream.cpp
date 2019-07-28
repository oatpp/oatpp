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

namespace oatpp { namespace data{ namespace stream {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FileInputStream

FileInputStream::FileInputStream(std::FILE* file, bool ownsFile)
  : m_file(file)
  , m_ownsFile(ownsFile)
  , m_ioMode(IOMode::NON_BLOCKING)
{}

FileInputStream::FileInputStream(const char* filename)
  : FileInputStream(std::fopen(filename, "rb"), true)
{
  if(!m_file) {
    OATPP_LOGE("[oatpp::data::stream::FileInputStream::FileInputStream(filename)]", "Error. Can't open file '%s'.", filename);
    throw std::runtime_error("[oatpp::data::stream::FileInputStream::FileInputStream(filename)]: Error. Can't open file.");
  }
}

FileInputStream::~FileInputStream() {
  if(m_ownsFile) {
    std::fclose(m_file);
  }
}

std::FILE* FileInputStream::getFile() {
  return m_file;
}

data::v_io_size FileInputStream::read(void *data, data::v_io_size count) {
  return std::fread(data, 1, count, m_file);
}

oatpp::async::Action FileInputStream::suggestInputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  OATPP_LOGE("[oatpp::data::stream::FileInputStream::suggestInputStreamAction()]", "Error. ioResult=%d", ioResult);

  const char* message =
    "Error. FileInputStream::suggestOutputStreamAction() method is called with (ioResult <= 0).\n"
    "Conceptual error.";

  throw std::runtime_error(message);

}

void FileInputStream::setInputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode FileInputStream::getInputStreamIOMode() {
  return m_ioMode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FileOutputStream

FileOutputStream::FileOutputStream(std::FILE* file, bool ownsFile)
  : m_file(file)
  , m_ownsFile(ownsFile)
  , m_ioMode(IOMode::NON_BLOCKING)
{}

FileOutputStream::FileOutputStream(const char* filename, const char* mode)
  : FileOutputStream(std::fopen(filename, mode), true)
{
  if(!m_file) {
    OATPP_LOGE("[oatpp::data::stream::FileOutputStream::FileOutputStream(filename, mode)]", "Error. Can't open file '%s'.", filename);
    throw std::runtime_error("[oatpp::data::stream::FileOutputStream::FileOutputStream(filename, mode)]: Error. Can't open file.");
  }
}

FileOutputStream::~FileOutputStream() {
  if(m_ownsFile) {
    std::fclose(m_file);
  }
}

std::FILE* FileOutputStream::getFile() {
  return m_file;
}

data::v_io_size FileOutputStream::write(const void *data, data::v_io_size count) {
  return std::fwrite(data, 1, count, m_file);
}


oatpp::async::Action FileOutputStream::suggestOutputStreamAction(data::v_io_size ioResult) {

  if(ioResult > 0) {
    return oatpp::async::Action::createActionByType(oatpp::async::Action::TYPE_REPEAT);
  }

  OATPP_LOGE("[oatpp::data::stream::FileOutputStream::suggestInputStreamAction()]", "Error. ioResult=%d", ioResult);

  const char* message =
    "Error. FileOutputStream::suggestOutputStreamAction() method is called with (ioResult <= 0).\n"
    "Conceptual error.";

  throw std::runtime_error(message);

}

void FileOutputStream::setOutputStreamIOMode(IOMode ioMode) {
  m_ioMode = ioMode;
}

IOMode FileOutputStream::getOutputStreamIOMode() {
  return m_ioMode;
}

}}}
