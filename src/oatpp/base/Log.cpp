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

#include "Log.hpp"

#include "oatpp/utils/parser/Caret.hpp"
#include "oatpp/macro/basic.hpp"

namespace oatpp::base{

LogMessage::LogMessage(const oatpp::String& msg)
  : m_msg(msg != nullptr ? msg : "<null>")
  , m_stream(256)
  , m_currParam(0)
{

  utils::parser::Caret caret(m_msg);
  while (caret.canContinue()) {

    if(caret.findText("{}", 2)) {
      m_params.push_back({caret.getPosition(), caret.getPosition() + 2});
      caret.inc(2);
    }

  }

}

std::string LogMessage::toStdString() const {
  if(m_currParam == 0) {
    m_stream.writeSimple(m_msg->data(), static_cast<v_buff_size>(m_msg->size()));
  } else if(m_currParam > 0) {
    auto& prev = m_params.at(m_currParam - 1);
    m_stream.writeSimple(m_msg->data() + prev.endPos, static_cast<v_buff_size>(m_msg->size()) - prev.endPos);
  }
  return m_stream.toStdString();
}

bool LogMessage::writeNextChunk() {

  if(m_currParam >= m_params.size()) return false;

  if(m_currParam == 0) {
    auto& curr = m_params.at(m_currParam);
    m_stream.writeSimple(m_msg->data(), curr.startPos);
  } else if(m_currParam > 0) {
    auto& prev = m_params.at(m_currParam - 1);
    auto& curr = m_params.at(m_currParam);
    m_stream.writeSimple(m_msg->data() + prev.endPos, curr.startPos - prev.endPos);
  }

  m_currParam ++;
  return true;

}

LogMessage& LogMessage::operator << (const char* str) {
  if(writeNextChunk()) {
    if(str != nullptr) {
      m_stream.writeSimple(str);
    } else {
      m_stream.writeSimple("{<char*(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (bool value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_int8 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_uint8 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_int16 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_uint16 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_int32 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_uint32 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_int64 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_uint64 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_float32 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (v_float64 value) {
  if(writeNextChunk()) {
    m_stream.writeAsString(value);
  }
  return *this;
}

LogMessage& LogMessage::operator << (const oatpp::String& str) {
  if(writeNextChunk()) {
    if(str != nullptr) {
      m_stream.writeSimple(str);
    } else {
      m_stream.writeSimple("{<String(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const Boolean& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<Boolean(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const Int8& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<Int8(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const UInt8& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<UInt8(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const Int16& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<Int16(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const UInt16& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<UInt16(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const Int32& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<Int32(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const UInt32& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<UInt32(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const Int64& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<Int64(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const UInt64& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<UInt64(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const Float32& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<Float32(null)>}");
    }
  }
  return *this;
}

LogMessage& LogMessage::operator << (const Float64& value) {
  if(writeNextChunk()) {
    if(value.get() != nullptr) {
      m_stream.writeAsString(*value);
    } else {
      m_stream.writeSimple("{<Float64(null)>}");
    }
  }
  return *this;
}

void LogMessage::log(v_uint32 priority, const std::string& tag, const LogMessage& message) {
  oatpp::Environment::log(priority, tag, message.toStdString());
}

}
