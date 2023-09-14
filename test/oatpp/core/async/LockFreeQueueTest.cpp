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

#include "LockFreeQueueTest.hpp"
#include "oatpp/core/async/utils/LockFreeQueue.hpp"
#include <thread>
#include <vector>

namespace oatpp { namespace test { namespace async {

void LockFreeQueueTest::onRun() {
  class Item {
    public:
      v_int32 value;
      Item() {
        value = 0;
      }
      explicit Item(v_int32 pValue)
        : value(pValue)
      {}
  };

  oatpp::async::utils::LockFreeQueue<Item> queue;

  std::vector<std::shared_ptr<std::thread>> producers;

  for (int i = 0; i < 10; ++i) {
    producers.push_back(std::make_shared<std::thread>([&queue, i]{
      for (v_int32 j = 0; j < 100; ++j) {
        queue.push(Item(i * 100 + j));
      }
    }));
  }

  v_int32 counter = 0;
  std::thread consumer([&queue, &counter]{
    Item item;
    while (counter < 1000) {
      if(queue.pop(item)) {
        counter ++;
      }
    }
  });

  for (auto& producer : producers) {
    producer->join();
  }
  consumer.join();
  OATPP_ASSERT(queue.empty())
  OATPP_ASSERT(counter == 1000)
}

}}}
