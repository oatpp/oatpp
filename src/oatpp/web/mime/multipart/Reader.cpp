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

void PartsParser::onPartData(const char* data, v_buff_size size) {
  if(size > 0) {
    if(m_currReader) {
      m_currReader->onPartData(m_currPart, data, size);
    }
  } else {
    if(m_currReader) {
      m_currReader->onPartData(m_currPart, nullptr, 0);
    }
    m_multipart->writeNextPartSimple(m_currPart);
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

async::CoroutineStarter AsyncPartsParser::onPartDone(const std::shared_ptr<Part>& part) {

  class PutPartCoroutine : public async::Coroutine<PutPartCoroutine> {
  private:
    Multipart* m_multipart;
    std::shared_ptr<AsyncPartReader> m_currReader;
    std::shared_ptr<Part> m_part;
  public:

    PutPartCoroutine(Multipart* multipart,
                     const std::shared_ptr<AsyncPartReader>& currReader,
                     const std::shared_ptr<Part>& part)
      : m_multipart(multipart)
      , m_currReader(currReader)
      , m_part(part)
    {}

    Action act() override {
      return m_currReader->onPartDataAsync(m_part, nullptr, 0).next(yieldTo(&PutPartCoroutine::putPart));
    }

    Action putPart() {
      async::Action action;
      m_multipart->writeNextPart(m_part, action);
      if(!action.isNone()) {
        return action;
      }
      return finish();
    }

  };

  return PutPartCoroutine::start(m_multipart, m_currReader, part);

}

async::CoroutineStarter AsyncPartsParser::onPartDataAsync(const char* data, v_buff_size size) {
  if(size > 0) {
    if(m_currReader) {
      return m_currReader->onPartDataAsync(m_currPart, data, size);
    }
  } else {
    return onPartDone(m_currPart);
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

v_io_size Reader::write(const void *data, v_buff_size count, async::Action& action) {
  data::buffer::InlineWriteData inlineData(data, count);
  m_parser.parseNext(inlineData, action);
  return count - inlineData.bytesLeft;
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
{}

v_io_size AsyncReader::write(const void *data, v_buff_size count, async::Action& action) {

  data::buffer::InlineWriteData inlineData(data, count);
  while(inlineData.bytesLeft > 0 && !m_parser.finished() && action.isNone()) {
    m_parser.parseNext(inlineData, action);
  }

  return count - inlineData.bytesLeft;

}

void AsyncReader::setPartReader(const oatpp::String& partName, const std::shared_ptr<AsyncPartReader>& reader) {
  m_partsParser->m_readers[partName] = reader;
}

void AsyncReader::setDefaultPartReader(const std::shared_ptr<AsyncPartReader>& reader) {
  m_partsParser->m_defaultReader = reader;
}

}}}}
