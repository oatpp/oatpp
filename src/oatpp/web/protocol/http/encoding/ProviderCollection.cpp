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

#include "ProviderCollection.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace encoding {

void ProviderCollection::add(const std::shared_ptr<EncoderProvider>& provider) {
  m_providers[provider->getEncodingName()] = provider;
}

std::shared_ptr<EncoderProvider> ProviderCollection::get(const data::share::StringKeyLabelCI& encoding) const {
  auto it = m_providers.find(encoding);
  if(it != m_providers.end()) {
    return it->second;
  }
  return nullptr;
}

std::shared_ptr<EncoderProvider> ProviderCollection::get(const std::unordered_set<data::share::StringKeyLabelCI>& encodings) const {

  for(const auto& encoding : encodings) {
    auto provider = get(encoding);
    if(provider) {
      return provider;
    }
  }

  return nullptr;

}

}}}}}
