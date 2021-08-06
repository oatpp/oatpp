/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#ifndef oatpp_web_protocol_http2_hpack_Hpack_hpp
#define oatpp_web_protocol_http2_hpack_Hpack_hpp

#include <utility>

#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/core/data/share/LazyStringMap.hpp"
#include "oatpp/core/Types.hpp"


namespace oatpp { namespace web { namespace protocol { namespace http2 { namespace hpack {

typedef std::multimap<oatpp::data::share::StringKeyLabelCI, oatpp::data::share::StringKeyLabel> HeaderMap;

class Table {
 public:
  class TableEntry {
   public:
    oatpp::data::share::StringKeyLabelCI key;
    oatpp::data::share::StringKeyLabel value;
    TableEntry(oatpp::data::share::StringKeyLabelCI pKey, oatpp::data::share::StringKeyLabel pValue)
        : key(std::move(pKey))
        , value(std::move(pValue)) {};
  };
 private:
  static const TableEntry s_staticTable[61];
  std::vector<TableEntry> m_dynamicTable;
  v_io_size m_maxEntries;
  v_io_size m_nextInDynamic;

 public:
  v_io_size findKey(const HeaderMap::iterator &it);
  v_io_size findKeyValue(const HeaderMap::iterator &it);
  bool keyHasValue(unsigned int idx);
  bool updateEntry(unsigned int idx, const oatpp::data::share::StringKeyLabelCI &key, const oatpp::data::share::StringKeyLabel &value);
  v_io_size addEntry(const oatpp::data::share::StringKeyLabelCI &key, const oatpp::data::share::StringKeyLabel &value);
  const TableEntry* getEntry(unsigned int idx);

  Table(v_io_size maxEntries)
    : m_maxEntries(maxEntries)
    , m_nextInDynamic(0) {
    m_dynamicTable.reserve(maxEntries);
  };
};

 class Hpack : public oatpp::base::Countable {
 private:
  enum IndexingMode {
    NoIndexing = 0u,
    NeverIndex = 0x10u,
    Indexing = 0x40u
  };

 private:
  Table m_table;
  static const char* TAG;

 public:
  Hpack() : m_table(64) {};

  std::list<Payload> deflate(const Headers &headers, v_io_size maxFrameSize);
  Headers inflate(const std::list<Payload> &payloads);

 private:
  v_io_size deflateKeyValuePair(Payload &to, const HeaderMap::iterator &it);

  static v_io_size handleNewTableSize(Payload &to, v_uint32 size);
  static v_io_size handleIndexedKeyValue(Payload &to, v_uint32 idx);
  static v_io_size handleIndexedKey(Payload &to, v_uint32 idx, data::share::StringKeyLabel &value, IndexingMode indexing);
  static v_io_size handleNewKeyValue(Payload &to,
                                     const data::share::StringKeyLabelCI &key,
                                     data::share::StringKeyLabel &value,
                                     IndexingMode indexing);

  static v_io_size calculateEncodedLength(v_uint32 size, v_uint32 prefix);
  static v_io_size encodeInteger(Payload &to, v_uint32 length, v_uint32 prefix);
  static v_io_size encodeString(Payload &to, p_uint8 const str, v_uint32 len);
  static IndexingMode shouldIndex(const oatpp::data::share::StringKeyLabelCI &key);
};

}}}}}

#endif //oatpp_web_protocol_http2_hpack_Hpack_hpp
