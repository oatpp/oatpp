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

#include "oatpp/async/CoroutineWaitList.hpp"

#include "oatpp/data/stream/Stream.hpp"

#include "oatpp/data/buffer/FIFOBuffer.hpp"
#include "oatpp/data/buffer/IOBuffer.hpp"

#include "oatpp/concurrency/SpinLock.hpp"

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
  public:
    static data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
  private:

    class WaitListListener : public oatpp::async::CoroutineWaitList::Listener {
    private:
      Pipe* m_pipe;
    public:

      WaitListListener(Pipe* pipe)
        : m_pipe(pipe)
      {}

      void onNewItem(oatpp::async::CoroutineWaitList& list) override {
        std::lock_guard<std::mutex> lock(m_pipe->m_mutex);
        if (m_pipe->m_fifo.availableToRead() > 0 || !m_pipe->m_open) {
          list.notifyAll();
        }
      }

    };

  private:
    Pipe* m_pipe;
    oatpp::data::stream::IOMode m_ioMode;
    
    /*
     * this one used for testing purposes only
     */
    v_io_size m_maxAvailableToRead;

    oatpp::async::CoroutineWaitList m_waitList;
    WaitListListener m_waitListListener;
  protected:
    
    Reader(Pipe* pipe, oatpp::data::stream::IOMode ioMode = oatpp::data::stream::IOMode::BLOCKING)
      : m_pipe(pipe)
      , m_ioMode(ioMode)
      , m_maxAvailableToRead(-1)
      , m_waitListListener(pipe)
    {
      m_waitList.setListener(&m_waitListListener);
    }

  public:

    /**
     * Limit the available amount of bytes to read from pipe.<br>
     * This method is used for testing purposes only.<br>
     * set to -1 in order to ignore this value.<br>
     * @param maxAvailableToRead - maximum available amount of bytes to read.
     */
    void setMaxAvailableToRead(v_io_size maxAvailableToRead);

    /**
     * Implements &id:oatpp::data::stream::InputStream::read; method.
     * Read data from pipe.
     * @param data - buffer to read data to.
     * @param count - max count of bytes to read.
     * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
     * caller MUST return this action on coroutine iteration.
     * @return - &id:oatpp::v_io_size;.
     */
    v_io_size read(void *data, v_buff_size count, async::Action& action) override;

    /**
     * Set InputStream I/O mode.
     * @param ioMode
     */
    void setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;

    /**
     * Get InputStream I/O mode.
     * @return
     */
    oatpp::data::stream::IOMode getInputStreamIOMode() override;

    /**
     * Get stream context.
     * @return
     */
    oatpp::data::stream::Context& getInputStreamContext() override;

    /**
     * Notify coroutine wait-list
     */
    void notifyWaitList();
    
  };

  /**
   * Pipe writer. Extends &id:oatpp::data::stream::OutputStream;.
   * Provides write interface for the pipe. Can work in both blocking and nonblocking regime.
   */
  class Writer : public oatpp::data::stream::OutputStream {
    friend Pipe;
  public:
    static data::stream::DefaultInitializedContext DEFAULT_CONTEXT;
  private:
    class WaitListListener : public oatpp::async::CoroutineWaitList::Listener {
    private:
      Pipe* m_pipe;
    public:
      WaitListListener(Pipe* pipe)
        : m_pipe(pipe)
      {}

      void onNewItem(oatpp::async::CoroutineWaitList& list) override {
        std::lock_guard<std::mutex> lock(m_pipe->m_mutex);
        if (m_pipe->m_fifo.availableToWrite() > 0 || !m_pipe->m_open) {
          list.notifyAll();
        }
      }
    };
  private:
    Pipe* m_pipe;
    oatpp::data::stream::IOMode m_ioMode;
    
    /*
     * this one used for testing purposes only
     */
    v_io_size m_maxAvailableToWrtie;

    oatpp::async::CoroutineWaitList m_waitList;
    WaitListListener m_waitListListener;
  protected:
    
    Writer(Pipe* pipe, oatpp::data::stream::IOMode ioMode = oatpp::data::stream::IOMode::BLOCKING)
      : m_pipe(pipe)
      , m_ioMode(ioMode)
      , m_maxAvailableToWrtie(-1)
      , m_waitListListener(pipe)
    {
      m_waitList.setListener(&m_waitListListener);
    }

  public:

    /**
     * Limit the available space for data writes in pipe.<br>
     * This method is used for testing purposes only.<br>
     * set to -1 in order to ignore this value.<br>
     * @param maxAvailableToWrite - maximum available amount of bytes to write.
     */
    void setMaxAvailableToWrite(v_io_size maxAvailableToWrite);

    /**
     * Implements &id:oatpp::data::stream::OutputStream::write; method.
     * Write data to pipe.
     * @param data - data to write to pipe.
     * @param count - data size.
     * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
     * caller MUST return this action on coroutine iteration.
     * @return - &id:oatpp::v_io_size;.
     */
    v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

    /**
     * Set OutputStream I/O mode.
     * @param ioMode
     */
    void setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) override;

    /**
     * Set OutputStream I/O mode.
     * @return
     */
    oatpp::data::stream::IOMode getOutputStreamIOMode() override;

    /**
     * Get stream context.
     * @return
     */
    oatpp::data::stream::Context& getOutputStreamContext() override;

    /**
     * Notify coroutine wait-list
     */
    void notifyWaitList();
    
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
   * Virtual destructor.
   */
  virtual ~Pipe() override;

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
