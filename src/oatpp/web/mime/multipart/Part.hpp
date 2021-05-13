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
#include "oatpp/core/data/stream/Stream.hpp"

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
  typedef oatpp::data::share::LazyStringMultimap<oatpp::data::share::StringKeyLabelCI_FAST> Headers;
private:
  oatpp::String m_name;
  oatpp::String m_filename;
  Headers m_headers;
  std::shared_ptr<data::stream::InputStream> m_inputStream;
  oatpp::String m_inMemoryData;
  v_int64 m_knownSize;
private:
  const char* m_tagName;
  std::shared_ptr<oatpp::base::Countable> m_tagObject;
public:

  /**
   * Constructor.
   * @param headers - headers of the part.
   * @param inputStream - input stream of the part data.
   * @param inMemoryData - possible in-memory data of the part. Same data as the referred by input stream. For convenience purposes.
   * @param knownSize - known size of the data in the input stream. Pass `-1` value if size is unknown.
   */
  Part(const Headers& headers,
       const std::shared_ptr<data::stream::InputStream>& inputStream,
       const oatpp::String inMemoryData,
       v_int64 knownSize);

  /**
   * Constructor.
   * @param headers - headers of the part.
   */
  Part(const Headers& headers);

  /**
   * Default constructor.
   */
  Part();

  /**
   * Set part data info.
   * @param inputStream - input stream of the part data.
   * @param inMemoryData - possible in-memory data of the part. Same data as the referred by input stream. For convenience purposes.
   * @param knownSize - known size of the data in the input stream. Pass `-1` value if size is unknown.
   */
  void setDataInfo(const std::shared_ptr<data::stream::InputStream>& inputStream,
                   const oatpp::String inMemoryData,
                   v_int64 knownSize);

  /**
   * Same as `setDataInfo(inputStream, nullptr, -1);.`
   * @param inputStream - input stream of the part data.
   */
  void setDataInfo(const std::shared_ptr<data::stream::InputStream>& inputStream);

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
  oatpp::String getHeader(const oatpp::data::share::StringKeyLabelCI_FAST& headerName) const;

  /**
   * Add http header.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI_FAST;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   */
  void putHeader(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Add http header if not already exists.
   * @param key - &id:oatpp::data::share::StringKeyLabelCI_FAST;.
   * @param value - &id:oatpp::data::share::StringKeyLabel;.
   * @return - `true` if header was added.
   */
  bool putHeaderIfNotExists(const oatpp::data::share::StringKeyLabelCI_FAST& key, const oatpp::data::share::StringKeyLabel& value);

  /**
   * Get input stream of the part data.
   * @return - input stream of the part data.
   */
  std::shared_ptr<data::stream::InputStream> getInputStream() const;

  /**
   * Get in-memory data (if applicable). <br>
   * It may be possible set for the part in case of storing part data in memory. <br>
   * This property is optional. Preferred way to access data of the part is through `getInputStream()` method.
   * @return - in-memory data.
   */
  oatpp::String getInMemoryData() const;

  /**
   * Return known size of the part data.
   * @return - known size of the part data. `-1` - if size is unknown.
   */
  v_int64 getKnownSize() const;

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
