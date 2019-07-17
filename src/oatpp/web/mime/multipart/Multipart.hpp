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

#ifndef oatpp_web_mime_multipart_Multipart_hpp
#define oatpp_web_mime_multipart_Multipart_hpp

#include "Part.hpp"
#include <unordered_map>
#include <list>

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * Typedef for headers map. Headers map key is case-insensitive.
 * `std::unordered_map` of &id:oatpp::data::share::StringKeyLabelCI_FAST; and &id:oatpp::data::share::StringKeyLabel;.
 */
typedef std::unordered_map<oatpp::data::share::StringKeyLabelCI_FAST, oatpp::data::share::StringKeyLabel> Headers;

/**
 * Structure that holds parts of Multipart.
 */
class Multipart {
private:
  oatpp::String m_boundary;
  std::unordered_map<oatpp::String, std::shared_ptr<Part>> m_namedParts;
  std::list<std::shared_ptr<Part>> m_parts;
public:

  /**
   * Constructor.
   * @param boundary - multipart boundary value.
   */
  Multipart(const oatpp::String& boundary);

  /**
   * Constructor.
   * @param requestHeaders - request headers. Headers must contain "Content-Type" header.
   */
  Multipart(const Headers& requestHeaders);

  /**
   * Default virtual Destructor.
   */
  virtual ~Multipart() = default;

  /**
   * Get multipart boundary value.
   * @return - multipart boundary value.
   */
  oatpp::String getBoundary();

  /**
   * Add part to Multipart.
   * @param part - &id:oatpp::web::mime::multipart::Part;.
   */
  void addPart(const std::shared_ptr<Part>& part);

  /**
   * Get part by name <br>
   * Applicable to named parts only.
   * @param name - &id:oatpp::String;.
   * @return - &id:oatpp::web::mime::multipart::Part;.
   */
  std::shared_ptr<Part> getNamedPart(const oatpp::String& name);

  /**
   * Get list of all parts.
   * @return - `std::list` of `std::shared_ptr` to &id:oatpp::web::mime::multipart::Part;.
   */
  const std::list<std::shared_ptr<Part>>& getAllParts();

  /**
   * Get parts count.
   * @return - parts count.
   */
  v_int32 count();

};

}}}}


#endif // oatpp_web_mime_multipart_Multipart_hpp
