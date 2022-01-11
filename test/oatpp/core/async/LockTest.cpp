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

#include "LockTest.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/async/Lock.hpp"

#include <thread>
#include <list>

namespace oatpp { namespace test { namespace async {

namespace {

static constexpr v_int32 NUM_SYMBOLS = 20;

class Buff {
private:
  oatpp::data::stream::BufferOutputStream *m_buffer;
  std::mutex m_mutex;
public:

  Buff(oatpp::data::stream::BufferOutputStream *buffer)
    : m_buffer(buffer) {}

  void writeChar(char c) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffer->writeCharSimple(c);
  }

};

class TestCoroutine : public oatpp::async::Coroutine<TestCoroutine> {
private:
  char m_symbol;
  Buff *m_buff;
  oatpp::async::LockGuard m_lockGuard;
  v_int32 m_counter;
public:

  TestCoroutine(char symbol, Buff *buff, oatpp::async::Lock *lock)
    : m_symbol(symbol), m_buff(buff), m_lockGuard(lock), m_counter(0) {}

  Action act() override {
    return m_lockGuard.lockAsync().next(yieldTo(&TestCoroutine::writeSymbol));
    //return yieldTo(&TestCoroutine::writeSymbol);
  }

  Action writeSymbol() {
    if (m_counter < NUM_SYMBOLS) {
      m_counter++;
      m_buff->writeChar(m_symbol);
      return repeat();
    }
    m_lockGuard.unlock();
    return finish();
  }

};

class NotSynchronizedCoroutine : public oatpp::async::Coroutine<NotSynchronizedCoroutine> {
private:
  char m_symbol;
  Buff *m_buff;
  v_int32 m_counter;
public:

  NotSynchronizedCoroutine(char symbol, Buff *buff)
    : m_symbol(symbol), m_buff(buff), m_counter(0)
  {}

  Action act() override {
    if (m_counter < NUM_SYMBOLS) {
      m_counter++;
      m_buff->writeChar(m_symbol);
      return repeat();
    }
    return finish();
  }

};

class TestCoroutine2 : public oatpp::async::Coroutine<TestCoroutine2> {
private:
  char m_symbol;
  Buff *m_buff;
  oatpp::async::Lock *m_lock;
public:

  TestCoroutine2(char symbol, Buff *buff, oatpp::async::Lock *lock)
    : m_symbol(symbol)
    , m_buff(buff)
    , m_lock(lock)
  {}

  Action act() override {
    return oatpp::async::synchronize(m_lock, NotSynchronizedCoroutine::start(m_symbol, m_buff)).next(finish());
  }

};

void testMethod(char symbol, Buff *buff, oatpp::async::Lock *lock) {

  std::lock_guard<oatpp::async::Lock> lockGuard(*lock);
  for (v_int32 i = 0; i < NUM_SYMBOLS; i++) {
    buff->writeChar(symbol);
  }

}

bool checkSymbol(char symbol, const char* data, v_buff_size size) {

  for (v_buff_size i = 0; i < size; i++) {

    if (data[i] == symbol && size - i >= NUM_SYMBOLS) {

      for (v_buff_size j = 0; j < NUM_SYMBOLS; j++) {

        if (data[i + j] != symbol) {
          OATPP_LOGD("aaa", "j pos=%d", j);
          return false;
        }

      }

      return true;

    }

  }

  OATPP_LOGD("aaa", "No symbol found");
  return false;

}

bool checkSymbol(char symbol, const oatpp::String& str) {
  return checkSymbol(symbol, str->data(), str->size());
}

}


void LockTest::onRun() {

  oatpp::async::Lock lock;
  oatpp::data::stream::BufferOutputStream buffer;
  Buff buff(&buffer);

  oatpp::async::Executor executor(10, 1, 1);

  for (v_int32 c = 0; c <= 127; c++) {
    executor.execute<TestCoroutine>((char)c, &buff, &lock);
  }

  for (v_int32 c = 128; c <= 200; c++) {
    executor.execute<TestCoroutine2>((char)c, &buff, &lock);
  }

  std::list<std::thread> threads;

  for (v_int32 c = 201; c <= 255; c++) {
    threads.push_back(std::thread(testMethod, (char)c, &buff, &lock));
  }

  for (std::thread &thread : threads) {
    thread.join();
  }

  executor.waitTasksFinished();
  executor.stop();
  executor.join();

  auto result = buffer.toString();

  for (v_int32 c = 0; c <= 255; c++) {
    bool check = checkSymbol((char)c, result);
    if(!check) {
      v_int32 code = c;
      auto str = oatpp::String((const char*)&c, 1);
      OATPP_LOGE(TAG, "Failed for symbol %d, '%s'", code, str->data());
    }
    OATPP_ASSERT(check);
  }

}

}}}