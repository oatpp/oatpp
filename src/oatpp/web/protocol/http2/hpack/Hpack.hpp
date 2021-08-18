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

#include "oatpp/core/data/stream/FIFOStream.hpp"
#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"

#include "oatpp/web/protocol/http2/Http2.hpp"
#include "oatpp/core/data/share/LazyStringMap.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http2 { namespace hpack {

typedef std::multimap<oatpp::data::share::StringKeyLabelCI, oatpp::data::share::StringKeyLabel> HeaderMap;

class Table : public oatpp::base::Countable {
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

class SimpleTable : public Table {
 private:
  static const TableEntry s_staticTable[61];
  std::vector<TableEntry> m_dynamicTable;

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

  explicit SimpleTable(v_io_size maxEntries) {
    m_dynamicTable.reserve(maxEntries);
  };
  static std::shared_ptr<SimpleTable> createShared(v_io_size maxEntries) {
    return std::make_shared<SimpleTable>(maxEntries);
  }
  ~SimpleTable() override;
};

class Hpack : public oatpp::base::Countable {
 public:
  virtual std::shared_ptr<data::stream::BufferedInputStream> deflate(const Headers &headers) = 0;
  virtual Headers inflate(const std::list<Payload> &payloads) = 0;
  virtual Headers inflate(const std::shared_ptr<data::stream::BufferedInputStream> &stream, v_io_size streamPayloadLength) = 0;
  virtual ~Hpack() = default;
};

class SimpleHpack : public Hpack {
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
  explicit SimpleHpack(const std::shared_ptr<Table>& table);
  static std::shared_ptr<SimpleHpack> createShared(const std::shared_ptr<Table> &table) {
    return std::make_shared<SimpleHpack>(table);
  }
  ~SimpleHpack();

  std::shared_ptr<data::stream::BufferedInputStream> deflate(const Headers &headers) override;
  Headers inflate(const std::list<Payload> &payloads) override;
  Headers inflate(const std::shared_ptr<data::stream::BufferedInputStream> &stream,
                  v_io_size streamPayloadLength) override;

 private:
  v_io_size inflateKeyValuePair(InflateMode mode,
                                Payload::const_iterator it,
                                Payload::const_iterator last,
                                Headers &hdr);
  v_io_size inflateKeyValuePair(SimpleHpack::InflateMode mode,
                                data::stream::BufferedInputStream *stream,
                                v_io_size streamPayloadLength,
                                Headers &hdr,
                                async::Action &action);
  v_io_size inflateKeyValuePairs(Headers &hdr, const Payload &payload);
  v_io_size deflateKeyValuePair(data::stream::WriteCallback *to, const HeaderMap::iterator &it);

  v_io_size inflateHandleNewTableSize(Payload::const_iterator it, Payload::const_iterator last);
  v_io_size inflateHandleNewTableSize(data::stream::BufferedInputStream *stream,
                                      v_io_size streamPayloadLength);

  static v_io_size deflateHandleNewTableSize(data::stream::WriteCallback *to, v_uint32 size);
  static v_io_size deflateHandleIndexedKeyValue(data::stream::WriteCallback *to, v_uint32 idx);
  static v_io_size deflateHandleIndexedKey(data::stream::WriteCallback *to,
                                           v_uint32 idx,
                                           data::share::StringKeyLabel &value,
                                           IndexingMode indexing);
  static v_io_size deflateHandleNewKeyValue(data::stream::WriteCallback *to,
                                            const data::share::StringKeyLabelCI &key,
                                            data::share::StringKeyLabel &value,
                                            IndexingMode indexing);

  static v_io_size deflateString(data::stream::WriteCallback *to, p_uint8 const str, v_uint32 len);
  static v_io_size inflateString(oatpp::String &value, Payload::const_iterator it, Payload::const_iterator last);

  static v_io_size inflateString(String &value,
                                 data::stream::BufferedInputStream *stream,
                                 v_io_size streamSize,
                                 async::Action action);

  static v_io_size calculateEncodedLength(v_uint32 size, v_uint32 prefix);
  static v_io_size encodeInteger(data::stream::WriteCallback *to,
                                 v_uint8 flags,
                                 v_uint32 length,
                                 v_uint32 prefix);
  static v_io_size encodeTableSize(data::stream::WriteCallback *to, v_uint32 size);
  static v_io_size decodeInteger(v_uint32 *res,
                                 Payload::const_iterator in,
                                 Payload::const_iterator last,
                                 size_t prefix);
  static v_io_size decodeInteger(v_uint32 *res,
                                 data::stream::BufferedInputStream *stream,
                                 v_io_size streamPayloadLength,
                                 v_uint32 prefix);
  static v_io_size decodeString(String &key, Payload::const_iterator in, Payload::const_iterator end);
  static v_io_size decodeString(String &key,
                                v_io_size stringSize,
                                data::stream::BufferedInputStream *stream);
  static IndexingMode shouldIndex(const oatpp::data::share::StringKeyLabelCI &key);
};

}}}}}

#endif //oatpp_web_protocol_http2_hpack_Hpack_hpp
