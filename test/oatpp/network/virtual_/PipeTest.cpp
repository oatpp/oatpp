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

#include "PipeTest.hpp"

#include "oatpp/network/virtual_/Pipe.hpp"

#include "oatpp/data/stream/BufferStream.hpp"

#include "oatpp-test/Checker.hpp"

#include <iostream>
#include <thread>

namespace oatpp { namespace test { namespace network { namespace virtual_ {
  
namespace {
  
  typedef oatpp::network::virtual_::Pipe Pipe;
  
  const char* DATA_CHUNK = "<0123456789/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ>";
  const v_buff_size CHUNK_SIZE = static_cast<v_buff_size>(std::strlen(DATA_CHUNK));

  class WriterTask : public oatpp::base::Countable {
  private:
    std::shared_ptr<Pipe> m_pipe;
    v_int64 m_chunksToTransfer;
    v_buff_size m_position = 0;
    v_buff_size m_transferedBytes = 0;
  public:
    
    WriterTask(const std::shared_ptr<Pipe>& pipe, v_int64 chunksToTransfer)
      : m_pipe(pipe)
      , m_chunksToTransfer(chunksToTransfer)
    {}
    
    void run() {
      while (m_transferedBytes < CHUNK_SIZE * m_chunksToTransfer) {
        auto res = m_pipe->getWriter()->writeSimple(&DATA_CHUNK[m_position], CHUNK_SIZE - m_position);
        if(res > 0) {
          m_transferedBytes += res;
          m_position += res;
          if(m_position == CHUNK_SIZE) {
            m_position = 0;
          }
        }
      }
      OATPP_LOGv("WriterTask", "sent {} bytes", m_transferedBytes)
    }
    
  };
  
  class ReaderTask : public oatpp::base::Countable {
  private:
    std::shared_ptr<oatpp::data::stream::BufferOutputStream> m_buffer;
    std::shared_ptr<Pipe> m_pipe;
    v_int64 m_chunksToTransfer;
  public:
    
    ReaderTask(const std::shared_ptr<oatpp::data::stream::BufferOutputStream> &buffer,
               const std::shared_ptr<Pipe>& pipe,
               v_int64 chunksToTransfer)
      : m_buffer(buffer)
      , m_pipe(pipe)
      , m_chunksToTransfer(chunksToTransfer)
    {}
    
    void run() {
      v_char8 readBuffer[256];
      while (m_buffer->getCurrentPosition() < CHUNK_SIZE * m_chunksToTransfer) {
        auto res = m_pipe->getReader()->readSimple(readBuffer, 256);
        if(res > 0) {
          m_buffer->writeSimple(readBuffer, res);
        }
      }
      OATPP_LOGv("ReaderTask", "sent {} bytes", m_buffer->getCurrentPosition())
    }
    
  };
  
  void runTransfer(const std::shared_ptr<Pipe>& pipe, v_int64 chunksToTransfer, bool writeNonBlock, bool readerNonBlock) {
    
    OATPP_LOGv("transfer", "writer-nb: {}, reader-nb: {}", writeNonBlock, readerNonBlock)
    
    auto buffer = std::make_shared<oatpp::data::stream::BufferOutputStream>();
    
    {
      
      oatpp::test::PerformanceChecker timer("timer");

      std::thread writerThread(&WriterTask::run, WriterTask(pipe, chunksToTransfer));
      std::thread readerThread(&ReaderTask::run, ReaderTask(buffer, pipe, chunksToTransfer));
    
      writerThread.join();
      readerThread.join();
      
    }
    
    OATPP_ASSERT(buffer->getCurrentPosition() == chunksToTransfer * CHUNK_SIZE)
    
    auto ruleBuffer = std::make_shared<oatpp::data::stream::BufferOutputStream>();
    for(v_int32 i = 0; i < chunksToTransfer; i ++) {
      ruleBuffer->writeSimple(DATA_CHUNK, CHUNK_SIZE);
    }
    
    auto str1 = buffer->toString();
    auto str2 = buffer->toString();
    
    OATPP_ASSERT(str1 == str2)
    
  }
  
}
  
void PipeTest::onRun() {
  
  auto pipe = Pipe::createShared();

  v_int64 chunkCount = oatpp::data::buffer::IOBuffer::BUFFER_SIZE * 10 / CHUNK_SIZE;
  
  runTransfer(pipe, chunkCount, false, false);
  runTransfer(pipe, chunkCount, true, false);
  runTransfer(pipe, chunkCount, false, true);
  runTransfer(pipe, chunkCount, true, true);

}
  
}}}}
