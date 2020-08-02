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

#ifndef oatpp_web_mime_multipart_PartList_hpp
#define oatpp_web_mime_multipart_PartList_hpp

#include "Multipart.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * Structure that holds Multipart parts in the `std::list`.
 */
class PartList : public Multipart {
private:
  std::unordered_map<oatpp::String, std::shared_ptr<Part>> m_namedParts;
  bool m_readIteratorInitialized;
  std::list<std::shared_ptr<Part>> m_parts;
  std::list<std::shared_ptr<Part>>::const_iterator m_iterator;
public:

  /**
   * Constructor.
   * @param boundary - multipart boundary value.
   */
  PartList(const oatpp::String& boundary);

  /**
   * Constructor.
   * @param requestHeaders - request headers. Headers must contain "Content-Type" header.
   */
  PartList(const Headers& requestHeaders);

  /**
   * Create Multipart object with random boundary. <br>
   * It will generate random vector of size `boundarySize` in bytes encoded in base64.
   * @param boundarySize - size of the random vecrot in bytes.
   * @return - `std::shared_ptr` to Multipart.
   */
  static std::shared_ptr<PartList> createSharedWithRandomBoundary(v_int32 boundarySize = 15);

  /**
   * Read part-by-part from Multipart.
   * @return
   */
  std::shared_ptr<Part> readNextPart(async::Action& action) override;

  /**
   * Write part-by-part to Multipart.
   * @param part
   */
  void writeNextPart(const std::shared_ptr<Part>& part, async::Action& action) override;

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
  v_int64 count();

};

}}}}

#endif //oatpp_web_mime_multipart_PartList_hpp
