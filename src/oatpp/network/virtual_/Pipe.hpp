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

#ifndef oatpp_network_virtual__Pipe_hpp
#define oatpp_network_virtual__Pipe_hpp

#include "oatpp/core/data/stream/Stream.hpp"

#include "oatpp/core/data/buffer/FIFOBuffer.hpp"
#include "oatpp/core/data/buffer/IOBuffer.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"

#include <mutex>
#include <condition_variable>

namespace oatpp { namespace network { namespace virtual_ {

/**
 * Virtual pipe implementation. Can be used for unidirectional data transfer between different threads of the same process. <br>
 * Under the hood it uses &id:oatpp::data::buffer::SynchronizedFIFOBuffer; over the &id:oatpp::data::buffer::IOBuffer;.
 */
class Pipe : public oatpp::base::Countable {
public:

  /**
   * Pipe Reader. Extends &id:oatpp::data::stream::InputStream;.
   * Provides read interface for the pipe. Can work in both blocking and nonblocking regime.
   */
  class Reader : public oatpp::data::stream::InputStream {
    friend Pipe;
  private:
    Pipe* m_pipe;
    bool m_nonBlocking;
    
    /*
     * this one used for testing purposes only
     */
    data::v_io_size m_maxAvailableToRead;
  protected:
    
    Reader(Pipe* pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
      , m_maxAvailableToRead(-1)
    {}

  public:

    /**
     * Set `true` to make non-blocking reads using &l:Pipe::Reader::read ();.
     * @param nonBlocking - `true` for nonblocking read.
     */
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }

    /**
     * Limit the available amount of bytes to read from pipe.<br>
     * This method is used for testing purposes only.<br>
     * set to -1 in order to ignore this value.<br>
     * @param maxAvailableToRead - maximum available amount of bytes to read.
     */
    void setMaxAvailableToRead(data::v_io_size maxAvailableToRead);

    /**
     * Implements &id:oatpp::data::stream::InputStream::read; method.
     * Read data from pipe.
     * @param data - buffer to read data to.
     * @param count - max count of bytes to read.
     * @return - &id:oatpp::data::v_io_size;.
     */
    data::v_io_size read(void *data, data::v_io_size count) override;
    
  };

  /**
   * Pipe writer. Extends &id:oatpp::data::stream::OutputStream;.
   * Provides write interface for the pipe. Can work in both blocking and nonblocking regime.
   */
  class Writer : public oatpp::data::stream::OutputStream {
    friend Pipe;
  private:
    Pipe* m_pipe;
    bool m_nonBlocking;
    
    /*
     * this one used for testing purposes only
     */
    data::v_io_size m_maxAvailableToWrtie;
  protected:
    
    Writer(Pipe* pipe, bool nonBlocking = false)
      : m_pipe(pipe)
      , m_nonBlocking(nonBlocking)
      , m_maxAvailableToWrtie(-1)
    {}

  public:

    /**
     * Set `true` to make non-blocking writes using &l:Pipe::Writer::write ();.
     * @param nonBlocking - `true` for nonblocking write.
     */
    void setNonBlocking(bool nonBlocking) {
      m_nonBlocking = nonBlocking;
    }

    /**
     * Limit the available space for data writes in pipe.<br>
     * This method is used for testing purposes only.<br>
     * set to -1 in order to ignore this value.<br>
     * @param maxAvailableToWrite - maximum available amount of bytes to write.
     */
    void setMaxAvailableToWrite(data::v_io_size maxAvailableToWrite);

    /**
     * Implements &id:oatpp::data::stream::OutputStream::write; method.
     * Write data to pipe.
     * @param data - data to write to pipe.
     * @param count - data size.
     * @return - &id:oatpp::data::v_io_size;.
     */
    data::v_io_size write(const void *data, data::v_io_size count) override;
    
  };
  
private:
  bool m_open;
  Writer m_writer;
  Reader m_reader;

  oatpp::data::buffer::IOBuffer m_buffer;
  oatpp::data::buffer::SynchronizedFIFOBuffer m_fifo;

  std::mutex m_mutex;
  std::condition_variable m_conditionRead;
  std::condition_variable m_conditionWrite;
public:

  /**
   * Constructor.
   */
  Pipe();

  /**
   * Create shared pipe.
   * @return - `std::shared_ptr` to Pipe.
   */
  static std::shared_ptr<Pipe> createShared();

  /**
   * Get pointer to &l:Pipe::Writer; for this pipe.
   * There can be only one &l:Pipe::Writer; per pipe.
   * @return - &l:Pipe::Writer;.
   */
  Writer* getWriter();


  /**
   * Get pointer to &l:Pipe::Reader; for this pipe.
   * There can be only one &l:Pipe::Reader; per pipe.
   * @return - &l:Pipe::Reader;.
   */
  Reader* getReader();

  /**
   * Mark pipe as closed.
   */
  void close();
  
};
  
}}}

#endif /* oatpp_network_virtual__Pipe_hpp */
