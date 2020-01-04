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

#include "ProcessorTest.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/data/buffer/Processor.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace buffer {

namespace {

typedef oatpp::data::buffer::Processor Processor;

class BaseProcessor : public oatpp::data::buffer::Processor {
private:
  v_int32 m_bufferSize;
  oatpp::data::stream::BufferOutputStream m_buffer;
public:

  BaseProcessor(v_int32 bufferSize)
    : m_bufferSize(bufferSize)
  {}

  oatpp::data::v_io_size suggestInputStreamReadSize() override {
    return m_bufferSize;
  }

  virtual void process(p_char8 data, v_buff_size size) = 0;

  v_int32 iterate(oatpp::data::stream::InlineReadData& dataIn, oatpp::data::stream::InlineReadData& dataOut) override {

    if(dataOut.bytesLeft > 0) {
      return Error::FLUSH_DATA_OUT;
    }

    if(dataIn.currBufferPtr != nullptr) {

      while (dataIn.bytesLeft > 0 && m_buffer.getCurrentPosition() < m_bufferSize) {
        m_buffer.writeSimple(dataIn.currBufferPtr, 1);
        dataIn.inc(1);
      }

      if(m_buffer.getCurrentPosition() < m_bufferSize) {
        return Error::PROVIDE_DATA_IN;
      }

      dataOut.set(m_buffer.getData(), m_buffer.getCurrentPosition());
      process(m_buffer.getData(), m_buffer.getCurrentPosition());
      m_buffer.setCurrentPosition(0);
      return Error::FLUSH_DATA_OUT;

    } else if(m_buffer.getCurrentPosition() > 0) {
      dataOut.set(m_buffer.getData(), m_buffer.getCurrentPosition());
      process(m_buffer.getData(), m_buffer.getCurrentPosition());
      m_buffer.setCurrentPosition(0);
      return Error::FLUSH_DATA_OUT;
    }

    dataOut.set(nullptr, 0);
    return Error::FINISHED;

  }

};

class ProcessorToUpper : public BaseProcessor {
public:

  ProcessorToUpper(v_int32 bufferSize) : BaseProcessor(bufferSize) {}

  void process(p_char8 data, v_buff_size size) override {
    oatpp::base::StrBuffer::upperCase(data, size);
  }

};

class ProcessorToLower : public BaseProcessor {
public:

  ProcessorToLower(v_int32 bufferSize) : BaseProcessor(bufferSize) {}

  void process(p_char8 data, v_buff_size size) override {
    oatpp::base::StrBuffer::lowerCase(data, size);
  }

};

class ProcessorChangeLetter : public BaseProcessor {
private:
  v_char8 m_fromChar;
  v_char8 m_toChar;
public:

  ProcessorChangeLetter(v_char8 fromChar, v_char8 toChar, v_int32 bufferSize)
    : BaseProcessor(bufferSize)
    , m_fromChar(fromChar)
    , m_toChar(toChar)
  {}

  void process(p_char8 data, v_buff_size size) override {
    for(v_buff_size i = 0; i < size; i ++) {

      if(data[i] == m_fromChar) {
        data[i] = m_toChar;
      }

    }
  }

};

void process(oatpp::data::stream::ReadCallback* readCallback,
             oatpp::data::stream::WriteCallback* writeCallback,
             oatpp::data::buffer::Processor* processor,
             const v_buff_size bufferSize)
{

  v_char8 buffer[bufferSize];

  oatpp::data::stream::InlineReadData inData;
  oatpp::data::stream::InlineReadData outData;

  v_int32 procRes = Processor::Error::PROVIDE_DATA_IN;

  while(true) {

    if(procRes == Processor::Error::PROVIDE_DATA_IN) {

      v_buff_size desiredToRead = processor->suggestInputStreamReadSize();
      if (desiredToRead > bufferSize) {
        desiredToRead = bufferSize;
      }

      auto res = readCallback->readSimple(buffer, desiredToRead);

      if (res > 0) {
        inData.set(buffer, res);
      } else {
        inData.set(nullptr, 0);
      }

    }

    procRes = processor->iterate(inData, outData);

    switch(procRes) {

      case Processor::Error::PROVIDE_DATA_IN: {
        continue;
      }

      case Processor::Error::FLUSH_DATA_OUT: {
        writeCallback->writeSimple(outData.currBufferPtr, outData.bytesLeft);
        outData.setEof();
        break;
      }

      case Processor::Error::FINISHED:
        return;

      default:
        throw std::runtime_error("Unknown buffer processor error.");

    }

  }

}

}

void ProcessorTest::onRun() {

  oatpp::String data = ">aaaaa0123456789bbbbb<";

  {
    oatpp::data::stream::BufferInputStream inStream(data);
    oatpp::data::stream::BufferOutputStream outStream;

    oatpp::data::buffer::ProcessingPipeline processor({
      oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorToUpper>(1)),
      oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorChangeLetter>('A', 'C', 10)),
      oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorToLower>(3))
    });

    process(&inStream, &outStream, &processor, 5);

    auto str = outStream.toString();
    OATPP_LOGD(TAG, "result='%s'", str->getData());

  }


}

}}}}}
