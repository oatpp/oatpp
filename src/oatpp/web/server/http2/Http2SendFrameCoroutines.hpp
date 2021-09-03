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

#ifndef oatpp_web_server_http2_Http2FrameCoroutines_hpp
#define oatpp_web_server_http2_Http2FrameCoroutines_hpp

#include "oatpp/web/server/http2/PriorityStreamScheduler.hpp"
#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 { namespace framecoroutines {

template<class F>
class SendFrameCoroutine : public async::Coroutine<F> {
 protected:
  std::shared_ptr<PriorityStreamSchedulerAsync> m_output;
  protocol::http2::Frame::Header::FrameType m_type;
  v_uint32 m_streamId;
  v_uint8 m_flags;
  v_uint32 m_priority = 0;
  v_uint8 m_buf[9];
  data::buffer::InlineWriteData m_inlineData;

 public:
  virtual ~SendFrameCoroutine() = default;
  virtual const data::share::MemoryLabel* frameData() const {return nullptr;};

 public:
  SendFrameCoroutine(const std::shared_ptr<PriorityStreamSchedulerAsync> &output,
                     protocol::http2::Frame::Header::FrameType type, v_uint32 streamId = 0, v_uint8 flags = 0, v_uint32 priority = 0)
  : m_output(output)
  , m_type(type)
  , m_streamId(streamId)
  , m_flags(flags)
  , m_priority(priority)
  , m_inlineData(m_buf, 9) {
  }

  async::Action defaultAct() {
    protocol::http2::Frame::Header hdr(frameData() ? frameData()->getSize() : 0, m_flags, m_type, m_streamId);
    hdr.writeToBuffer(m_buf);
    return m_output->lock(m_priority, async::Coroutine<F>::yieldTo(&SendFrameCoroutine<F>::sendFrameHeader));
  }

  async::Action act() override {
    return defaultAct();
  }

  async::Action sendFrameHeader() {
    if (frameData() && frameData()->getSize() > 0) {
      return m_output->writeExactSizeDataAsyncInline(m_inlineData, async::Coroutine<F>::yieldTo(&SendFrameCoroutine<F>::sendFrameData));
    }
    return m_output->writeExactSizeDataAsyncInline(m_inlineData, async::Coroutine<F>::yieldTo(&SendFrameCoroutine<F>::finalize));
  }

  async::Action sendFrameData() {
    return m_output->writeExactSizeDataAsync(frameData()->getData(), frameData()->getSize()).next(async::Coroutine<F>::yieldTo(&SendFrameCoroutine<F>::finalize));
  }

  async::Action finalize() {
    OATPP_LOGD("oatpp::web::server::http2::framecoroutines::SendFrameCoroutine", "Send %s (length:%lu, flags:0x%02x, streamId:%lu)", protocol::http2::Frame::Header::frameTypeStringRepresentation(m_type), frameData() ? frameData()->getSize() : 0, m_flags, m_streamId);
    m_output->unlock();
    return async::Coroutine<F>::finish();
  }

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SendGoawayFrameCoroutine

class SendGoaway : public SendFrameCoroutine<SendGoaway> {
 private:
  data::share::MemoryLabel m_label = data::share::MemoryLabel(nullptr, buf, 8);
  v_uint8 buf[8];

 public:
  SendGoaway(const std::shared_ptr<PriorityStreamSchedulerAsync> &output, v_uint32 lastStream, protocol::http2::error::ErrorCode errorCode)
  : SendFrameCoroutine<SendGoaway>(output, protocol::http2::Frame::Header::FrameType::GOAWAY, 0, 0, PriorityStreamSchedulerAsync::PRIORITY_MAX){
    p_uint8 data = buf;
    *data++ = (lastStream >> 24) & 0xff;
    *data++ = (lastStream >> 16) & 0xff;
    *data++ = (lastStream >> 8) & 0xff;
    *data++ = lastStream & 0xff;
    *data++ = (errorCode >> 24) & 0xff;
    *data++ = (errorCode >> 16) & 0xff;
    *data++ = (errorCode >> 8) & 0xff;
    *data++ = errorCode & 0xff;
  }

  const data::share::MemoryLabel *frameData() const override {
    return &m_label;
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SendPingFrameCoroutine

class SendPing : public SendFrameCoroutine<SendPing> {
 private:
  const std::shared_ptr<data::stream::InputStream> m_input;
  v_uint8 m_data[8];
  data::share::MemoryLabel m_label = data::share::MemoryLabel(nullptr, m_data, 8);
  data::buffer::InlineReadData m_reader = data::buffer::InlineReadData(m_data, 8);
 public:
  const data::share::MemoryLabel *frameData() const override {
    return &m_label;
  }

  SendPing(const std::shared_ptr<data::stream::InputStream> &input, const std::shared_ptr<PriorityStreamSchedulerAsync> &output)
  : SendFrameCoroutine<SendPing>(output, protocol::http2::Frame::Header::FrameType::PING, 0, protocol::http2::Frame::Header::Flags::Ping::PING_ACK, PriorityStreamSchedulerAsync::PRIORITY_MAX)
  , m_input(input) {}

  async::Action act() override {
    if (m_reader.bytesLeft == 0) {
      return defaultAct();
    }
    return m_input->readExactSizeDataAsyncInline(m_reader, repeat());
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SendAckSettings

class SendAckSettings : public SendFrameCoroutine<SendAckSettings> {
 public:
  SendAckSettings(const std::shared_ptr<PriorityStreamSchedulerAsync> &output)
  : SendFrameCoroutine<SendAckSettings>(output, protocol::http2::Frame::Header::FrameType::SETTINGS, 0, protocol::http2::Frame::Header::Flags::Settings::SETTINGS_ACK, PriorityStreamSchedulerAsync::PRIORITY_MAX)
  {}
};

class SendResetStream : public SendFrameCoroutine<SendResetStream> {
 private:
  data::share::MemoryLabel m_label;
  v_uint32 m_errorCode;

 public:
  SendResetStream(const std::shared_ptr<PriorityStreamSchedulerAsync> &output, v_uint32 stream, protocol::http2::error::ErrorCode errorCode)
  : SendFrameCoroutine<SendResetStream>(output, protocol::http2::Frame::Header::FrameType::RST_STREAM, stream, 0, PriorityStreamSchedulerAsync::PRIORITY_MAX)
  , m_errorCode(htonl(errorCode))
  , m_label(nullptr, &m_errorCode, 4) {}

  const data::share::MemoryLabel *frameData() const override {
    return &m_label;
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SendSettings

class SendSettings : public SendFrameCoroutine<SendSettings> {
 private:
  const std::shared_ptr<Http2Settings> m_settings;
  v_uint8 m_data[6*6];
  data::share::MemoryLabel m_label;

 public:
  SendSettings(const std::shared_ptr<PriorityStreamSchedulerAsync> &output, const std::shared_ptr<Http2Settings> &settings)
  : SendFrameCoroutine<SendSettings>(output, protocol::http2::Frame::Header::FrameType::SETTINGS, 0, 0, PriorityStreamSchedulerAsync::PRIORITY_MAX)
  , m_settings(settings)
  , m_label(nullptr, m_data, 6*6) {
    static const Http2Settings::Identifier idents[] = {
        Http2Settings::SETTINGS_HEADER_TABLE_SIZE,
        Http2Settings::SETTINGS_ENABLE_PUSH,
        Http2Settings::SETTINGS_MAX_CONCURRENT_STREAMS,
        Http2Settings::SETTINGS_INITIAL_WINDOW_SIZE,
        Http2Settings::SETTINGS_MAX_FRAME_SIZE,
        Http2Settings::SETTINGS_MAX_HEADER_LIST_SIZE
    };
    p_uint8 dataptr = m_data;
    v_buff_size count = 0;

    for (int i = 0; i < 6; ++i) {
      if (settings->getSetting(idents[i]) != DEFAULT_SETTINGS.getSetting(idents[i])) {
        auto setting = settings->getSetting(idents[i]);
        *dataptr++ = (idents[i] >> 8) & 0xff;
        *dataptr++ = idents[i] & 0xff;
        *dataptr++ = (setting >> 24) & 0xff;
        *dataptr++ = (setting >> 16) & 0xff;
        *dataptr++ = (setting >> 8) & 0xff;
        *dataptr++ = setting & 0xff;
        count += 6;
      }
    }
    m_label = data::share::MemoryLabel(nullptr, m_data, count);
  }
};

}}}}}
#endif //oatpp_web_server_http2_Http2FrameCoroutines_hpp
