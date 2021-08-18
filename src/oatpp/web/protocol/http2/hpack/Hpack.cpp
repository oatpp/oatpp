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

#include "Hpack.hpp"
#include "Huffman.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http2 { namespace hpack {

const SimpleTable::TableEntry SimpleTable::s_staticTable[61] = {
    {Header::AUTHORITY, nullptr},
    {Header::METHOD, oatpp::data::share::StringKeyLabel("GET")},
    {Header::METHOD, oatpp::data::share::StringKeyLabel("POST")},
    {Header::PATH, oatpp::data::share::StringKeyLabel("/")},
    {Header::PATH, oatpp::data::share::StringKeyLabel("/index.html")},
    {Header::SCHEME, oatpp::data::share::StringKeyLabel("http")},
    {Header::SCHEME, oatpp::data::share::StringKeyLabel("https")},
    {Header::STATUS, oatpp::data::share::StringKeyLabel("200")},
    {Header::STATUS, oatpp::data::share::StringKeyLabel("204")},
    {Header::STATUS, oatpp::data::share::StringKeyLabel("206")},
    {Header::STATUS, oatpp::data::share::StringKeyLabel("304")},
    {Header::STATUS, oatpp::data::share::StringKeyLabel("400")},
    {Header::STATUS, oatpp::data::share::StringKeyLabel("404")},
    {Header::STATUS, oatpp::data::share::StringKeyLabel("500")},
    {Header::ACCEPT_CHARSET, nullptr},
    {Header::ACCEPT_ENCODING, oatpp::data::share::StringKeyLabel("gzip, deflate")},
    {Header::ACCEPT_LANGUAGE, nullptr},
    {Header::ACCEPT_RANGES, nullptr},
    {Header::ACCEPT, nullptr},
    {Header::ACCESS_CONTROL_ALLOW_ORIGIN, nullptr},
    {Header::AGE, nullptr},
    {Header::ALLOW, nullptr},
    {Header::AUTHORIZATION, nullptr},
    {Header::CACHE_CONTROL, nullptr},
    {Header::CONTENT_DISPOSITION, nullptr},
    {Header::CONTENT_ENCODING, nullptr},
    {Header::CONTENT_LANGUAGE, nullptr},
    {Header::CONTENT_LENGTH, nullptr},
    {Header::CONTENT_LOCATION, nullptr},
    {Header::CONTENT_RANGE, nullptr},
    {Header::CONTENT_TYPE, nullptr},
    {Header::COOKIE, nullptr},
    {Header::DATE, nullptr},
    {Header::ETAG, nullptr},
    {Header::EXPECT, nullptr},
    {Header::EXPIRES, nullptr},
    {Header::FROM, nullptr},
    {Header::HOST, nullptr},
    {Header::IF_MATCH, nullptr},
    {Header::IF_MODIFIED_SINCE, nullptr},
    {Header::IF_NONE_MATCH, nullptr},
    {Header::IF_RANGE, nullptr},
    {Header::IF_UNMODIFIED_SINCE, nullptr},
    {Header::LAST_MODIFIED, nullptr},
    {Header::LINK, nullptr},
    {Header::LOCATION, nullptr},
    {Header::MAX_FORWARDS, nullptr},
    {Header::PROXY_AUTHENTICATE, nullptr},
    {Header::PROXY_AUTHORIZATION, nullptr},
    {Header::RANGE, nullptr},
    {Header::REFERER, nullptr},
    {Header::REFRESH, nullptr},
    {Header::RETRY_AFTER, nullptr},
    {Header::SERVER, nullptr},
    {Header::SET_COOKIE, nullptr},
    {Header::STRICT_TRANSPORT_SECURITY, nullptr},
    {Header::TRANSFER_ENCODING, nullptr},
    {Header::USER_AGENT, nullptr},
    {Header::VARY, nullptr},
    {Header::VIA, nullptr},
    {Header::WWW_AUTHENTICATE, nullptr}
};

v_io_size SimpleTable::findKeyValue(const  HeaderMap::iterator &it) {
  for (int d = 0; d < m_dynamicTable.size(); ++d) {
    if (m_dynamicTable[d].key == it->first) {
      if (m_dynamicTable[d].value != nullptr) {
        if (m_dynamicTable[d].value == it->second) {
          return d + 61;
        }
      }
    }
  }
  for (int i = 0; i < 61; ++i) {
    if (s_staticTable[i].key == it->first) {
      if (s_staticTable[i].value != nullptr) {
        if (s_staticTable[i].value == it->second) {
          return i;
        }
      } else {
        return i;
      }
    }
  }
  return -1;
}

v_io_size SimpleTable::findKey(const HeaderMap::iterator &it) {
  for (int i = 0; i < 61; ++i) {
    if (s_staticTable[i].key == it->first) {
      return i;
    }
  }
  for (int d = 0; d < m_dynamicTable.size(); ++d) {
    if (m_dynamicTable[d].key == it->first) {
      return d+61;
    }
  }
  return -1;
}

bool SimpleTable::keyHasValue(unsigned int idx) {
  if (idx > 60) {
    idx -= 61;
    if (idx > m_dynamicTable.size()) {
      return false;
    }
    return m_dynamicTable[idx].value != nullptr;
  }
  return s_staticTable[idx].value != nullptr;
}

const SimpleTable::TableEntry *SimpleTable::getEntry(unsigned int idx) {
  if (idx > 60) {
    idx -= 61;
    if (idx > m_dynamicTable.size()) {
      return nullptr;
    }
    return &m_dynamicTable[idx];
  }
  return &s_staticTable[idx];
}

bool SimpleTable::updateEntry(unsigned int idx,
                        const data::share::StringKeyLabelCI &key,
                        const data::share::StringKeyLabel &value) {
  if (idx > 60) {
    idx -= 61;
    if (idx > m_dynamicTable.size()) {
      return false;
    }
    m_dynamicTable[idx].key = key;
    m_dynamicTable[idx].value = value;
    return true;
  }
  m_dynamicTable[idx].key = key;
  m_dynamicTable[idx].value = value;
  return true;
}

v_io_size SimpleTable::addEntry(const data::share::StringKeyLabelCI &key, const data::share::StringKeyLabel &value) {
  m_dynamicTable.emplace_back(key, value);
  return m_dynamicTable.size() + 61;
}

v_uint32 SimpleTable::changeTableSize(v_uint32 newSize) {
  if (newSize < 61) {
    m_dynamicTable.clear();
    return 61;
  }

  if (newSize - 61 > m_dynamicTable.size()) {
    m_dynamicTable.reserve(newSize - 61);
  } else {
    m_dynamicTable = std::vector<TableEntry>(m_dynamicTable.begin(), m_dynamicTable.begin() + (newSize - 61));
  }

  return getTableSize();
}

v_uint32 SimpleTable::getTableSize() {
  return m_dynamicTable.size() + 61;
}

SimpleTable::~SimpleTable() {

}

const char* SimpleHpack::TAG = "oatpp::web::protocol::http2::hpack::SimpleHpack";

SimpleHpack::SimpleHpack(const std::shared_ptr<Table> &table)
  : m_table(table)
  , m_initialTableSize(m_table->getTableSize()) {
//  OATPP_LOGD(TAG, "Constructing Hpack(%p) with initialTableSize=%lu", this, m_table->getTableSize());
}

SimpleHpack::~SimpleHpack() {
//  OATPP_LOGD(TAG, "Destructing Hpack(%p)", this);
}

v_io_size SimpleHpack::inflateKeyValuePair(InflateMode mode,
                                           Payload::const_iterator it,
                                           Payload::const_iterator last,
                                           Headers &hdr) {
  bool requireIndex = (*it & 0x40) != 0;
  bool dontIndex = (*it & 0xf0u) == 0x10u;
  v_uint32 consumed, step;
  v_uint32 len;
//  OATPP_LOGD(TAG, "inflatehd: indexing required=%d, no_index=%d\n",
//             m_wantsIndex, m_dontWantsIndex);
//  if (m_blockMode == BlockType::KEYVALUE) {
//    ++it;
//  }
  switch (mode) {
    case INDEXED_KEY_INDEXED_VALUE:
      consumed = decodeInteger(&len, it, last, 7);
      if (consumed > 0) {
        auto kv = m_table->getEntry(len-1);
        if (kv) {
//          OATPP_LOGD(TAG, "inflateKeyValuePair: k='%s' v='%s'",kv->key.getData(), kv->value.getData());
          hdr.put(kv->key, kv->value);
        } else {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: Could not find indexed keyvalue in table");
        }
      } else {
        throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: decodeInteger signaled error");
      }
      break;
    case INDEXED_KEY_TEXT_VALUE:
      if (requireIndex) {
        step = decodeInteger(&len, it, last, 6);
      } else {
        step = decodeInteger(&len, it, last, 4);
      }
      if (step > 0) {
        auto keyentry = m_table->getEntry(len-1);
        if (!keyentry) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: Could not find indexed key in table");
        }
        it += step;

        oatpp::String value;
        consumed = inflateString(value, it, last);
        if (consumed < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: inflateString signaled error");
        }
        if (requireIndex) {
          m_table->addEntry(keyentry->key, value);
        }
        hdr.put(keyentry->key, value);
//        OATPP_LOGD(TAG, "inflateKeyValuePair: k='%s' v='%s'",keyentry->key.getData(), value->data());
      }
      else {
        throw std::runtime_error(
            "[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: decodeInteger signaled error");
      }
      consumed += step;
      break;
    case TEXT_KEY_TEXT_VALUE:
      ++it;
      {
        oatpp::String key, value;
        step = inflateString(key, it, last);
        if (step < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: inflateString signaled error");
        }
        it += step;
        consumed = inflateString(value, it, last);
        if (consumed < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: inflateString signaled error");
        }
        if (requireIndex) {
          m_table->addEntry(key, value);
        }
        consumed += step;
        hdr.put(key, value);
//        OATPP_LOGD(TAG, "inflateKeyValuePair: k='%s' v='%s'", key->data(), value->data());
      }
      break;
  }
  return consumed;
}

v_io_size SimpleHpack::inflateKeyValuePair(SimpleHpack::InflateMode mode,
                                           data::stream::BufferedInputStream *stream,
                                           v_io_size streamPayloadLength,
                                           Headers &hdr,
                                           async::Action &action) {
  v_uint8 it;
  stream->peek(&it, 1, action);
  bool requireIndex = (it & 0x40) != 0;
  bool dontIndex = (it & 0xf0u) == 0x10u;
  v_io_size consumed, step;
  v_uint32 len;

  switch (mode) {
    case INDEXED_KEY_INDEXED_VALUE:
      consumed = decodeInteger(&len, stream, streamPayloadLength, 7);
      if (consumed > 0) {
        auto kv = m_table->getEntry(len-1);
        if (kv) {
//          OATPP_LOGD(TAG, "inflateKeyValuePair: k='%s' v='%s'",kv->key.getData(), kv->value.getData());
          hdr.put(kv->key, kv->value);
        } else {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: Could not find indexed keyvalue in table");
        }
      } else {
        throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: decodeInteger signaled error");
      }
      break;
    case INDEXED_KEY_TEXT_VALUE:
      if (requireIndex) {
        step = decodeInteger(&len, stream, streamPayloadLength, 6);
      } else {
        step = decodeInteger(&len, stream, streamPayloadLength, 4);
      }
      if (step > 0) {
        auto keyentry = m_table->getEntry(len-1);
        if (!keyentry) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: Could not find indexed key in table");
        }

        oatpp::String value;
        consumed = inflateString(value, stream, streamPayloadLength, async::Action());
        if (consumed < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: inflateString signaled error");
        }
        if (requireIndex) {
          m_table->addEntry(keyentry->key, value);
        }
        hdr.put(keyentry->key, value);
//        OATPP_LOGD(TAG, "inflateKeyValuePair: k='%s' v='%s'",keyentry->key.getData(), value->data());
      }
      else {
        throw std::runtime_error(
            "[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: decodeInteger signaled error");
      }
      consumed += step;
      break;
    case TEXT_KEY_TEXT_VALUE:
      stream->commitReadOffset(1);
      {
        oatpp::String key, value;
        step = inflateString(key, stream, streamPayloadLength-1, async::Action());
        if (step < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: inflateString signaled error");
        }
        consumed = inflateString(value, stream, streamPayloadLength-1, async::Action());
        if (consumed < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePair] Error: inflateString signaled error");
        }
        if (requireIndex) {
          m_table->addEntry(key, value);
        }
        consumed += step + 1;
        hdr.put(key, value);
//        OATPP_LOGD(TAG, "inflateKeyValuePair: k='%s' v='%s'", key->data(), value->data());
      }
      break;
  }
  return consumed;
}

v_io_size SimpleHpack::inflateHandleNewTableSize(Payload::const_iterator it, Payload::const_iterator last) {
  v_uint32 res;
  v_uint32 consumed = decodeInteger(&res, it, last, 5);
  if (consumed < 1) {
    throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateHandleNewTableSize] Error: decodeInteger signaled an error");
  }
  // ToDo: Handle table size update
  return consumed;
}

v_io_size SimpleHpack::inflateHandleNewTableSize(data::stream::BufferedInputStream *stream,
                                                 v_io_size streamPayloadLength) {
  v_uint32 res;
  v_uint32 consumed = decodeInteger(&res, stream, streamPayloadLength, 5);
  if (consumed < 1) {
    throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateHandleNewTableSize] Error: decodeInteger signaled an error");
  }
  // ToDo: Handle table size update
  return consumed;
}

v_io_size SimpleHpack::inflateKeyValuePairs(Headers &hdr, const Payload &payload) {
  auto it = payload.begin();
  v_io_size consumed = 0, step;
  while ( it < payload.end()) {
    if ((*it & 0xe0u) == 0x20u) {
//      OATPP_LOGD(TAG, "inflateKeyValuePairs: Table size change");
      if (it == payload.begin()) {
        throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: header table size change must appear at the beginning of the header block");
      }
      step = inflateHandleNewTableSize(it, payload.end());
      if (step < 1) {
        throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateHandleNewTableSize signaled an error");
      }
      it += step;
    } else if (*it & 0x80u) {
//      OATPP_LOGD(TAG, "inflateKeyValuePairs: Indexed key, indexed value");
      step = inflateKeyValuePair(InflateMode::INDEXED_KEY_INDEXED_VALUE,
                                it,
                                payload.end(),
                                hdr);
      if (step < 1) {
        throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateKeyValuePair signaled an error");
      }
      it += step;
    } else {
      if (*it == 0x40u || *it == 0 || *it == 0x10u) {
//        OATPP_LOGD(TAG, "inflateKeyValuePairs: Text key, text value");
        step = inflateKeyValuePair(InflateMode::TEXT_KEY_TEXT_VALUE, it, payload.end(), hdr);
        if (step < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateKeyValuePair signaled an error");
        }
        it += step;
      } else {
//        OATPP_LOGD(TAG, "inflateKeyValuePairs: Indexed key, text value");
        step = inflateKeyValuePair(InflateMode::INDEXED_KEY_TEXT_VALUE, it, payload.end(), hdr);
        if (step < 1) {
          throw std::runtime_error("[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateKeyValuePair signaled an error");
        }
        it += step;
      }
    }
    consumed += step;
  }
  return consumed;
}

Headers SimpleHpack::inflate(const std::list<Payload> &payloads) {
  Headers headers;

  /*
   * Proxying to stream-inflate is roughly 5x slower than keeping the own implementation for a Payload.
   * Until I decide how important it is to inflate a list of Payloads instead of the input stream, I keep it for reference

  for (const auto &payload : payloads) {
    oatpp::String memoryHandle((const char*)payload.data(), payload.size());
    auto bufStream = std::make_shared<oatpp::data::stream::BufferInputStream>(memoryHandle);
    auto proxy = data::stream::InputStreamBufferedProxy::createShared(bufStream, std::make_shared<std::string>(data::buffer::IOBuffer::BUFFER_SIZE, 0));
    Headers subheaders = inflate(proxy, payload.size());
    auto all = subheaders.getAll();
    for (auto &hdr : all) {
      headers.put(hdr.first, hdr.second);
    }
  }
   */

  for (const auto &payload : payloads) {
    inflateKeyValuePairs(headers, payload);
  }

  return headers;
}

Headers SimpleHpack::inflate(const std::shared_ptr<data::stream::BufferedInputStream> &stream,
                             v_io_size streamPayloadLength) {
  Headers headers;
  async::Action action;
  v_io_size consumed = 0;
  v_io_size step;

  while (consumed < streamPayloadLength) {
    v_uint8 it;
    stream->peek(&it, 1, action);
    if ((it & 0xe0u) == 0x20u) {
//      OATPP_LOGD(TAG, "inflateKeyValuePairs: Table size change");
//      if (consumed != 0) {
//        throw std::runtime_error(
//            "[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: header table size change must appear at the beginning of the header block");
//      }
      step = inflateHandleNewTableSize(stream.get(), streamPayloadLength);
      if (step < 1) {
        throw std::runtime_error(
            "[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateHandleNewTableSize signaled an error");
      }
      consumed += step;
    } else if (it & 0x80u) {
//      OATPP_LOGD(TAG, "inflateKeyValuePairs: Indexed key, indexed value");
      step = inflateKeyValuePair(InflateMode::INDEXED_KEY_INDEXED_VALUE, stream.get(), streamPayloadLength - consumed, headers, action);
      if (step < 1) {
        throw std::runtime_error(
            "[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateKeyValuePair signaled an error");
      }
      consumed += step;
    } else {
      if (it == 0x40u || it == 0 || it == 0x10u) {
//        OATPP_LOGD(TAG, "inflateKeyValuePairs: Text key, text value");
        step = inflateKeyValuePair(InflateMode::TEXT_KEY_TEXT_VALUE, stream.get(), streamPayloadLength - consumed, headers, action);
        if (step < 1) {
          throw std::runtime_error(
              "[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateKeyValuePair signaled an error");
        }
      } else {
//        OATPP_LOGD(TAG, "inflateKeyValuePairs: Indexed key, text value");
        step = inflateKeyValuePair(InflateMode::INDEXED_KEY_TEXT_VALUE, stream.get(), streamPayloadLength - consumed, headers, action);
        if (step < 1) {
          throw std::runtime_error(
              "[oatpp::web::protocol::http2::hpack::SimpleHpack::inflateKeyValuePairs] Error: inflateKeyValuePair signaled an error");
        }
      }
      consumed += step;
    }
  }
  return headers;
}

std::shared_ptr<data::stream::BufferedInputStream> SimpleHpack::deflate(const Headers &headers) {
  std::shared_ptr<data::stream::FIFOInputStream> payload;

  auto all = headers.getAll();

  if (m_initialTableSize != m_table->getTableSize()) {
    deflateHandleNewTableSize(payload.get(), m_table->getTableSize());
    m_initialTableSize = m_table->getTableSize();
  }

  for (auto it = all.begin(); it != all.end(); ++it) {
//    OATPP_LOGD(TAG, "Deflating '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
    v_io_size sz = deflateKeyValuePair(payload.get(), it);
    if (sz < 0) {
//      OATPP_LOGD(TAG, "Exceeded framesize creating new frame payload.");
      deflateKeyValuePair(payload.get(), it);
    }
  }

  return payload;
}

v_io_size SimpleHpack::deflateKeyValuePair(data::stream::WriteCallback *to, const HeaderMap::iterator &it) {
  v_io_size idx = -1;
  IndexingMode imode = shouldIndex(it->first);

  if (imode == NEVER_INDEX) {
    idx = m_table->findKey(it);
  } else {
    idx = m_table->findKeyValue(it);
  }

//  OATPP_LOGD(TAG, "deflateKeyValuePair: Idx=%ld, IndexMode=%02x", idx, imode);

  if (idx > -1) {
    if (m_table->keyHasValue(idx) && imode != NEVER_INDEX) {
      // exact match
//      OATPP_LOGD(TAG, "deflateKeyValuePair: key and value match");
      return deflateHandleIndexedKeyValue(to, idx);
    }

    if (imode == INDEXING) {
//      OATPP_LOGD(TAG, "deflateKeyValuePair: Adding '%s: %s' to dynamic table", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
      m_table->addEntry(it->first, it->second);
    }

//    OATPP_LOGD(TAG, "deflateKeyValuePair: Handling key match '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
    return deflateHandleIndexedKey(to, idx, it->second, imode);

  } else {
    idx = m_table->findKey(it);
    if (idx > -1) {
      if (imode == INDEXING) {
//        OATPP_LOGD(TAG, "deflateKeyValuePair: Adding '%s: %s' to dynamic table", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
        m_table->addEntry(it->first, it->second);
      }

//      OATPP_LOGD(TAG, "deflateKeyValuePair: Handling key match '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
      return deflateHandleIndexedKey(to, idx, it->second, imode);
    }
  }
//  OATPP_LOGD(TAG, "deflateKeyValuePair: Handling new KeyValue '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
  return deflateHandleNewKeyValue(to, it->first, it->second, imode);

}

SimpleHpack::IndexingMode SimpleHpack::shouldIndex(const data::share::StringKeyLabelCI &key) {
  // ToDo: Make me faster by hashing or other black magic
  if (key == Header::AUTHORIZATION || key == Header::COOKIE) { // ToDo: firefox does not compress cookies with value < 20
//    OATPP_LOGD(TAG, "shouldIndex(%s): Never", (p_uint8)key.getData());
    return NEVER_INDEX;
  }
  if (key == Header::PATH || key == Header::AGE ||
      key == Header::CONTENT_LENGTH || key == Header::ETAG ||
      key == Header::IF_MODIFIED_SINCE ||
      key == Header::IF_NONE_MATCH || key == Header::LOCATION ||
      key == Header::SET_COOKIE) {
//    OATPP_LOGD(TAG, "shouldIndex(%s): No", (p_uint8)key.getData());
    return NO_INDEXING;
  }
//  OATPP_LOGD(TAG, "shouldIndex(%s): Index", (p_uint8)key.getData());
  return INDEXING;
}

v_io_size SimpleHpack::deflateHandleIndexedKey(data::stream::WriteCallback *to, v_uint32 idx, data::share::StringKeyLabel &value, IndexingMode indexing) {
  v_io_size blocklen;
  v_io_size prefixlen;
  v_io_size res;

  if (indexing == INDEXING) {
    prefixlen = 6;
  } else {
    prefixlen = 4;
  }

  blocklen = calculateEncodedLength(idx + 1, prefixlen);

  if (16 < blocklen) {
    return -1;
  }

  res = encodeInteger(to, indexing, idx + 1, prefixlen);
  if (res < 0) {
    return res;
  }

  res = deflateString(to, (p_uint8 const) value.getData(), value.getSize());
  if (res < 0) {
    return res;
  }

  return blocklen;
}

v_io_size SimpleHpack::deflateHandleNewKeyValue(data::stream::WriteCallback *to,
                                                const data::share::StringKeyLabelCI &key,
                                                data::share::StringKeyLabel &value,
                                                IndexingMode indexing) {

  v_io_size ret;

  to->writeSimple(&indexing,1);

  ret = deflateString(to, (p_uint8 const) key.getData(), key.getSize());
  if (ret < 0) {
    return ret;
  }
  ret = deflateString(to, (p_uint8 const) value.getData(), value.getSize());
  if (ret < 0) {
    return ret;
  }

  return 0;
}


v_io_size SimpleHpack::deflateHandleIndexedKeyValue(data::stream::WriteCallback *to, v_uint32 idx) {
  v_io_size blocklen;

  blocklen = calculateEncodedLength(idx + 1, 7);

  if (16 < blocklen) {
    return -1;
  }

  encodeInteger(to, 0x80, idx + 1, 7);

  return blocklen;
}

v_io_size SimpleHpack::deflateHandleNewTableSize(data::stream::WriteCallback *to, v_uint32 size) {
  v_io_size blocklen;

  blocklen = calculateEncodedLength(size, 5);

  if (16 < blocklen) {
    // ToDo: Error-Codes
    return -1;
  }

  encodeInteger(to, 0x20, size, 5);

  return blocklen;
}

v_io_size SimpleHpack::calculateEncodedLength(v_uint32 size, v_uint32 prefix) {
  size_t k = (size_t)((1 << prefix) - 1);
  size_t len = 0;

  if (size < k) {
    return 1;
  }

  size -= k;
  ++len;

  for (; size >= 128; size >>= 7, ++len)
    ;

  return len + 1;
}

v_io_size SimpleHpack::encodeInteger(data::stream::WriteCallback *to,
                                     v_uint8 flags,
                                     v_uint32 length,
                                     v_uint32 prefix) {
//  OATPP_LOGD(TAG, "encodeInteger(length=%lu, prefix=%lu)", length, prefix);
  v_io_size k = (v_io_size)((1 << prefix) - 1);
  v_io_size written = 1;
  flags = (uint8_t)(flags & ~k);

  if (length < k) {
    flags = (uint8_t)(flags | length);
    to->writeSimple(&flags, 1);
    return written;
  }

  flags = (uint8_t)(flags | k);
  to->writeSimple(&flags, 1);

  length -= k;

  for (; length >= 128; length >>= 7) {
    to->writeCharSimple((uint8_t)((1 << 7) | (length & 0x7f)));
    ++written;
  }

  to->writeCharSimple((uint8_t)length);
  ++written;

  return written;
}

v_io_size SimpleHpack::inflateString(oatpp::String &value, Payload::const_iterator it, Payload::const_iterator last) {
  v_uint32 len;
  v_io_size consumed = decodeInteger(&len, it, last, 7), second;
  bool huffman = (*it & (1 << 7)) != 0;
  it += consumed;
  if (huffman) {
    // huffman
    second = Huffman::decode(value, it, it + len);
  } else {
    // normal
    second = decodeString(value, it, it + len);
  }
  if (second < 1) {
    return second;
  }
  return second + consumed;
}

v_io_size SimpleHpack::inflateString(String &value,
                                     data::stream::BufferedInputStream *stream,
                                     v_io_size streamSize,
                                     async::Action action) {
  v_uint32 len;
  v_uint8 it;
  if (streamSize < 1) {
    OATPP_LOGE(TAG, "inflateString: data ended before decoding done");
    return -1;
  }
  stream->peek(&it, 1, action);
  v_io_size consumed = decodeInteger(&len, stream, streamSize, 7), second;
  if (streamSize < len) {
    OATPP_LOGE(TAG, "inflateString: Extracted stream-length longer than stream");
    return -1;
  }
  bool huffman = (it & (1 << 7)) != 0;
  if (huffman) {
    // huffman
    second = Huffman::decode(value, len, stream);
  } else {
    // normal
    second = decodeString(value, len, stream);
  }
  if (second < 1) {
    return second;
  }
  return second + consumed;
}

v_io_size SimpleHpack::deflateString(data::stream::WriteCallback *to, p_uint8 const str, v_uint32 len) {
//  OATPP_LOGD(TAG, "deflateString(str=\"%.*s\", len=%lu)", len, str, len);
  size_t blocklen;
  bool huffman = false;
  size_t hufflen = Huffman::calculateSize(str, len);

  if (hufflen < len) {
    huffman = true;
  } else {
    hufflen = len;
  }

  blocklen = calculateEncodedLength(hufflen, 7);

  if (16 < blocklen) {
    return -1;
  }

  encodeInteger(to, huffman ? 1 << 7 : 0, hufflen, 7);

  if (huffman) {
    Huffman::encode(to, str, len);
  } else {
    to->writeSimple(str, hufflen);
  }

  return blocklen;
}

v_io_size SimpleHpack::encodeTableSize(data::stream::WriteCallback *to, v_uint32 size) {

  size_t blocklen = calculateEncodedLength(size, 5);

  if (16 < blocklen) {
    return -1;
  }

  encodeInteger(to, 0x20, size, 5);

  return blocklen;
}

v_io_size SimpleHpack::decodeInteger(v_uint32 *res,
                                     Payload::const_iterator in,
                                     Payload::const_iterator last,
                                     size_t prefix) {
  //   auto buffer = std::make_shared<data::stream::BufferInputStream>(oatpp::String((const char*)in.base(), (v_buff_size)(last-in)));
  v_uint32 k = (uint8_t)((1 << prefix) - 1);
  v_uint32 n = 0;
  v_uint32 shift = 0;
  auto start = in;

  if ((*in & k) != k) {
    *res = (*in) & k;
    return 1;
  }
  n = k;
  if (++in == last) {
    OATPP_LOGE(TAG, "decodeInteger: data ended before decoding done");
    return -1;
  }

  for (; in != last; ++in, shift += 7) {
    uint32_t add = *in & 0x7f;

    if (shift >= 32) {
      OATPP_LOGE(TAG, "decodeInteger: shift exponent overflow");
      return -1;
    }

    if ((UINT32_MAX >> shift) < add) {
      OATPP_LOGE(TAG, "decodeInteger: integer overflow on shift");
      return -1;
    }

    add <<= shift;

    if (UINT32_MAX - add < n) {
      OATPP_LOGE(TAG, "decodeInteger: integer overflow on addition");
      return -1;
    }

    n += add;

    if ((*in & (1 << 7)) == 0) {
      break;
    }
  }


  if (in == last) {
    OATPP_LOGE(TAG, "decodeInteger: data ended before decoding done");
    return -1;
  }

  *res = n;
  return (ssize_t)(in + 1 - start);
}

v_io_size SimpleHpack::decodeInteger(v_uint32 *res,
                                     data::stream::BufferedInputStream *stream,
                                     v_io_size streamPayloadLength,
                                     v_uint32 prefix) {
  v_uint32 k = (uint8_t)((1 << prefix) - 1);
  v_uint32 n = 0;
  v_uint32 shift = 0;
  v_uint8 in;
  v_io_size consumed = 1;
  if (streamPayloadLength < 1) {
    OATPP_LOGE(TAG, "decodeInteger: data ended before decoding done");
    return -1;
  }
  stream->readExactSizeDataSimple(&in, 1);

  if ((in & k) != k) {
    *res = (in) & k;
    return consumed;
  }
  n = k;
  if (streamPayloadLength == 1) {
    OATPP_LOGE(TAG, "decodeInteger: data ended before decoding done");
    return -1;
  }

  for (; consumed < streamPayloadLength; shift += 7) {
    stream->readExactSizeDataSimple(&in, 1);
    ++consumed;
    uint32_t add = in & 0x7f;

    if (shift >= 32) {
      OATPP_LOGE(TAG, "decodeInteger: shift exponent overflow");
      return -1;
    }

    if ((UINT32_MAX >> shift) < add) {
      OATPP_LOGE(TAG, "decodeInteger: integer overflow on shift");
      return -1;
    }

    add <<= shift;

    if (UINT32_MAX - add < n) {
      OATPP_LOGE(TAG, "decodeInteger: integer overflow on addition");
      return -1;
    }

    n += add;

    if ((in & (1 << 7)) == 0) {
      break;
    }
  }

  *res = n;
  return consumed;
}

v_io_size SimpleHpack::decodeString(String &key, Payload::const_iterator in, Payload::const_iterator end) {
  v_buff_size len = (v_uint32)(end - in);
  auto *ptr = (const char*)&(*in); // ???
  key = oatpp::String(ptr, len);
  return len;
}

v_io_size SimpleHpack::decodeString(String &key,
                                    v_io_size stringSize,
                                    data::stream::BufferedInputStream *stream) {
  key = oatpp::String(stringSize);
  stream->readExactSizeDataSimple((void*)key->data(), stringSize);
  return stringSize;
}

}}}}}