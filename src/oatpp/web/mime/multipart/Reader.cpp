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

#include "Reader.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PartsParser

PartsParser::PartsParser(Multipart* multipart)
  : m_multipart(multipart)
{}

PartsParser::PartsParser(Multipart* multipart, const PartReadersMap& readersMap)
  : m_readers(readersMap)
  , m_multipart(multipart)
{}

void PartsParser::onPartHeaders(const Headers& partHeaders) {

  m_currPart = std::make_shared<Part>(partHeaders);

  if(m_currPart->getName()) {
    auto it = m_readers.find(m_currPart->getName());
    if(it != m_readers.end()) {
      m_currReader = it->second;
    } else {
      m_currReader = m_defaultReader;
    }
  }

  if(m_currReader) {
    m_currReader->onNewPart(m_currPart);
  }

}

void PartsParser::onPartData(p_char8 data, v_buff_size size) {
  if(size > 0) {
    if(m_currReader) {
      m_currReader->onPartData(m_currPart, data, size);
    }
  } else {
    m_multipart->addPart(m_currPart);
    if(m_currReader) {
      m_currReader->onPartData(m_currPart, data, size);
    }
  }
}

void PartsParser::setPartReader(const oatpp::String& partName, const std::shared_ptr<PartReader>& reader) {
  m_readers[partName] = reader;
}

void PartsParser::setDefaultPartReader(const std::shared_ptr<PartReader>& reader) {
  m_defaultReader = reader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncPartsParser

AsyncPartsParser::AsyncPartsParser(Multipart* multipart)
  : m_multipart(multipart)
{}

AsyncPartsParser::AsyncPartsParser(Multipart* multipart, const AsyncPartReadersMap& readersMap)
  : m_readers(readersMap)
  , m_multipart(multipart)
{}

async::CoroutineStarter AsyncPartsParser::onPartHeadersAsync(const Headers& partHeaders) {

  m_currPart = std::make_shared<Part>(partHeaders);

  if(m_currPart->getName()) {
    auto it = m_readers.find(m_currPart->getName());
    if(it != m_readers.end()) {
      m_currReader = it->second;
    } else {
      m_currReader = m_defaultReader;
    }
  }

  if(m_currReader) {
    return m_currReader->onNewPartAsync(m_currPart);
  }

  return nullptr;

}

async::CoroutineStarter AsyncPartsParser::onPartDataAsync(p_char8 data, v_buff_size size) {
  if(size > 0) {
    if(m_currReader) {
      return m_currReader->onPartDataAsync(m_currPart, data, size);
    }
  } else {
    m_multipart->addPart(m_currPart);
    if(m_currReader) {
      return m_currReader->onPartDataAsync(m_currPart, data, size);
    }
  }
  return nullptr;
}

void AsyncPartsParser::setPartReader(const oatpp::String& partName, const std::shared_ptr<AsyncPartReader>& reader) {
  m_readers[partName] = reader;
}

void AsyncPartsParser::setDefaultPartReader(const std::shared_ptr<AsyncPartReader>& reader) {
  m_defaultReader = reader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InMemoryReader

Reader::Reader(Multipart* multipart)
  : m_partsParser(std::make_shared<PartsParser>(multipart))
  , m_parser(multipart->getBoundary(), m_partsParser, nullptr)
{}

data::v_io_size Reader::write(const void *data, v_buff_size count) {
  m_parser.parseNext((p_char8) data, count);
  return count;
}

void Reader::setPartReader(const oatpp::String& partName, const std::shared_ptr<PartReader>& reader) {
  m_partsParser->m_readers[partName] = reader;
}

void Reader::setDefaultPartReader(const std::shared_ptr<PartReader>& reader) {
  m_partsParser->m_defaultReader = reader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsyncReader

AsyncReader::AsyncReader(const std::shared_ptr<Multipart>& multipart)
  : m_partsParser(std::make_shared<AsyncPartsParser>(multipart.get()))
  , m_parser(multipart->getBoundary(), nullptr, m_partsParser)
  , m_multipart(multipart)
  , m_lastIOSize(data::IOError::RETRY_WRITE)
{}

data::v_io_size AsyncReader::write(const void *data, v_buff_size count) {

  if(m_parser.finished()) {
    return 0;
  }

  if(m_lastIOSize < 0) {

    m_lastIOSize = count;
    if (m_lastIOSize > m_buffer.getSize()) {
      m_lastIOSize = m_buffer.getSize();
    }

    std::memcpy(m_buffer.getData(), data, m_lastIOSize);
    m_inlineData.set(m_buffer.getData(), m_lastIOSize);
    return data::IOError::SUGGEST_ACTION_WRITE;

  } else if(m_lastIOSize <= count){
    auto result = m_lastIOSize - m_inlineData.bytesLeft;
    m_lastIOSize = data::IOError::RETRY_WRITE;
    return result;
  }

  return data::IOError::BROKEN_PIPE;

}

async::Action AsyncReader::suggestOutputStreamAction(data::v_io_size ioResult) {

  if(m_lastIOSize > 0 && ioResult == data::IOError::SUGGEST_ACTION_WRITE) {
    return m_parser.parseNextAsyncInline(m_inlineData, async::Action::createActionByType(async::Action::TYPE_REPEAT));
  }

  return new async::Error("[oatpp::web::mime::multipart::AsyncReader::suggestOutputStreamAction]: Invalid state.");

}

void AsyncReader::setPartReader(const oatpp::String& partName, const std::shared_ptr<AsyncPartReader>& reader) {
  m_partsParser->m_readers[partName] = reader;
}

void AsyncReader::setDefaultPartReader(const std::shared_ptr<AsyncPartReader>& reader) {
  m_partsParser->m_defaultReader = reader;
}

}}}}
