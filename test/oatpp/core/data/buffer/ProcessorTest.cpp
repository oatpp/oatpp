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

#include "oatpp/core/async/Executor.hpp"

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

  oatpp::v_io_size suggestInputStreamReadSize() override {
    return m_bufferSize;
  }

  virtual void process(p_char8 data, v_buff_size size) = 0;

  v_int32 iterate(oatpp::data::buffer::InlineReadData& dataIn, oatpp::data::buffer::InlineReadData& dataOut) override {

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
    utils::String::upperCase_ASCII(data, size);
  }

};

class ProcessorToLower : public BaseProcessor {
public:

  ProcessorToLower(v_int32 bufferSize) : BaseProcessor(bufferSize) {}

  void process(p_char8 data, v_buff_size size) override {
    utils::String::lowerCase_ASCII(data, size);
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

class TestCoroutine : public oatpp::async::Coroutine<TestCoroutine> {
public:
  static std::atomic<v_int64> COUNTER;
private:
  oatpp::String m_data;
  oatpp::String m_etalon;
  std::shared_ptr<oatpp::data::stream::ReadCallback> m_readCallback;
  std::shared_ptr<oatpp::data::stream::BufferOutputStream> m_writeCallback;
  std::shared_ptr<oatpp::data::buffer::Processor> m_processor;
  std::shared_ptr<oatpp::data::buffer::IOBuffer> m_buffer;
public:

  TestCoroutine(const oatpp::String &mData,
                const oatpp::String &mEtalon,
                const std::shared_ptr<oatpp::data::stream::ReadCallback> &mReadCallback,
                const std::shared_ptr<oatpp::data::stream::BufferOutputStream> &mWriteCallback,
                const std::shared_ptr<Processor> &mProcessor,
                const std::shared_ptr<oatpp::data::buffer::IOBuffer> &mBuffer)
    : m_data(mData)
    , m_etalon(mEtalon)
    , m_readCallback(mReadCallback)
    , m_writeCallback(mWriteCallback)
    , m_processor(mProcessor)
    , m_buffer(mBuffer)
  {
    COUNTER ++;
  }

  ~TestCoroutine() {
    COUNTER --;
  }

  Action act() {
    return oatpp::data::stream::transferAsync(m_readCallback, m_writeCallback, 0, m_buffer, m_processor)
      .next(yieldTo(&TestCoroutine::compare));
  }

  Action compare() {
    auto result = m_writeCallback->toString();
    OATPP_ASSERT(m_etalon == result);
    return finish();
  }

};

std::atomic<v_int64> TestCoroutine::COUNTER(0);

oatpp::String runTestCase(const oatpp::String& data, v_int32 p1N, v_int32 p2N, v_int32 p3N, v_int32 bufferSize) {

  oatpp::data::stream::BufferInputStream inStream(data);
  oatpp::data::stream::BufferOutputStream outStream;

  oatpp::data::buffer::ProcessingPipeline processor({
    oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorToUpper>(p1N)),
    oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorChangeLetter>('A', '-', p2N)),
    oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorToLower>(p3N)),
  });

  std::unique_ptr<v_char8[]> buffer(new v_char8[bufferSize]);
  oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer.get(), bufferSize, &processor);

  return outStream.toString();

}

}

void ProcessorTest::onRun() {

  oatpp::String data = ">aaaaa0123456789bbbbb<";
  oatpp::String etalon = ">-----0123456789bbbbb<";

  for(v_int32 p1N = 1; p1N < 11; p1N ++) {
    for(v_int32 p2N = 1; p2N < 11; p2N ++) {
      for (v_int32 p3N = 1; p3N < 11; p3N++) {
        for (v_int32 buffSize = 1; buffSize < 11; buffSize++) {

          auto result = runTestCase(data, p1N, p2N, p3N, buffSize);

          if (result != etalon) {
            OATPP_LOGD(TAG, "error[%d, %d, %d, b=%d] result='%s'", p1N, p2N, p3N, buffSize, result->data());
          }
          OATPP_ASSERT(result == etalon);

        }
      }
    }
  }

  {

    v_char8 buffer[1024];

    for(v_int32 i = 1; i < 30; i++) {

      oatpp::data::stream::BufferInputStream inStream(data);
      oatpp::data::stream::BufferOutputStream outStream;

      auto progress = oatpp::data::stream::transfer(&inStream, &outStream, 0, buffer, i);

      auto result = outStream.toString();

      OATPP_ASSERT(result == data);
      OATPP_ASSERT(inStream.getCurrentPosition() == progress);

    }

  }

  oatpp::async::Executor executor;

  {

    for(v_int32 p1N = 1; p1N < 11; p1N ++) {
      for(v_int32 p2N = 1; p2N < 11; p2N ++) {
        for (v_int32 p3N = 1; p3N < 11; p3N++) {

          auto inStream = std::make_shared<oatpp::data::stream::BufferInputStream>(data);
          auto outStream = std::make_shared<oatpp::data::stream::BufferOutputStream>();

          auto processor = std::shared_ptr<oatpp::data::buffer::ProcessingPipeline>(new oatpp::data::buffer::ProcessingPipeline({
            oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorToUpper>(p1N)),
            oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorChangeLetter>('A', '-', p2N)),
            oatpp::base::ObjectHandle<Processor>(std::make_shared<ProcessorToLower>(p3N)),
          }));

          auto buffer = std::make_shared<oatpp::data::buffer::IOBuffer>();

          executor.execute<TestCoroutine>(data, etalon, inStream, outStream, processor, buffer);

        }
      }
    }

    while(TestCoroutine::COUNTER > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  }

  executor.waitTasksFinished();
  executor.stop();
  executor.join();

}

}}}}}
