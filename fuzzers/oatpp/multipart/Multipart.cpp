/***************************************************************************
 *
 * Fuzz harness: multipart/form-data stateful stream parser.
 *
 * Threat model: a multipart body is fully attacker-controlled (any HTTP
 * client can POST one). StatefulParser is a five-state machine
 * (BOUNDARY -> AFTER_BOUNDARY -> HEADERS -> DATA -> DONE) fed by chunks as
 * they arrive from the socket; every transition depends on attacker bytes
 * matching the negotiated boundary string and "\r\n" framing.
 *
 * The harness derives the boundary from the first input line (bounded,
 * attacker-chosen, exactly like a request Content-Type boundary parameter),
 * and feeds the remainder through parseNext() in fuzz-varied chunk sizes so
 * state transitions are exercised at every alignment - the property the
 * upstream StatefulParserTest checks with fixed step sizes.
 *
 * Mutation-resistant gates ("\r\n--<boundary>" framing, part-header syntax)
 * are covered by the shipped seed corpus and dictionary.
 *
 * Expected bug classes: memory safety in the boundary-matcher and header
 * buffer handling, hangs on adversarial partial boundaries, leaks.
 ***************************************************************************/

#include "oatpp/web/mime/multipart/StatefulParser.hpp"
#include "oatpp/data/stream/BufferStream.hpp"

#include <vector>
#include <cstddef>

namespace {

class NullListener : public oatpp::web::mime::multipart::StatefulParser::Listener {
public:
  std::size_t partsFound = 0;
  std::size_t dataChunks = 0;
  std::size_t dataBytes = 0;
  void onPartHeaders(const Headers& partHeaders) override {
    ++partsFound;
    (void) partHeaders;
  }
  void onPartData(const char* data, v_buff_size size) override {
    ++dataChunks;
    dataBytes += static_cast<std::size_t>(size);
  }
};

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {

  if (size < 2) {
    return 0;
  }

  try {

    /* first line = boundary (attacker-chosen, bounded like a real
     * Content-Type boundary attribute) */
    std::size_t nl = 0;
    while (nl < size && data[nl] != '\n') {
      ++nl;
    }
    if (nl == 0 || nl >= size) {
      return 0; /* need a boundary line and body bytes after it */
    }
    std::size_t boundaryLen = nl;
    if (data[boundaryLen - 1] == '\r') {
      boundaryLen -= 1;
    }
    if (boundaryLen == 0 || boundaryLen > 128) {
      return 0;
    }

    oatpp::String boundary(reinterpret_cast<const char*>(data), boundaryLen);
    oatpp::String bodyText(reinterpret_cast<const char*>(data + nl + 1), size - nl - 1);

    auto listener = std::make_shared<NullListener>();
    oatpp::web::mime::multipart::StatefulParser parser(boundary, listener, nullptr);

    oatpp::data::stream::BufferInputStream stream(bodyText.getPtr(), bodyText->data(), bodyText->size());

    /* varied chunk sizes straddle the boundary matcher at every alignment */
    const v_buff_size step = 1 + static_cast<v_buff_size>(data[0] % 16);
    std::vector<v_char8> buffer(static_cast<std::size_t>(step));

    oatpp::v_io_size bytesRead;
    int guard = 0;
    while (((bytesRead = stream.readSimple(buffer.data(), step)) != 0) && guard++ < 100000) {
      oatpp::data::buffer::InlineWriteData inlineData(buffer.data(), bytesRead);
      while (inlineData.bytesLeft > 0 && !parser.finished()) {
        oatpp::async::Action action;
        parser.parseNext(inlineData, action);
      }
      if (parser.finished()) {
        break;
      }
    }

    (void) listener->partsFound;
    (void) listener->dataChunks;
    (void) listener->dataBytes;

  } catch (...) {
    /* malformed multipart bodies are handled via the state machine; faults
     * surfaced by ASan before an exception are findings */
  }

  return 0;

}
