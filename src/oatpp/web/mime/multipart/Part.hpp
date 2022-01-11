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

#ifndef oatpp_web_mime_multipart_Part_hpp
#define oatpp_web_mime_multipart_Part_hpp

#include "oatpp/core/data/share/LazyStringMap.hpp"
#include "oatpp/core/data/resource/Resource.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * One part of the multipart.
 */
class Part {
public:
  /**
   * Typedef for headers map. Headers map key is case-insensitive.
   * For more info see &id:oatpp::data::share::LazyStringMap;.
   */
  typedef oatpp::data::share::LazyStringMultimap<oatpp::data::share::StringKeyLabelCI> Headers;
private:
  oatpp::String m_name;
  oatpp::String m_filename;
  Headers m_headers;
  std::shared_ptr<data::resource::Resource> m_payload;
private:
  const char* m_tagName;
  std::shared_ptr<oatpp::base::Countable> m_tagObject;
public:

  /**
   * Default constructor.
   */
  Part() = default;

  /**
   * Constructor.
   * @param headers - headers of the part.
   * @param payload - part payload.
   */
  Part(const Headers& headers, const std::shared_ptr<data::resource::Resource>& payload = nullptr);

  /**
   * Set payload.
   * @param payload
   */
  void setPayload(const std::shared_ptr<data::resource::Resource>& payload);

  /**
   * Get payload.
   * @return
   */
  std::shared_ptr<data::resource::Resource> getPayload();

  /**
   * Get name of the part.
   * @return - name of the part.
   */
  oatpp::String getName() const;

  /**
   * Get filename of the part (if applicable).
   * @return - filename.
   */
  oatpp::String getFilename() const;

  /**
   * Get request's headers map.
   * @return Headers map
   */
  const Headers& getHeaders() const;

  /**
   * Get header value
   * @param headerName
   * @return header value
   */
  oatpp::String getHeader(const oatpp::data::share::StringKeyLabelCI& headerName) const;

  /**
   * Add http header.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   */
  void putHeader(const oatpp::data::share::StringKeyLabelCI& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Add http header if not already exists.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   * @return - `true` if header was added.
   */
  bool putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Tag-object - object used to associate some data with the Part. <br>
   * Ex.: used by &id:oatpp::web::mime::multipart::InMemoryPartReader;. to
   * associate intermediate buffer with the part.
   * @param tagName
   * @param tagObject
   */
  void setTag(const char* tagName, const std::shared_ptr<oatpp::base::Countable>& tagObject);

  /**
   * Get tag name.
   * @return
   */
  const char* getTagName();

  /**
   * Get tag object.
   * @return
   */
  std::shared_ptr<oatpp::base::Countable> getTagObject();

  /**
   * Clear the tag.
   */
  void clearTag();

};

}}}}

#endif // oatpp_web_mime_multipart_Part_hpp
