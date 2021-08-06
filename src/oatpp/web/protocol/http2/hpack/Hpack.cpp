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

namespace oatpp { namespace web { namespace protocol { namespace http2 { namespace hpack {

const Table::TableEntry Table::s_staticTable[61] = {
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

v_io_size Table::findKeyValue(const  HeaderMap::iterator &it) {
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

v_io_size Table::findKey(const HeaderMap::iterator &it) {
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

bool Table::keyHasValue(unsigned int idx) {
  if (idx > 60) {
    idx -= 61;
    if (idx > m_dynamicTable.size()) {
      return false;
    }
    return m_dynamicTable[idx].value != nullptr;
  }
  return s_staticTable[idx].value != nullptr;
}

const Table::TableEntry *Table::getEntry(unsigned int idx) {
  if (idx > 60) {
    idx -= 61;
    if (idx > m_dynamicTable.size()) {
      return nullptr;
    }
    return &m_dynamicTable[idx];
  }
  return &s_staticTable[idx];
}

bool Table::updateEntry(unsigned int idx,
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

v_io_size Table::addEntry(const data::share::StringKeyLabelCI &key, const data::share::StringKeyLabel &value) {
  m_dynamicTable.emplace_back(key, value);
  return m_dynamicTable.size() + 61;
}

const char* Hpack::TAG = "oatpp::web::protocol::http2::hpack::Hpack";

Hpack::IndexingMode Hpack::shouldIndex(const data::share::StringKeyLabelCI &key) {
  // ToDo: Make me faster by hashing or other black magic
  if (key == Header::AUTHORIZATION || key == Header::COOKIE) { // ToDo: firefox does not compress cookies with value < 20
//    OATPP_LOGD(TAG, "shouldIndex(%s): Never", (p_uint8)key.getData());
    return NeverIndex;
  }
  if (key == Header::PATH || key == Header::AGE ||
      key == Header::CONTENT_LENGTH || key == Header::ETAG ||
      key == Header::IF_MODIFIED_SINCE ||
      key == Header::IF_NONE_MATCH || key == Header::LOCATION ||
      key == Header::SET_COOKIE) {
//    OATPP_LOGD(TAG, "shouldIndex(%s): No", (p_uint8)key.getData());
    return NoIndexing;
  }
//  OATPP_LOGD(TAG, "shouldIndex(%s): Index", (p_uint8)key.getData());
  return Indexing;
}

std::list<Payload> Hpack::deflate(const Headers &headers, v_io_size maxFrameSize) {
  std::list<Payload> payloads;
  payloads.emplace_back();
  auto pit = payloads.begin();
  pit->reserve(maxFrameSize);

  auto all = headers.getAll();

  for (auto it = all.begin(); it != all.end(); ++it) {
//    OATPP_LOGD(TAG, "Deflating '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
    v_io_size sz = deflateKeyValuePair(*pit, it);
    if (sz < 0) {
//      OATPP_LOGD(TAG, "Exceeded framesize creating new frame payload.");
      payloads.emplace_back();
      ++pit;
      deflateKeyValuePair(*pit, it);
    }
  }

  return payloads;
}

v_io_size Hpack::deflateKeyValuePair(Payload &to, const HeaderMap::iterator &it) {
  v_io_size idx = -1;
  IndexingMode imode = shouldIndex(it->first);

  if (imode == NeverIndex) {
    idx = m_table.findKey(it);
  } else {
    idx = m_table.findKeyValue(it);
  }

//  OATPP_LOGD(TAG, "deflateKeyValuePair: Idx=%ld, IndexMode=%02x", idx, imode);

  if (idx > -1) {
    if (m_table.keyHasValue(idx) && imode != NeverIndex) {
      // exact match
//      OATPP_LOGD(TAG, "deflateKeyValuePair: Is exact match");
      v_io_size rv = handleIndexedKeyValue(to, idx);
      if (rv < 0) {
        return rv;
      }

      return to.size();
    }

    if (imode == Indexing) {
//      OATPP_LOGD(TAG, "deflateKeyValuePair: Adding '%s: %s' to dynamic table", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
      m_table.addEntry(it->first, it->second);
    }

//    OATPP_LOGD(TAG, "deflateKeyValuePair: Handling key match '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
    return handleIndexedKey(to, idx, it->second, imode);

  } else {
    idx = m_table.findKey(it);
    if (idx > -1) {
      if (imode == Indexing) {
//        OATPP_LOGD(TAG, "deflateKeyValuePair: Adding '%s: %s' to dynamic table", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
        m_table.addEntry(it->first, it->second);
      }

//      OATPP_LOGD(TAG, "deflateKeyValuePair: Handling key match '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
      return handleIndexedKey(to, idx, it->second, imode);
    }
  }
//  OATPP_LOGD(TAG, "deflateKeyValuePair: Handling new KeyValue '%s: %s'", (p_uint8)it->first.getData(), (p_uint8)it->second.getData());
  return handleNewKeyValue(to, it->first, it->second, imode);

}

v_io_size Hpack::handleIndexedKey(Payload &to, v_uint32 idx, data::share::StringKeyLabel &value, IndexingMode indexing) {
  v_io_size blocklen;
  v_io_size prefixlen;
  v_io_size res;

  if (indexing == Indexing) {
    prefixlen = 6;
  } else {
    prefixlen = 4;
  }

  blocklen = calculateEncodedLength(idx + 1, prefixlen);

  if (16 < blocklen || (to.size() + blocklen > to.capacity())) {
    return -1;
  }

  to.emplace_back(indexing);

  res = encodeInteger(to, idx + 1, prefixlen);
  if (res < 0) {
    return res;
  }

  res = encodeString(to, (p_uint8 const)value.getData(), value.getSize());
  if (res < 0) {
    return res;
  }

  return blocklen;
}

v_io_size Hpack::handleNewKeyValue(Payload &to,
                                   const data::share::StringKeyLabelCI &key,
                                   data::share::StringKeyLabel &value,
                                   IndexingMode indexing) {

  v_io_size ret;

  if (key.getSize() + value.getSize() + 1 + to.size() > to.capacity()) {
    return -1;
  }

  to.emplace_back(indexing);

  ret = encodeString(to, (p_uint8 const)key.getData(), key.getSize());
  if (ret < 0) {
    return ret;
  }
  ret = encodeString(to, (p_uint8 const)value.getData(), value.getSize());
  if (ret < 0) {
    return ret;
  }

  return 0;
}


v_io_size Hpack::handleIndexedKeyValue(Payload &to, v_uint32 idx) {
  v_io_size blocklen;

  blocklen = calculateEncodedLength(idx + 1, 7);

  if (16 < blocklen || (to.size() + blocklen > to.capacity())) {
    return -1;
  }

  to.emplace_back(0x80u);
  encodeInteger(to, idx + 1, 7);

  return blocklen;
}

v_io_size Hpack::handleNewTableSize(Payload &to, v_uint32 size) {
  v_io_size blocklen;

  blocklen = calculateEncodedLength(size, 5);

  if (16 < blocklen || (to.size() + blocklen > to.capacity())) {
    // ToDo: Error-Codes
    return -1;
  }

  to.emplace_back(0x20u);

  encodeInteger(to, size, 5);

  return blocklen;
}

v_io_size Hpack::calculateEncodedLength(v_uint32 size, v_uint32 prefix) {
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

v_io_size Hpack::encodeInteger(Payload &to, v_uint32 length, v_uint32 prefix) {
//  OATPP_LOGD(TAG, "encodeInteger(length=%lu, prefix=%lu)", length, prefix);
  v_io_size k = (v_io_size)((1 << prefix) - 1);
  p_uint8 begin = to.data() + to.size() - 1;
  p_uint8 buf = begin;

  *buf = (uint8_t)(*buf & ~k);

  if (length < k) {
    *buf = (uint8_t)(*buf | length);
    return 1;
  }

  *buf = (uint8_t)(*buf | k);

  length -= k;

  for (; length >= 128; length >>= 7) {
    to.emplace_back((uint8_t)((1 << 7) | (length & 0x7f)));
  }

  to.emplace_back((uint8_t)length);

  return (size_t)(buf - begin);
}

v_io_size Hpack::encodeString(Payload &to, p_uint8 const str, v_uint32 len) {
//  OATPP_LOGD(TAG, "encodeString(str=\"%.*s\", len=%lu)", len, str, len);
  size_t blocklen;
  bool huffman = false;
  size_t hufflen = Huffman::calculateSize(str, len);

  if (hufflen < len) {
    huffman = true;
  } else {
    hufflen = len;
  }

  blocklen = calculateEncodedLength(hufflen, 7);

  if (16 < blocklen || (to.size() + blocklen > to.capacity())) {
    return -1;
  }

  to.emplace_back(huffman ? 1 << 7 : 0);
  encodeInteger(to, hufflen, 7);

  if (huffman) {
    Huffman::encode(to, str, len);
  } else {
    to.insert(to.end(), str, str + hufflen);
  }

  return blocklen;
}

//
//HpackEncoder::HpackEncoder(const oatpp::web::protocol::http2::Headers &headers,
//             oatpp::v_io_size maxFrameSize)
//             : m_headers(headers.getAll())
//             , m_framesize(maxFrameSize)
//             , m_hit(m_headers.begin()){
//}
//
//bool HpackEncoder::HasContinuation() {
//  return m_hit != m_headers.end();
//}
//
//void HpackEncoder::EncodeInteger(p_uint8 to, v_uint32 prefix, v_uint32 value) {
//  if (prefix < 0 | prefix > 8) {
//    throw std::runtime_error("[oatpp:: web::protocol::http2::hpack::HpackEncoder::EncodeInteger] Error: Prefix bits must be between 1 and 8");
//  }
//  v_uint32 max = PrefixMaxNumber[prefix];
//  if (value < max) {
//    *to |= value;
//  }
//}
//
//std::vector<v_uint8> HpackEncoder::GetCompressedHeaders() {
//  std::vector<v_uint8> payload(m_framesize);
//  p_uint8 data = payload.data();
//  p_uint8 end = payload.data() + m_framesize;
//  for (;m_hit != m_headers.end(); ++m_hit) {
//    int idx = Table::findKey(m_hit);
//    if (idx < 61) {
//      // found in static table
//      if (Table::keyHasValue(idx)) {
//        if (data + 1 > end) {
//          payload.resize(payload.data() - data);
//          return payload;
//        }
//        *data++ = 0x80 | (idx & 0x7f);
//      } else {
//        if (data + 2 + (m_hit->second.getSize() & 0x7F)  > end) {
//          payload.resize(payload.data() - data);
//          return payload;
//        }
//        *data++ = (idx & 0x0f) | 0x10;
//        *data++ = m_hit->second.getSize() & 0x7F;
//        memcpy(data, m_hit->second.getData(), m_hit->second.getSize() & 0x7F);
//        *data += m_hit->second.getSize() & 0x7F;
//      }
//    } else {
//      if (data + 3 + (m_hit->first.getSize() & 0x7F) + (m_hit->second.getSize() & 0x7F)  > end) {
//        payload.resize(payload.data() - data);
//        return payload;
//      }
//      *data++ = 0x10;
//      *data++ = m_hit->first.getSize() & 0x7F;
//      memcpy(data, m_hit->first.getData(), m_hit->first.getSize() & 0x7F);
//      *data += m_hit->first.getSize() & 0x7F;
//      *data++ = m_hit->second.getSize() & 0x7F;
//      memcpy(data, m_hit->second.getData(), m_hit->second.getSize() & 0x7F);
//      *data += m_hit->second.getSize() & 0x7F;
//    }
//  }
//  payload.resize(payload.data() - data);
//  return payload;
//}
//
//Headers HpackDecoder::DecompressHeaders(const std::vector<v_uint8> &compressedData) {
//  oatpp::web::protocol::http2::Headers hdrs;
//  auto it = compressedData.begin();
//  while (it != compressedData.end()) {
//    if (((*it) & 0x0f) == 0) {
//      v_uint8 ksize = *it;
//      ++it;
//      auto kptr = &(*it);
//      it += ksize;
//      v_uint8 vsize = *it;
//      ++it;
//      auto vptr = &(*it);
//      hdrs.put({nullptr, (const char*)kptr, ksize}, {nullptr, (const char*)vptr, vsize});
//      it += vsize;
//    } else if ((*it) & 0x80) {
//      auto entry = Table::getEntry((*it) & 0x7f);
//      hdrs.put(entry->key, entry->value);
//      ++it;
//    } else if ((*it) & 0x) {
//
//    }
//  }
//}

}}}}}