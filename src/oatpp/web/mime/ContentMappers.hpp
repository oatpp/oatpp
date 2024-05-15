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

#ifndef oatpp_web_mime_ContentMappers_hpp
#define oatpp_web_mime_ContentMappers_hpp

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/data/mapping/ObjectMapper.hpp"

#include <shared_mutex>

namespace oatpp::web::mime {

class ContentMappers {
private:

  struct MatchedMapper {

    std::shared_ptr<data::mapping::ObjectMapper> mapper;
    v_float64 quality;

    bool operator < (const MatchedMapper& other) const {
      return quality > other.quality;
    }

  };

private:
  typedef std::unordered_map<data::share::StringKeyLabelCI, std::shared_ptr<data::mapping::ObjectMapper>> MappersBySubtypes;
private:
  std::pair<oatpp::String, oatpp::String> typeAndSubtype(const data::share::StringKeyLabelCI& contentType) const;
  std::shared_ptr<data::mapping::ObjectMapper> selectMapper(const protocol::http::HeaderValueData& values) const;
private:
  std::unordered_map<data::share::StringKeyLabelCI, MappersBySubtypes> m_index;
  std::unordered_map<data::share::StringKeyLabelCI, std::shared_ptr<data::mapping::ObjectMapper>> m_mappers;
  std::shared_ptr<data::mapping::ObjectMapper> m_defaultMapper;
  mutable std::shared_mutex m_mutex;
public:

  ContentMappers() = default;
  virtual ~ContentMappers() = default;

  void putMapper(const std::shared_ptr<data::mapping::ObjectMapper>& mapper);

  void setDefaultMapper(const oatpp::String& contentType);
  void setDefaultMapper(const std::shared_ptr<data::mapping::ObjectMapper>& mapper);

  std::shared_ptr<data::mapping::ObjectMapper> getMapper(const oatpp::String& contentType) const;
  std::shared_ptr<data::mapping::ObjectMapper> getDefaultMapper() const;

  std::shared_ptr<data::mapping::ObjectMapper> selectMapperForContent(const oatpp::String& contentTypeHeader) const;

  std::shared_ptr<data::mapping::ObjectMapper> selectMapper(const oatpp::String& acceptHeader) const;
  std::shared_ptr<data::mapping::ObjectMapper> selectMapper(const std::vector<oatpp::String>& acceptableContentTypes) const;

  void clear();

};

}

#endif //oatpp_web_mime_ContentMappers_hpp
