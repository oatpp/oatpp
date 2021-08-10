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
        : key(std::move(pKey)), value(std::move(pValue)) {};
  };

 public:
  virtual v_io_size findKey(const HeaderMap::iterator &it) = 0;
  virtual v_io_size findKeyValue(const HeaderMap::iterator &it) = 0;
  virtual bool keyHasValue(unsigned int idx) = 0;
  virtual bool updateEntry(unsigned int idx,
                           const oatpp::data::share::StringKeyLabelCI &key,
                           const oatpp::data::share::StringKeyLabel &value) = 0;
  virtual v_io_size addEntry(const oatpp::data::share::StringKeyLabelCI &key,
                             const oatpp::data::share::StringKeyLabel &value) = 0;
  virtual const TableEntry *getEntry(unsigned int idx) = 0;
  virtual v_uint32 changeTableSize(v_uint32 newSize) = 0;
  virtual v_uint32 getTableSize() = 0;
  virtual ~Table() = default;
};

class SimpleTable : public Table, public oatpp::base::Countable {
 private:
  static const TableEntry s_staticTable[61];
  std::vector<TableEntry> m_dynamicTable;
  v_io_size m_maxEntries;
  v_io_size m_nextInDynamic;

 public:
  v_io_size findKey(const HeaderMap::iterator &it) override;
  v_io_size findKeyValue(const HeaderMap::iterator &it) override;
  bool keyHasValue(unsigned int idx) override;
  bool updateEntry(unsigned int idx,
                   const oatpp::data::share::StringKeyLabelCI &key,
                   const oatpp::data::share::StringKeyLabel &value) override;
  v_io_size addEntry(const oatpp::data::share::StringKeyLabelCI &key,
                     const oatpp::data::share::StringKeyLabel &value) override;
  const TableEntry *getEntry(unsigned int idx) override;
  v_uint32 changeTableSize(v_uint32 newSize) override;
  v_uint32 getTableSize() override;

  SimpleTable(v_io_size maxEntries)
      : m_maxEntries(maxEntries), m_nextInDynamic(0) {
    m_dynamicTable.reserve(maxEntries);
  };
  ~SimpleTable() override;
};

class Hpack {
 public:
  virtual std::list<Payload> deflate(const Headers &headers, v_io_size maxFrameSize) = 0;
  virtual Headers inflate(const std::list<Payload> &payloads) = 0;
  virtual ~Hpack() = default;
};

class SimpleHpack : public Hpack, public oatpp::base::Countable {
 private:
  enum IndexingMode {
    NO_INDEXING = 0u,
    NEVER_INDEX = 0x10u,
    INDEXING = 0x40u
  };

  enum InflateMode {
    INDEXED_KEY_INDEXED_VALUE,
    INDEXED_KEY_TEXT_VALUE,
    TEXT_KEY_TEXT_VALUE,
  };

 private:
  std::shared_ptr<Table> m_table;
  v_uint32 m_initialTableSize;
  static const char *TAG;

 public:
  SimpleHpack(std::shared_ptr<Table> table);

  std::list<Payload> deflate(const Headers &headers, v_io_size maxFrameSize);
  Headers inflate(const std::list<Payload> &payloads);

 private:
  v_io_size inflateKeyValuePair(InflateMode mode,
                                Payload::const_iterator it,
                                Payload::const_iterator last,
                                Headers &hdr);
  v_io_size inflateKeyValuePairs(Headers &hdr, const Payload &payload);
  v_io_size deflateKeyValuePair(Payload &to, const HeaderMap::iterator &it);

  v_io_size inflateHandleNewTableSize(Payload::const_iterator it, Payload::const_iterator last);

  static v_io_size deflateHandleNewTableSize(Payload &to, v_uint32 size);
  static v_io_size deflateHandleIndexedKeyValue(Payload &to, v_uint32 idx);
  static v_io_size deflateHandleIndexedKey(Payload &to,
                                           v_uint32 idx,
                                           data::share::StringKeyLabel &value,
                                           IndexingMode indexing);
  static v_io_size deflateHandleNewKeyValue(Payload &to,
                                            const data::share::StringKeyLabelCI &key,
                                            data::share::StringKeyLabel &value,
                                            IndexingMode indexing);

  static v_io_size deflateString(Payload &to, p_uint8 const str, v_uint32 len);
  static v_io_size inflateString(oatpp::String &value, Payload::const_iterator it, Payload::const_iterator last);

  static v_io_size calculateEncodedLength(v_uint32 size, v_uint32 prefix);
  static v_io_size encodeInteger(Payload &to, v_uint32 length, v_uint32 prefix);
  static v_io_size encodeTableSize(Payload &to, v_uint32 size);
  static v_io_size decodeInteger(v_uint32 *res,
                                 Payload::const_iterator in,
                                 Payload::const_iterator last,
                                 size_t prefix);
  static v_io_size decodeString(String &key, Payload::const_iterator in, Payload::const_iterator end);
  static IndexingMode shouldIndex(const oatpp::data::share::StringKeyLabelCI &key);
};

}}}}}

#endif //oatpp_web_protocol_http2_hpack_Hpack_hpp
