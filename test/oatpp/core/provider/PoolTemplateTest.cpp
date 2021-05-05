/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>,
 * Matthias Haselmaier <mhaselmaier@gmail.com>
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

#include "PoolTemplateTest.hpp"
#include <future>

#include "oatpp/core/provider/Pool.hpp"

namespace oatpp { namespace test { namespace core { namespace provider {

namespace {

struct Resource {
};

class Provider : public oatpp::provider::Provider<Resource> {
public:

  std::shared_ptr<Resource> get() override {
    return std::make_shared<Resource>();
  }

  async::CoroutineStarterForResult<const std::shared_ptr<Resource> &> getAsync() override {

    class GetCoroutine : public oatpp::async::CoroutineWithResult<GetCoroutine, const std::shared_ptr<Resource>&> {
    private:
      Provider* m_provider;
    public:

      GetCoroutine(Provider* provider)
        : m_provider(provider)
      {}

      Action act() override {
        return _return(std::make_shared<Resource>());
      }

    };

    return GetCoroutine::startForResult(this);
  }

  void invalidate(const std::shared_ptr<Resource>& resource) override {
    (void) resource;
  }

  void stop() override {
    OATPP_LOGD("Provider", "stop()");
  }

};

struct AcquisitionProxy : public oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy> {

  AcquisitionProxy(const std::shared_ptr<Resource>& resource, const std::shared_ptr<PoolInstance>& pool)
    : oatpp::provider::AcquisitionProxy<Resource, AcquisitionProxy>(resource, pool)
  {}

};

typedef oatpp::provider::PoolTemplate<Resource, AcquisitionProxy> PoolTemplate;

}

void PoolTemplateTest::onRun() {
  
  const auto provider = std::make_shared<Provider>();
  const v_int64 maxResources = 1;
  auto poolTemplate = PoolTemplate::createShared(provider, maxResources, std::chrono::seconds(1));  

  std::shared_ptr<Resource> resource = poolTemplate->get(poolTemplate);
  OATPP_ASSERT(resource != nullptr);

  OATPP_ASSERT(poolTemplate->get(poolTemplate, std::chrono::milliseconds(500)) == nullptr);
  std::future<std::shared_ptr<Resource>> futureResource = std::async(std::launch::async, [&poolTemplate]() {
    return poolTemplate->get(poolTemplate);
  });
  OATPP_ASSERT(futureResource.wait_for(std::chrono::seconds(1)) == std::future_status::timeout);

  poolTemplate->stop();

  OATPP_ASSERT(poolTemplate->get(poolTemplate, std::chrono::milliseconds(500)) == nullptr);
  OATPP_ASSERT(poolTemplate->get(poolTemplate) == nullptr);

}

}}}}
