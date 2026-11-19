/***************************************************************************
 *
 * Fuzz harness: HTTP protocol starting-line + header parsing.
 *
 * Threat model: this is the first code an attacker's bytes hit when they
 * reach an oatpp HTTP server or client. The request/response starting line
 * and every header value arrive raw from the wire; the parser walks them
 * through oatpp::utils::parser::Caret and stores parsed views as
 * oatpp::data::share::MemoryLabels. The whole buffer is attacker-controlled,
 * so the harness feeds arbitrary bytes as the header text, exactly as
 * RequestHeadersReader/ResponseHeadersReader would hand them over after
 * reading them from a socket.
 *
 * Expected bug classes: memory safety (out-of-bounds reads/writes in string
 * handling), unbounded/incorrect integer parsing of the version and status
 * code, pathological blowups on adversarial header blocks, and leaks from
 * the MemoryLabel shared_ptr handling.
 *
 * Exercised entry points (all previously uncovered - the web/ module was not
 * linked into the only fuzz target of this project):
 *   - http::Parser::parseRequestStartingLine / parseResponseStartingLine
 *   - http::Parser::parseHeaders / parseOneHeader (via parseHeaders)
 *   - http::Parser::parseHeaderValueData
 *   - incoming::RequestHeadersReader::readHeaders (in-memory stream)
 *   - incoming::ResponseHeadersReader::readHeaders (in-memory stream)
 ***************************************************************************/

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/incoming/RequestHeadersReader.hpp"
#include "oatpp/web/protocol/http/incoming/ResponseHeadersReader.hpp"
#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/data/stream/StreamBufferedProxy.hpp"

#include <string>
#include <cstring>

namespace {

void fuzzStartLineAndHeaders(const uint8_t* data, std::size_t size, bool asResponse) {

  oatpp::String text(reinterpret_cast<const char*>(data), size);
  oatpp::utils::parser::Caret caret(text);
  oatpp::web::protocol::http::Status status;
  oatpp::web::protocol::http::Headers headers;

  if (asResponse) {
    oatpp::web::protocol::http::ResponseStartingLine line;
    oatpp::web::protocol::http::Parser::parseResponseStartingLine(line, text.getPtr(), caret, status);
  } else {
    oatpp::web::protocol::http::RequestStartingLine line;
    oatpp::web::protocol::http::Parser::parseRequestStartingLine(line, text.getPtr(), caret, status);
  }

  /* production flow: the same caret continues into the header section */
  oatpp::web::protocol::http::Parser::parseHeaders(headers, text.getPtr(), caret, status);

  /* sub-parse every parsed header value the way content-disposition and
   * content-type handling do */
  oatpp::web::protocol::http::HeaderValueData valueData;
  for (const auto& entry : headers.getAll()) {
    oatpp::web::protocol::http::Parser::parseHeaderValueData(valueData, entry.second, ';');
    oatpp::web::protocol::http::Parser::parseHeaderValueData(valueData, entry.second, ',');
  }

}

void fuzzRequestHeadersReader(const uint8_t* data, std::size_t size) {

  oatpp::String text(reinterpret_cast<const char*>(data), size);
  auto stream = std::make_shared<oatpp::data::stream::BufferInputStream>(text);

  auto bufferMem = std::make_shared<std::string>(4096, '\0');
  oatpp::data::share::MemoryLabel bufferLabel(bufferMem);
  auto proxy = oatpp::data::stream::InputStreamBufferedProxy::createShared(stream, bufferLabel);

  oatpp::data::stream::BufferOutputStream headersBuffer;
  oatpp::web::protocol::http::incoming::RequestHeadersReader reader(&headersBuffer, 2048, 4096);

  oatpp::web::protocol::http::HttpError::Info error;
  auto result = reader.readHeaders(proxy.get(), error);
  (void) result;

}

void fuzzResponseHeadersReader(const uint8_t* data, std::size_t size) {

  /* in-memory IOStream adapter: ResponseHeadersReader reads from an
   * IOStream as a client reads from a connection */
  class MemoryIOStream : public oatpp::data::stream::IOStream {
  private:
    const v_char8* m_data;
    v_buff_size m_size;
    v_buff_size m_pos = 0;
    oatpp::data::stream::IOMode m_ioMode{};
    static oatpp::data::stream::Context& inContext() {
      static oatpp::data::stream::DefaultInitializedContext ctx(oatpp::data::stream::StreamType::STREAM_FINITE);
      return ctx;
    }
    static oatpp::data::stream::Context& outContext() {
      static oatpp::data::stream::DefaultInitializedContext ctx(oatpp::data::stream::StreamType::STREAM_INFINITE);
      return ctx;
    }
  public:
    MemoryIOStream(const v_char8* data, v_buff_size size)
      : m_data(data), m_size(size)
    {}
    oatpp::v_io_size read(void* out, v_buff_size count, oatpp::async::Action& action) override {
      (void) action;
      if (m_pos >= m_size) {
        return 0; /* end-of-file */
      }
      v_buff_size n = (count < m_size - m_pos) ? count : (m_size - m_pos);
      std::memcpy(out, m_data + m_pos, static_cast<std::size_t>(n));
      m_pos += n;
      return n;
    }
    oatpp::v_io_size write(const void*, v_buff_size count, oatpp::async::Action& action) override {
      (void) count;
      (void) action;
      return 0;
    }
    void setInputStreamIOMode(oatpp::data::stream::IOMode) override {}
    oatpp::data::stream::IOMode getInputStreamIOMode() override { return m_ioMode; }
    oatpp::data::stream::Context& getInputStreamContext() override { return inContext(); }
    void setOutputStreamIOMode(oatpp::data::stream::IOMode) override {}
    oatpp::data::stream::IOMode getOutputStreamIOMode() override { return m_ioMode; }
    oatpp::data::stream::Context& getOutputStreamContext() override { return outContext(); }
  };

  auto stream = std::make_shared<MemoryIOStream>(
    reinterpret_cast<const v_char8*>(data),
    static_cast<v_buff_size>(size)
  );

  auto bufferMem = std::make_shared<std::string>(4096, '\0');
  oatpp::data::share::MemoryLabel bufferLabel(bufferMem);

  oatpp::web::protocol::http::incoming::ResponseHeadersReader reader(bufferLabel, 4096);

  oatpp::web::protocol::http::HttpError::Info error;
  auto result = reader.readHeaders(stream, error);
  (void) result;

}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {

  if (size == 0) {
    return 0;
  }

  try {
    fuzzStartLineAndHeaders(data, size, (size & 1) != 0);
    if ((size & 2) != 0) {
      fuzzRequestHeadersReader(data, size);
    }
    if ((size & 4) != 0) {
      fuzzResponseHeadersReader(data, size);
    }
  } catch (...) {
    /* parse errors are reported via Status/Info out-params; unexpected
     * exceptions from malformed wire input are still findings if ASan fires
     * before this catches them */
  }

  return 0;

}
